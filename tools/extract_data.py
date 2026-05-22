#!/usr/bin/env python3
"""Extermination (SCUS-97112) DATA.DAT / INDEX.IDX archive tool.

Original tooling for the Extermination decompilation project. This script reads
only files the user supplies from their own legally-dumped disc; it redistributes
nothing and embeds no disc-derived data.

Archive layout (reverse-engineered; region map validated by exact tiling):

  INDEX.IDX -- N sectors of 0x800 bytes; sector i is the descriptor for chunk i.
  DATA.DAT  -- the payload the descriptors point into.

  Descriptor layout (a 0x800 top-level sector, or a 0x70 nested block, share it):
    +0x00  u32 id
    +0x04  u32 off          absolute byte offset into DATA.DAT
    +0x08  u32 size         region size in DATA.DAT
    +0x0C  u32 flags        bit 0x10000 / bit 0x1 each add an 8-byte block
                            before the file-entry table
    +0x14  u32 size2        secondary size
    +0x18  u32 nested_count count of nested sub-chunk blocks (top-level only)
    +0x1C  u32 file_count   count of file entries
    +0x20  file-entry table (after the optional flag blocks): each entry is
           <u24 region-relative offset, u8 file_id>, sorted by offset
    +0x100 nested sub-chunk blocks: nested_count blocks of 0x70 bytes each

  Top-level chunks plus all nested sub-chunk blocks tile DATA.DAT exactly.
  Within each region, the file entries tile that region exactly.

Usage:
  extract_data.py list      --disc /Volumes/Untitled
  extract_data.py regions   --disc /Volumes/Untitled [-v]
  extract_data.py files     --disc /Volumes/Untitled [-v]
  extract_data.py extract   --disc /Volumes/Untitled --out extract/
  extract_data.py inspect 4 --disc /Volumes/Untitled
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

SECTOR = 0x800
NESTED_BASE = 0x100
NESTED_STRIDE = 0x70


def u32(buf: bytes, pos: int) -> int:
    return struct.unpack_from("<I", buf, pos)[0]


def entry_table_start(desc: bytes) -> int:
    """Offset of the file-entry table within a descriptor.

    The table follows +0x20, after up to three optional 8-byte blocks. One
    block each is present for: a non-zero word at +0x10, the flags bit 0x10000,
    and the flags bit 0x1.
    """
    flags = u32(desc, 0x0C)
    extra = 0
    if u32(desc, 0x10):
        extra += 8
    if flags & 0x10000:
        extra += 8
    if flags & 0x1:
        extra += 8
    return 0x20 + extra


class Region:
    """A leaf span of DATA.DAT (a top-level chunk or a nested sub-chunk).

    `desc` is the region's descriptor bytes -- a 0x800 sector for a top-level
    chunk, a 0x70 block for a nested sub-chunk -- which carries its file table.
    """
    __slots__ = ("id", "off", "size", "flags", "label", "desc")

    def __init__(self, desc: bytes, label: str):
        self.id, self.off, self.size, self.flags = struct.unpack_from("<4I", desc)
        self.label = label
        self.desc = desc

    @property
    def end(self) -> int:
        return self.off + self.size

    def file_entries(self) -> list[tuple[int, int]]:
        """Return [(file_id, region-relative offset), ...] sorted by offset."""
        count = u32(self.desc, 0x1C)
        start = entry_table_start(self.desc)
        out = []
        for i in range(count):
            pos = start + 4 * i
            if pos + 4 > len(self.desc):
                break
            w = u32(self.desc, pos)
            out.append((w >> 24, w & 0xFFFFFF))
        return out

    def files(self) -> list[tuple[int, int, int]]:
        """Return [(file_id, rel_offset, size), ...] for the region's files.

        With no file entries the whole region is treated as a single file.
        """
        entries = self.file_entries()
        if not entries:
            return [(0, 0, self.size)]
        out = []
        for i, (fid, off) in enumerate(entries):
            nxt = entries[i + 1][1] if i + 1 < len(entries) else self.size
            out.append((fid, off, nxt - off))
        return out


class Chunk:
    __slots__ = ("sector", "region", "nested_count", "nested")

    def __init__(self, sector: int, raw: bytes):
        self.sector = sector
        self.region = Region(raw, f"chunk{sector:02d}")
        self.nested_count = u32(raw, 0x18)
        self.nested: list[Region] = []
        for i in range(self.nested_count):
            pos = NESTED_BASE + NESTED_STRIDE * i
            block = raw[pos:pos + NESTED_STRIDE]
            if len(block) >= 0x20:
                self.nested.append(Region(block, f"chunk{sector:02d}.n{i}"))


def resolve_paths(args) -> tuple[Path, Path]:
    if args.index and args.data:
        return Path(args.index), Path(args.data)
    if args.disc:
        d = Path(args.disc)
        for base in (d / "DATA", d):
            idx, dat = base / "INDEX.IDX", base / "DATA.DAT"
            if idx.is_file() and dat.is_file():
                return idx, dat
        raise SystemExit(f"could not find DATA/INDEX.IDX + DATA/DATA.DAT under {d}")
    raise SystemExit("specify --disc DIR, or both --index FILE and --data FILE")


def read_chunks(index_path: Path) -> list[Chunk]:
    blob = index_path.read_bytes()
    if not blob or len(blob) % SECTOR:
        raise SystemExit(f"{index_path}: size {len(blob)} is not a multiple of {SECTOR:#x}")
    return [Chunk(i, blob[i * SECTOR:(i + 1) * SECTOR]) for i in range(len(blob) // SECTOR)]


def all_regions(chunks: list[Chunk]) -> list[Region]:
    """Every leaf region (top-level chunk + nested sub-chunk), offset-sorted."""
    regions = [c.region for c in chunks if c.region.size]
    for c in chunks:
        regions += [n for n in c.nested if n.size]
    regions.sort(key=lambda r: r.off)
    return regions


def check_tiling(regions: list[Region], data_size: int) -> list[str]:
    problems: list[str] = []
    cursor = 0
    for r in regions:
        if r.off < cursor:
            problems.append(f"{r.label}: off {r.off:#x} overlaps previous (cursor {cursor:#x})")
        elif r.off > cursor:
            problems.append(f"gap {cursor:#x}..{r.off:#x} ({r.off - cursor:#x} bytes) before {r.label}")
        cursor = max(cursor, r.end)
    if cursor != data_size:
        problems.append(f"coverage ends at {cursor:#x}, DATA.DAT is {data_size:#x}")
    return problems


def check_files(region: Region) -> list[str]:
    """Verify the region's file entries tile [0, region.size) exactly."""
    problems: list[str] = []
    cursor = 0
    for fid, off, size in region.files():
        if off != cursor:
            problems.append(f"{region.label}: file id {fid:#x} at {off:#x}, expected {cursor:#x}")
        if size < 0:
            problems.append(f"{region.label}: file id {fid:#x} has negative size")
        cursor = off + size
    if cursor != region.size:
        problems.append(f"{region.label}: files end at {cursor:#x}, region is {region.size:#x}")
    return problems


