#!/usr/bin/env python3
"""ps2_card.py - a formatted, empty 8 MB PS2 memory card image.

The fork's sessions start with fresh card files, which the game sees as
unformatted, and its Format fails on them (the port's docs/SAVE_GAME.md 6.2).
This writes what a format leaves: the superblock, the indirect FAT, the FAT and
a root directory holding only its "." and ".." entries, with each page's ECC.
Every other page is erased. Nothing here comes from the disc.

Usage (native arm64 macOS): python3 tools/ps2_card.py <out.ps2> [<save directory>]
(with a save directory: the card holds that one directory and its files)
"""
import struct
import sys
from pathlib import Path

PAGE = 512
SPARE = 16
PAGES_PER_CLUSTER = 2
PAGES_PER_BLOCK = 16
CLUSTERS = 8192
PAGES = CLUSTERS * PAGES_PER_CLUSTER
BLOCKS = PAGES // PAGES_PER_BLOCK
IFC_CLUSTER = 8                 # the indirect FAT's cluster
FAT_CLUSTERS = 32               # the FAT's, right after it
ALLOC_OFFSET = IFC_CLUSTER + 1 + FAT_CLUSTERS
ALLOC_END = CLUSTERS - 2 * PAGES_PER_BLOCK // PAGES_PER_CLUSTER - ALLOC_OFFSET
FREE, CHAIN_END = 0x7FFFFFFF, 0xFFFFFFFF
FLAGS = 0x2B                    # ECC, bad block list, erased pages read 0xFF

PARITY = [bin(i).count('1') & 1 for i in range(256)]
COLUMN = [sum(PARITY[b & mask] << i for i, mask in enumerate((0x55, 0x33, 0x0F, 0x00, 0xAA, 0xCC, 0xF0)))
          for b in range(256)]


def ecc(chunk):
    column, line0, line1 = 0x77, 0x7F, 0x7F
    for i, b in enumerate(chunk):
        column ^= COLUMN[b]
        if PARITY[b]:
            line0 ^= ~i & 0xFF
            line1 ^= i
    return bytes((column, line0 & 0x7F, line1))


def page(data):
    data = data.ljust(PAGE, b'\xff')
    return data + b''.join(ecc(data[i:i + 128]) for i in range(0, PAGE, 128)) + bytes(SPARE - 12)


def superblock():
    head = b'Sony PS2 Memory Card Format 1.2.0.0'.ljust(40, b'\0')
    head += struct.pack('<4H', PAGE, PAGES_PER_CLUSTER, PAGES_PER_BLOCK, 0xFF00)
    head += struct.pack('<6I', CLUSTERS, ALLOC_OFFSET, ALLOC_END, 0, BLOCKS - 1, BLOCKS - 2)
    head = head.ljust(0x50, b'\0')
    head += struct.pack('<32I', IFC_CLUSTER, *[CHAIN_END] * 31)
    head += b'\xff' * (32 * 4)
    return head + bytes((2, FLAGS))


def dirent(mode, length, name):
    when = bytes((0, 0, 0, 0, 1, 1)) + struct.pack('<H', 2001)
    return struct.pack('<HHI8sII8sI28x32s416x', mode, 0, length, when, 0, 0, when, 0, name)


