#!/usr/bin/env python3
"""Place a compiled overlay function's local .rodata (jump tables) where the
original has it, for tools/overlay/link_overlay.py.

WHY. An overlay is linked 0x40 below the address it runs at: the MWo3 header
is loaded first at the arena base 0x00823500, so the payload byte at link
address L runs at L + 0x40. The original code stores RUNTIME addresses: a
switch dispatcher builds its jump table's runtime address with a lui/addiu
pair, and the table's entries are the runtime addresses of the case labels.
The splat assembly reproduces both as plain numbers, so the assembled
function and its table (inside the data-section object) link byte-identically.
A switch compiled from C instead carries the table in its own .rodata, with
the table address and the entries as relocations. Left to the default
script, GNU ld put that .rodata after the data section (outside the image)
and resolved both at LINK addresses, 0x40 low; such dispatchers had to stay
NEARMISS.

HOW (exact placement, not substitution; same idea as the boot ELF's
tools/decomp/rodata_pin.py).
  1. plan_object() proves, for each code object with a non-empty .rodata, the
     single original RUNTIME address T of every .rodata section: each .text
     HI16/LO16 pair against the section is compared with the original
     instruction pair at the same place (the compiled text sits in its own
     slot), T = original target - in-place addend - symbol value. Every pair
     must agree. The section's own relocations (R_MIPS_32 against a symbol of
     the object's .text, the case labels) are resolved at runtime addresses
     (link + 0x40), and the result must equal the original bytes at T. T must
     honour the section alignment and T - 0x40 must lie inside one data-section
     object.
  2. write_pinned_copy() writes a link copy of the object with the +0x40 run
     bias added to exactly those in-place addends (the text HI16/LO16 pairs
     against a pinned section, and the pinned sections' R_MIPS_32 entries), and
     renames each pinned section to `.ovlpin.<func>.<k>` so the linker script
     can place it by name.
  3. split_data() cuts the data-section object around the link spans
     [T - 0x40, T - 0x40 + size) into pieces. link_overlay.py emits the pieces
     and the pinned sections in address order inside the .data output section,
     so the COMPILED table bytes are what ld writes at the original place, the
     relocations resolve to the original runtime values, and nothing else
     moves. Data symbols inside a carved span become absolute definitions at
     their (unchanged) link address.

Any object that fails a check stops the link with the reason: a compiled
.rodata that cannot be proven has no correct placement. The whole-overlay
byte comparison in link_overlay.py remains the final arbiter.
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "decomp"))
from rodata_pin import (ElfObj, _pack_object, _s16,  # noqa: E402
                        R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16,
                        SHN_UNDEF, SHN_ABS)

HDR = 0x40          # MWo3 header size = run bias (runtime = link + 0x40)
PIN_PREFIX = ".ovlpin."
UNPINNABLE_DATA = (".data", ".sdata", ".sbss")


class PinError(Exception):
    pass


def _u32(b: bytes, off: int) -> int:
    return struct.unpack_from("<I", b, off)[0]


def plan_object(stem: str, obj_path: Path, link_vram: int, load: int,
                orig: bytes, regions: list[tuple[int, int, str]]) -> dict | None:
    """Pin plan for one filler code object, or None if it has no .rodata.

    `orig` is the original overlay file (MWo3 header included), so the byte
    at link address L is orig[HDR + L - load] and the byte at runtime address
    T is orig[T - load]. Raises PinError when the placement cannot be proven.
    """
    obj = ElfObj(obj_path)
    rodata = [i for i in obj.sections_named(".rodata") if obj.sh[i][5]]
    if not rodata:
        return None
    for sec in UNPINNABLE_DATA:
        for i in obj.sections_named(sec):
            if obj.sh[i][5]:
                raise PinError(f"{stem}: non-empty {sec} next to .rodata")
    text = obj.index_of(".text")
    if text is None:
        raise PinError(f"{stem}: .rodata without .text")
    tbytes = obj.raw(text)

    def orig_link(addr: int, size: int) -> bytes:
        off = HDR + addr - load
        if off < HDR or off + size > len(orig):
            raise PinError(f"{stem}: link 0x{addr:08x} outside the overlay")
        return orig[off:off + size]

    # 1. runtime base of each section from its HI16/LO16 pairs
    bases: dict[int, set[int]] = {i: set() for i in rodata}
    pairs: dict[int, list[tuple[int, int, int]]] = {i: [] for i in rodata}
    hi_pending: dict[int, tuple[int, int, int]] = {}
    for off, kind, si in obj.relocs(text):
        sym = obj.syms[si]
        sec = sym["shndx"]
        if sec not in bases:
            continue
        if kind not in (R_MIPS_HI16, R_MIPS_LO16):
            raise PinError(f"{stem}: text relocation type {kind} against .rodata at +0x{off:x}")
        compiled = _u32(tbytes, off)
        original = _u32(orig_link(link_vram + off, 4), 0)
        if kind == R_MIPS_HI16:
            hi_pending[sec] = (off, compiled, original)
            continue
        if sec not in hi_pending:
            raise PinError(f"{stem}: LO16 without a preceding HI16 at +0x{off:x}")
        hoff, hi_c, hi_o = hi_pending[sec]
        addend = ((hi_c & 0xFFFF) << 16) + _s16(compiled)
        target = ((hi_o & 0xFFFF) << 16) + _s16(original)
        bases[sec].add((target - addend - sym["value"]) & 0xFFFFFFFF)
        pairs[sec].append((hoff, off, addend))

    sections = []
    for sec in rodata:
        if len(bases[sec]) != 1:
            raise PinError(f"{stem}: .rodata section {sec} has {len(bases[sec])} "
                           f"candidate addresses (needs exactly one)")
        T = next(iter(bases[sec]))
        size, align = obj.sh[sec][5], max(1, obj.sh[sec][8])
        if T % align:
            raise PinError(f"{stem}: 0x{T:08x} violates .rodata alignment {align}")
        # 2. resolve the entries at runtime addresses; compare with the original
        data = bytearray(obj.raw(sec))
        entries = []
        for off, kind, si in obj.relocs(sec):
            sym = obj.syms[si]
            if kind != R_MIPS_32 or sym["shndx"] != text:
                raise PinError(f"{stem}: .rodata relocation type {kind} against "
                               f"'{sym['name']}' is not a local text address")
            addend = _u32(data, off)
            struct.pack_into("<I", data, off,
                             (link_vram + sym["value"] + addend + HDR) & 0xFFFFFFFF)
            entries.append(off)
        L = T - HDR
        if bytes(data) != orig_link(L, size):
            raise PinError(f"{stem}: .rodata section {sec} differs from the original "
                           f"at runtime 0x{T:08x}")
        region = next((u for lo, hi, u in regions if lo <= L and L + size <= hi), None)
        if region is None:
            raise PinError(f"{stem}: link span 0x{L:08x}+0x{size:x} is not inside "
                           f"one data-section object")
        sections.append(dict(index=sec, runtime=T, link=L, size=size, align=align,
                             region=region, pairs=pairs[sec], entries=entries,
                             name=f"{PIN_PREFIX}{stem}.{len(sections)}"))
    return dict(stem=stem, link_vram=link_vram, sections=sections)


def write_pinned_copy(plan: dict, src: Path, dst: Path) -> None:
    """Copy `src` to `dst` with the +0x40 run bias applied to the pinned
    sections' in-place addends and those sections renamed for placement."""
    obj = ElfObj(src)
    out = bytearray(obj.data)
    text = obj.index_of(".text")
    tbase = obj.sh[text][4]
    shoff = _u32(out, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", out, 0x2E)
    his: dict[int, int] = {}
    for s in plan["sections"]:
        # text HI16/LO16: AHL' = AHL + 0x40, re-split with the LO16 sign carry
        for hoff, loff, addend in s["pairs"]:
            new = (addend + HDR) & 0xFFFFFFFF
            lo = new & 0xFFFF
            hi = ((new - _s16(lo)) >> 16) & 0xFFFF
            if his.setdefault(hoff, hi) != hi:
                raise PinError(f"{plan['stem']}: HI16 at +0x{hoff:x} is shared by "
                               f"LO16s that need different biased high halves")
            for off, field in ((hoff, hi), (loff, lo)):
                w = _u32(out, tbase + off)
                struct.pack_into("<I", out, tbase + off, (w & 0xFFFF0000) | field)
        # R_MIPS_32 entries: in-place addend + 0x40
        rbase = obj.sh[s["index"]][4]
        for off in s["entries"]:
            struct.pack_into("<I", out, rbase + off,
                             (_u32(out, rbase + off) + HDR) & 0xFFFFFFFF)
    # Rename the pinned sections: append a new .shstrtab at the end of the file.
    names = bytearray(obj.raw(shstrndx))
    for s in plan["sections"]:
        off = len(names)
        names += s["name"].encode() + b"\0"
        struct.pack_into("<I", out, shoff + s["index"] * shentsize, off)
        # placement is explicit; alignment 4 keeps ld from padding before it
        struct.pack_into("<I", out, shoff + s["index"] * shentsize + 32, 4)
    new_off = len(out)
    out += names
    struct.pack_into("<I", out, shoff + shstrndx * shentsize + 16, new_off)
    struct.pack_into("<I", out, shoff + shstrndx * shentsize + 20, len(names))
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_bytes(bytes(out))


def data_regions(data_objs: list[tuple[str, Path, int]]) -> list[tuple[int, int, str]]:
    """(link start, link end, stem) of each data-section object's .data."""
    out = []
    for stem, path, vram in data_objs:
        obj = ElfObj(path)
        i = obj.index_of(".data")
        if i is not None:
            out.append((vram, vram + obj.sh[i][5], stem))
    return out


def _as_data_section(blob: bytes) -> bytes:
    """rodata_pin._pack_object writes one section named .text; rename it to
    .data (same length) so the piece is not matched by `* (.text)`."""
    b = bytearray(blob)
    old = b"\0.text\0.rel.text\0"
    at = b.find(old)
    if at < 0:
        raise PinError("packed piece has an unexpected .shstrtab")
    b[at:at + len(old)] = b"\0.data\0.rel.data\0"
    shoff = _u32(b, 0x20)
    struct.pack_into("<I", b, shoff + 40 + 8, 0x3)   # section 1 flags: WA
    return bytes(b)


def split_data(region_obj: Path, stem: str, region_vram: int,
               carves: list[dict], out_dir: Path
               ) -> tuple[list[tuple[str, str]], dict[str, int]]:
    """Split a data-section object around the carved link spans.

    Returns (layout, absolute): layout is the ordered sequence of
    ("piece", object path) and ("pin", section name) entries for the .data
    output section; absolute holds data symbols that sat inside a carved span
    (defined absolutely at their unchanged link address).
    """
    obj = ElfObj(region_obj)
    data = obj.index_of(".data")
    if data is None:
        raise PinError(f"{region_obj}: no .data")
    blob = obj.raw(data)
    size = len(blob)
    spans = []
    cursor = 0
    for c in sorted(carves, key=lambda c: c["link"]):
        lo, hi = c["link"] - region_vram, c["link"] + c["size"] - region_vram
        if lo < cursor or hi > size or lo % 4 or hi % 4:
            raise PinError(f"{stem}: carve {c['name']} at 0x{c['link']:08x} overlaps, "
                           f"leaves the object or is not word aligned")
        spans.append((cursor, lo, "piece", None))
        spans.append((lo, hi, "pin", c["name"]))
        cursor = hi
    spans.append((cursor, size, "piece", None))

    relocs = obj.relocs(data)
    for off, _, si in relocs:
        for lo, hi, kind, _ in spans:
            if lo < off + 4 and off < hi and (kind == "pin" or not (lo <= off and off + 4 <= hi)):
                raise PinError(f"{stem}: data relocation at +0x{off:x} is inside or "
                               f"across a carved span")
        s = obj.syms[si]
        if s["bind"] == 0 and s["shndx"] not in (SHN_ABS,):
            raise PinError(f"{stem}: data relocation at +0x{off:x} uses a local symbol")

    defined = [s for s in obj.syms if s["shndx"] == data and s["type"] != 3 and s["bind"] != 0]
    locals_abs = [s for s in obj.syms if s["shndx"] == SHN_ABS and s["bind"] == 0 and s["name"]]
    out_dir.mkdir(parents=True, exist_ok=True)
    layout: list[tuple[str, str]] = []
    absolute: dict[str, int] = {}
    n = 0
    for lo, hi, kind, payload in spans:
        last = hi == size
        in_span = [s for s in defined if lo <= s["value"] < hi or (last and s["value"] == hi)]
        if kind == "pin" or hi == lo:
            for s in in_span:
                if "." not in s["name"]:   # splat's *.NON_MATCHING markers
                    absolute[s["name"]] = region_vram + s["value"]
            if kind == "pin":
                layout.append(("pin", payload))
            continue
        symbols = [dict(name="", value=0, size=0, bind=0, type=3, shndx=1)]
        symbols += [dict(s, shndx=SHN_ABS) for s in locals_abs]
        index: dict[str, int] = {}
        for s in in_span:
            symbols.append(dict(s, value=s["value"] - lo, shndx=1))
            index[s["name"]] = len(symbols)
        piece_relocs = []
        for off, kind_, si in relocs:
            if not lo <= off < hi:
                continue
            name = obj.syms[si]["name"]
            if name not in index:
                symbols.append(dict(name=name, value=0, size=0, bind=1, type=0,
                                    shndx=SHN_UNDEF))
                index[name] = len(symbols)
            piece_relocs.append((off - lo, kind_, index[name]))
        order = sorted(range(len(symbols)), key=lambda i: symbols[i]["bind"] != 0)
        remap = {old + 1: new + 1 for new, old in enumerate(order)}
        symbols = [symbols[i] for i in order]
        piece_relocs = [(o, k, remap[si]) for o, k, si in piece_relocs]
        path = out_dir / f"{stem}__pin{n:02d}.o"
        path.write_bytes(_as_data_section(
            _pack_object(obj, blob[lo:hi], 4, symbols, piece_relocs)))
        layout.append(("piece", str(path)))
        n += 1
    return layout, absolute