def cmd_list(args) -> int:
    index_path, data_path = resolve_paths(args)
    chunks = read_chunks(index_path)
    print(f"INDEX.IDX : {index_path}  ({len(chunks)} chunks)")
    print(f"DATA.DAT  : {data_path}  ({data_path.stat().st_size:#x} bytes)")
    print()
    print(f"{'chunk':>5} {'offset':>10} {'size':>10} {'flags':>9} {'nested':>7} {'files':>6}")
    for c in chunks:
        r = c.region
        print(f"{c.sector:>5} {r.off:>#10x} {r.size:>#10x} {r.flags:>#9x} "
              f"{c.nested_count:>7} {len(r.file_entries()):>6}")
    return 0


def cmd_regions(args) -> int:
    index_path, data_path = resolve_paths(args)
    regions = all_regions(read_chunks(index_path))
    data_size = data_path.stat().st_size
    problems = check_tiling(regions, data_size)
    print(f"{len(regions)} leaf regions across DATA.DAT ({data_size:#x} bytes)")
    if args.verbose:
        print()
        print(f"{'#':>4} {'start':>10} {'size':>10} {'flags':>9}  label")
        for i, r in enumerate(regions):
            print(f"{i:>4} {r.off:>#10x} {r.size:>#10x} {r.flags:>#9x}  {r.label}")
    print()
    if problems:
        print(f"REGION TILING INCOMPLETE ({len(problems)} issue(s)):")
        for p in problems[:40]:
            print(f"  - {p}")
        return 1
    print("region map validated: top-level + nested regions tile DATA.DAT exactly.")
    return 0


