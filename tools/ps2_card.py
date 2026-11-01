#!/usr/bin/env python3
"""ps2_card.py - a formatted, empty 8 MB PS2 memory card image.

The fork's sessions start with fresh card files, which the game sees as
unformatted, and its Format fails on them (the port's docs/SAVE_GAME.md 6.2).
This writes what a format leaves: the superblock, the indirect FAT, the FAT and
a root directory holding only its "." and ".." entries, with each page's ECC.
Every other page is erased. Nothing here comes from the disc.

Usage (native arm64 macOS): python3 tools/ps2_card.py <out.ps2>
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


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    write(sys.argv[1])