def image():
    pages = {0: page(superblock())}
    words = [IFC_CLUSTER + 1 + i for i in range(FAT_CLUSTERS)]
    table = struct.pack(f'<{len(words)}I', *words)
    pages[IFC_CLUSTER * PAGES_PER_CLUSTER] = page(table.ljust(PAGE, b'\xff'))
    pages[IFC_CLUSTER * PAGES_PER_CLUSTER + 1] = page(b'\xff' * PAGE)
    fat = [CHAIN_END] + [FREE] * (ALLOC_END - 1)
    fat += [CHAIN_END] * (FAT_CLUSTERS * 256 - len(fat))
    fat = struct.pack(f'<{len(fat)}I', *fat)
    first = (IFC_CLUSTER + 1) * PAGES_PER_CLUSTER
    for i in range(0, len(fat), PAGE):
        pages[first + i // PAGE] = page(fat[i:i + PAGE])
    root = ALLOC_OFFSET * PAGES_PER_CLUSTER
    pages[root] = page(dirent(0x8427, 2, b'.'))
    pages[root + 1] = page(dirent(0xA426, 0, b'..'))
    erased = b'\xff' * (PAGE + SPARE)
    return b''.join(pages.get(i, erased) for i in range(PAGES))


def write(path):
    Path(path).write_bytes(image())


CLUSTER = PAGE * PAGES_PER_CLUSTER
DIR_MODE, FILE_MODE, DOTDOT_MODE = 0x8427, 0x8497, 0xA426


def dirent_at(mode, length, cluster, dir_entry, name):
    when = bytes((0, 0, 0, 0, 1, 1)) + struct.pack('<H', 2001)
    return struct.pack('<HHI8sII8sI28x32s416x', mode, 0, length, when, cluster, dir_entry, when, 0, name)


def image_with_dir(dirname, files):
    """A formatted card holding one directory in the root with `files`
    ({name: bytes}, in that order): what a save to a fresh card leaves, laid
    out by this tool (clusters allocated in order from the first free one).
    Directory entries: the directory's own entry in the root (length = its
    entry count, cluster = its first cluster), its "." (the parent's cluster
    0 and this directory's index 2 in the root) and ".."; files' entries
    carry their length and first cluster. FAT entries: 0x80000000 | next,
    the last 0xFFFFFFFF."""
    base = bytearray(image())
    raw = PAGE + SPARE

    def put_page(n, data):
        base[n * raw:(n + 1) * raw] = page(data)

    fat = [0x80000000 | 1, CHAIN_END] + [FREE] * (ALLOC_END - 2)   # the root grows to two clusters
    nxt = [2]

    def alloc(nbytes):
        count = max(1, -(-nbytes // CLUSTER))
        first = nxt[0]
        for k in range(count):
            c = first + k
            fat[c] = CHAIN_END if k == count - 1 else (0x80000000 | (c + 1))
        nxt[0] += count
        return first, count

    def put(first, data):
        data = bytes(data)
        for k in range(0, max(len(data), 1), PAGE):
            put_page((ALLOC_OFFSET + first) * PAGES_PER_CLUSTER + k // PAGE, data[k:k + PAGE])

    names = list(files)
    dir_entries = 2 + len(names)
    dir_first, _n = alloc(dir_entries * PAGE)
    entries = [dirent_at(DIR_MODE, 0, 0, 2, b'.'), dirent_at(DOTDOT_MODE, 0, 0, 0, b'..')]
    for name in names:
        first, _n = alloc(len(files[name]))
        put(first, files[name])
        entries.append(dirent_at(FILE_MODE, len(files[name]), first, 0, name.encode()))
    put(dir_first, b''.join(entries))
    root = ALLOC_OFFSET * PAGES_PER_CLUSTER
    put_page(root, dirent(0x8427, 3, b'.'))
    put_page(root + 2, dirent_at(DIR_MODE, dir_entries, dir_first, 0, dirname.encode()))
    words = fat + [CHAIN_END] * (FAT_CLUSTERS * 256 - len(fat))
    blob = struct.pack(f'<{len(words)}I', *words)
    first = (IFC_CLUSTER + 1) * PAGES_PER_CLUSTER
    for i in range(0, len(blob), PAGE):
        put_page(first + i // PAGE, blob[i:i + PAGE])
    return bytes(base)


def write_save(path, folder):
    """A card image holding the save directory `folder` (a host card's
    slot folder, e.g. the port's build/start_saves/<name>/slot1/<dir>)."""
    folder = Path(folder)
    files = {p.name: p.read_bytes() for p in sorted(folder.iterdir()) if p.is_file()}
    Path(path).write_bytes(image_with_dir(folder.name, files))


if __name__ == '__main__':
    if len(sys.argv) == 3:
        write_save(sys.argv[1], sys.argv[2])
    elif len(sys.argv) == 2:
        write(sys.argv[1])
    else:
        sys.exit(__doc__)