def cmd_files(args) -> int:
    index_path, _ = resolve_paths(args)
    regions = all_regions(read_chunks(index_path))
    total_files = 0
    problems: list[str] = []
    for r in regions:
        files = r.files()
        total_files += len(files)
        problems += check_files(r)
        if args.verbose:
            for fid, off, size in files:
                print(f"  {r.label:<16} file id 0x{fid:02x}  off {off:>#9x}  size {size:>#9x}")
    print(f"\n{total_files} files across {len(regions)} regions")
    if problems:
        print(f"FILE TILING INCOMPLETE ({len(problems)} issue(s)):")
        for p in problems[:40]:
            print(f"  - {p}")
        return 1
    print("file map validated: file entries tile every region exactly.")
    return 0


def cmd_extract(args) -> int:
    index_path, data_path = resolve_paths(args)
    chunks = read_chunks(index_path)
    regions = all_regions(chunks)
    data_size = data_path.stat().st_size
    problems = check_tiling(regions, data_size)
    if problems and not args.force:
        print(f"refusing to extract: region map incomplete ({len(problems)} issue(s)).")
        print("run `regions` to see them, or pass --force.")
        return 1

    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    manifest = []
    n_files = 0
    with data_path.open("rb") as f:
        for r in regions:
            region_dir = out / r.label
            region_dir.mkdir(exist_ok=True)
            for i, (fid, off, size) in enumerate(r.files()):
                f.seek(r.off + off)
                payload = f.read(size)
                if len(payload) != size:
                    print(f"{r.label} file {i}: short read ({len(payload)} of {size})")
                    return 1
                name = f"f{i:02d}_id{fid:02x}.bin"
                (region_dir / name).write_bytes(payload)
                manifest.append(f"{r.label}/{name}\t{r.off + off:#x}\t{size:#x}\t"
                                f"region_flags={r.flags:#x}")
                n_files += 1
    (out / "manifest.txt").write_text("\n".join(manifest) + "\n")
    print(f"extracted {n_files} files from {len(regions)} regions to {out}/")
    print(f"manifest: {out / 'manifest.txt'}")
    return 0


def cmd_inspect(args) -> int:
    index_path, _ = resolve_paths(args)
    chunks = read_chunks(index_path)
    if not (0 <= args.sector < len(chunks)):
        raise SystemExit(f"sector {args.sector} out of range 0..{len(chunks) - 1}")
    c = chunks[args.sector]
    r = c.region
    print(f"chunk {c.sector}: off={r.off:#x} size={r.size:#x} flags={r.flags:#x} "
          f"nested={c.nested_count} files={len(r.file_entries())}")
    for fid, off, size in r.files():
        print(f"  file id 0x{fid:02x}: off {off:#x} size {size:#x}")
    for n in c.nested:
        print(f"  nested {n.label}: off={n.off:#x} size={n.size:#x} flags={n.flags:#x} "
              f"files={len(n.file_entries())}")
        for fid, off, size in n.files():
            print(f"    file id 0x{fid:02x}: off {off:#x} size {size:#x}")
    return 0


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination DATA.DAT/INDEX.IDX archive tool")
    sub = p.add_subparsers(dest="cmd", required=True)

    def add_disc_args(sp):
        sp.add_argument("--disc", help="disc root or DATA/ directory (e.g. /Volumes/Untitled)")
        sp.add_argument("--index", help="explicit path to INDEX.IDX")
        sp.add_argument("--data", help="explicit path to DATA.DAT")

    sp = sub.add_parser("list", help="print the top-level chunk table")
    add_disc_args(sp)
    sp.set_defaults(func=cmd_list)

    sp = sub.add_parser("regions", help="build and validate the leaf-region map")
    add_disc_args(sp)
    sp.add_argument("-v", "--verbose", action="store_true", help="print every region")
    sp.set_defaults(func=cmd_regions)

    sp = sub.add_parser("files", help="build and validate the per-region file map")
    add_disc_args(sp)
    sp.add_argument("-v", "--verbose", action="store_true", help="print every file")
    sp.set_defaults(func=cmd_files)

    sp = sub.add_parser("extract", help="extract every file of DATA.DAT")
    add_disc_args(sp)
    sp.add_argument("--out", default="extract", help="output directory (default: extract/)")
    sp.add_argument("--force", action="store_true", help="extract even if tiling is incomplete")
    sp.set_defaults(func=cmd_extract)

    sp = sub.add_parser("inspect", help="dump one chunk's regions and files")
    sp.add_argument("sector", type=int, help="sector / chunk index to inspect")
    add_disc_args(sp)
    sp.set_defaults(func=cmd_inspect)

    args = p.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
