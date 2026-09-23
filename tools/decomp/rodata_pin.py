#!/usr/bin/env python3
"""Pin a compiled object's function-local .rodata at its ORIGINAL address.

WHY. A switch dispatcher (or a function with a static const table) compiled
from C carries its own .rodata: the jump table / constant array lives in the
object, and the function's `lui/addiu` pair is relocated against that local
section. The original binary keeps those bytes in the consolidated data region
(func_00261544 / func_00271DF8 in link order), at fixed addresses. Before this
module, link.py let mwldmips place compiled .rodata wherever `*(.rodata)` flows
(after the whole boot image), so the relocated `lui/addiu` encoded the wrong
address; fill_unmatched therefore linked every such function from its splat .s
(the "local-data guard").

HOW (exact placement, not substitution).
  1. plan_object() proves, for one compiled object, that each .rodata section
     has exactly one ORIGINAL address: every text HI16/LO16 pair against the
     section is compared with the original instruction pair at the same offset
     (the compiled text is byte-identical there), so base = original_address -
     in-place addend. All pairs must agree.
  2. It resolves the section's own relocations (jump-table R_MIPS_32 entries
     against the function's .text) with the function's original vram and
     requires the result to equal the original bytes at that base, the base to
     honour the section alignment, and -- for an object with several .rodata
     sections -- the linker's sequential aligned layout to reproduce every base
     with only zero padding in between. The pinned span must lie inside one
     data-region link unit that has no C source (so it is always assembled).
  3. link.py then splits that data-region object into pieces around the pinned
     spans and emits `<func>.o (.rodata)` between the pieces in the LCF, so the
     COMPILED bytes are what mwldmips writes at the original address, the
     compiled relocations resolve to the original values, and no other symbol
     moves (each piece keeps its original length). Region symbols that fall
     inside a carved span become absolute LCF definitions.

Any object that fails a check keeps the local-data guard (linked from .s).
The whole-image byte comparison in link.py remains the final arbiter.
"""
from __future__ import annotations

import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIG_ELF = ROOT / "config" / "SCUS_971.12"
PINS_JSON = ROOT / "build" / "rodata_pins.json"
PIECE_DIR = ROOT / "build" / "rodata_pin"

ORIG_LOAD_OFFSET = 0x300
ORIG_LOAD_VRAM = 0x00100000
ORIG_LOAD_FILESZ = 0x175B00

R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16 = 2, 5, 6
SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS, SHT_REL = 1, 2, 3, 8, 9
SHN_UNDEF, SHN_ABS = 0, 0xFFF1

# Unreferenced .rodata that a source deliberately keeps and whose bytes ARE an
# original table. The text reaches that table through an external symbol, so no
# HI16/LO16 pair names the section; the address is given here and every other
# check (bytes after relocation, alignment, containment) still applies.
#   sub_image_buffer_needs_to_be_aligned (ee-gcc): `keep[]` lists the four
#   computed-goto case labels so gcc keeps those blocks; resolved, it is the
#   first 16 bytes of the original dispatch table at jtbl_0026B650.
EXPLICIT_SYMBOL_PINS: dict[str, dict[str, int]] = {
    "sub_image_buffer_needs_to_be_aligned": {"keep.3": 0x0026B650},
}

# Allocated data sections other than .rodata that we never pin (their original
# homes are the gp window / .data, not the function-local rodata stream).
UNPINNABLE_DATA = (".data", ".sdata", ".sbss", ".lit4", ".lit8")


class ElfObj:
    """Minimal ELF32 little-endian relocatable-object reader."""

    def __init__(self, path: Path):
        self.path = path
        self.data = d = path.read_bytes()
        if d[:6] != b"\x7fELF\x01\x01":
            raise ValueError(f"not an ELF32 LE object: {path}")
        self.header = d[:52]
        shoff = struct.unpack_from("<I", d, 0x20)[0]
        shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x2E)
        self.sh = [struct.unpack_from("<10I", d, shoff + i * shentsize) for i in range(shnum)]
        names = self.raw(shstrndx)
        self.names = [self._cstr(names, h[0]) for h in self.sh]
        self.syms: list[dict] = []
        for h in self.sh:
            if h[1] == SHT_SYMTAB:
                strings = self.raw(h[6])
                for off in range(h[4], h[4] + h[5], h[9] or 16):
                    name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", d, off)
                    self.syms.append(dict(name=self._cstr(strings, name), value=value, size=size,
                                          bind=info >> 4, type=info & 15, other=other, shndx=shndx))
                break

    @staticmethod
    def _cstr(blob: bytes, off: int) -> str:
        return blob[off:blob.index(b"\0", off)].decode("ascii", "replace")

    def raw(self, index: int) -> bytes:
        h = self.sh[index]
        if h[1] == SHT_NOBITS:
            return bytes(h[5])
        return self.data[h[4]:h[4] + h[5]]

    def index_of(self, name: str) -> int | None:
        return self.names.index(name) if name in self.names else None

    def sections_named(self, name: str) -> list[int]:
        return [i for i, n in enumerate(self.names) if n == name]

    def relocs(self, target: int) -> list[tuple[int, int, int]]:
        """(offset, type, symbol index) of the SHT_REL section applying to `target`."""
        out = []
        for h in self.sh:
            if h[1] == SHT_REL and h[7] == target:
                for off in range(h[4], h[4] + h[5], h[9] or 8):
                    r_offset, info = struct.unpack_from("<II", self.data, off)
                    out.append((r_offset, info & 0xFF, info >> 8))
        return out


_ORIG_CACHE: list[bytes] = []


def original_bytes(addr: int, size: int) -> bytes:
    if not _ORIG_CACHE:
        _ORIG_CACHE.append(ORIG_ELF.read_bytes())
    if not (ORIG_LOAD_VRAM <= addr and addr + size <= ORIG_LOAD_VRAM + ORIG_LOAD_FILESZ):
        raise ValueError(f"0x{addr:08x}+0x{size:x} is outside the original load segment")
    off = addr - ORIG_LOAD_VRAM + ORIG_LOAD_OFFSET
    return _ORIG_CACHE[0][off:off + size]


def _s16(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value >= 0x8000 else value


def _align_up(value: int, align: int) -> int:
    return value if align <= 1 else (value + align - 1) // align * align


def plan_object(name: str, obj_path: Path, vram: int,
                regions: list[tuple[int, int, str]]) -> tuple[dict | None, str]:
    """Prove where `obj_path`'s local .rodata belongs. Returns (pin, reason).

    `regions` lists (start, end, unit) for the link units that may be split
    (assembled data-region objects with no C source). pin is None when the
    object cannot be proven; reason then says why (kept under the guard).
    """
    try:
        obj = ElfObj(obj_path)
    except (OSError, ValueError, struct.error) as exc:
        return None, f"unreadable object: {exc}"
    for sec in UNPINNABLE_DATA:
        for i in obj.sections_named(sec):
            if obj.sh[i][5]:
                return None, f"non-empty {sec} (only .rodata is pinned)"
    rodata = [i for i in obj.sections_named(".rodata") if obj.sh[i][5]]
    if not rodata:
        return None, "no .rodata"
    text = obj.index_of(".text")
    if text is None:
        return None, "no .text"
    text_bytes = obj.raw(text)

    # 1. Base address of each section from its HI16/LO16 pairs.
    bases: dict[int, set[int]] = {i: set() for i in rodata}
    pending: dict[int, tuple[int, int]] = {}
    for off, kind, si in obj.relocs(text):
        sym = obj.syms[si]
        sec = sym["shndx"]
        if sec not in bases:
            continue
        if kind not in (R_MIPS_HI16, R_MIPS_LO16):
            return None, f"text relocation type {kind} against .rodata at +0x{off:x}"
        compiled = struct.unpack_from("<I", text_bytes, off)[0]
        original = struct.unpack("<I", original_bytes(vram + off, 4))[0]
        if kind == R_MIPS_HI16:
            pending[sec] = (compiled, original)
            continue
        if sec not in pending:
            return None, f"LO16 without a preceding HI16 at +0x{off:x}"
        hi_c, hi_o = pending[sec]
        addend = ((hi_c & 0xFFFF) << 16) + _s16(compiled)
        target = ((hi_o & 0xFFFF) << 16) + _s16(original)
        bases[sec].add((target - addend - sym["value"]) & 0xFFFFFFFF)
    for sym_name, addr in EXPLICIT_SYMBOL_PINS.get(name, {}).items():
        for sym in obj.syms:
            if sym["name"] == sym_name and sym["shndx"] in bases:
                bases[sym["shndx"]].add((addr - sym["value"]) & 0xFFFFFFFF)
    sections = []
    for sec in rodata:
        if len(bases[sec]) != 1:
            return None, (f".rodata section {sec}: {len(bases[sec])} candidate addresses "
                          f"(needs exactly one)")
        base = next(iter(bases[sec]))
        size, align = obj.sh[sec][5], max(1, obj.sh[sec][8])
        if base % align:
            return None, f".rodata section {sec}: 0x{base:08x} violates alignment {align}"
        # 2. Resolve the section's own relocations and compare with the original.
        data = bytearray(obj.raw(sec))
        for off, kind, si in obj.relocs(sec):
            sym = obj.syms[si]
            if kind != R_MIPS_32 or sym["shndx"] != text:
                return None, (f".rodata section {sec}: relocation type {kind} against "
                              f"'{sym['name'] or obj.names[sym['shndx']]}' is not a local text address")
            addend = struct.unpack_from("<I", data, off)[0]
            struct.pack_into("<I", data, off, (vram + sym["value"] + addend) & 0xFFFFFFFF)
        try:
            if bytes(data) != original_bytes(base, size):
                return None, f".rodata section {sec}: bytes differ from the original at 0x{base:08x}"
        except ValueError as exc:
            return None, str(exc)
        sections.append(dict(index=sec, addr=base, size=size, align=align))

    # 3. The linker lays an object's .rodata sections out in index order, each
    #    aligned; that layout must reproduce every base with zero-only padding.
    cursor = sections[0]["addr"]
    for sec in sections:
        placed = _align_up(cursor, sec["align"])
        if placed != sec["addr"]:
            return None, (f".rodata section {sec['index']}: sequential layout puts it at "
                          f"0x{placed:08x}, original is 0x{sec['addr']:08x}")
        if any(original_bytes(cursor, placed - cursor)):
            return None, f"non-zero original bytes in the alignment gap before 0x{placed:08x}"
        cursor = placed + sec["size"]
    start, end = sections[0]["addr"], cursor
    unit = next((u for lo, hi, u in regions if lo <= start and end <= hi), None)
    if unit is None:
        return None, f"span 0x{start:08x}-0x{end:08x} is not inside one splittable data-region unit"
    return dict(name=name, vram=vram, region=unit, start=start, end=end,
                sections=sections), "pinned"


# ---------------------------------------------------------------------------
# Splitting a data-region object (used by link.py)
# ---------------------------------------------------------------------------

def _pack_object(template: ElfObj, text: bytes, align: int,
                 symbols: list[dict], relocs: list[tuple[int, int, int]]) -> bytes:
    """Serialize a relocatable object: .text, .rel.text, .symtab, .strtab, .shstrtab.

    `symbols` excludes the null entry; locals must precede globals.
    `relocs` hold (offset, type, index into `symbols` + 1).
    """
    shstr = b"\0.text\0.rel.text\0.symtab\0.strtab\0.shstrtab\0"
    n_text, n_rel, n_sym, n_str, n_shstr = 1, 7, 17, 25, 33
    strtab = bytearray(b"\0")
    symtab = bytearray(16)
    first_global = 1
    for i, s in enumerate(symbols, 1):
        name_off = 0
        if s["name"]:
            name_off = len(strtab)
            strtab += s["name"].encode() + b"\0"
        symtab += struct.pack("<IIIBBH", name_off, s["value"], s["size"],
                              (s["bind"] << 4) | s["type"], s.get("other", 0), s["shndx"])
        if s["bind"] == 0:
            first_global = i + 1
    rel = b"".join(struct.pack("<II", off, (si << 8) | kind) for off, kind, si in relocs)

    blobs = [text, rel, bytes(symtab), bytes(strtab), shstr]
    offset = 52
    offsets = []
    body = bytearray()
    for blob in blobs:
        pad = _align_up(offset, 4) - offset
        body += bytes(pad)
        offset += pad
        offsets.append(offset)
        body += blob
        offset += len(blob)
    shoff = _align_up(offset, 4)
    body += bytes(shoff - offset)
    headers = [
        (0,) * 10,
        (n_text, SHT_PROGBITS, 0x6, 0, offsets[0], len(text), 0, 0, align, 0),
        (n_rel, SHT_REL, 0x40, 0, offsets[1], len(rel), 3, 1, 4, 8),
        (n_sym, SHT_SYMTAB, 0, 0, offsets[2], len(symtab), 4, first_global, 4, 16),
        (n_str, SHT_STRTAB, 0, 0, offsets[3], len(strtab), 0, 0, 1, 0),
        (n_shstr, SHT_STRTAB, 0, 0, offsets[4], len(shstr), 0, 0, 1, 0),
    ]
    for h in headers:
        body += struct.pack("<10I", *h)
    header = bytearray(template.header)
    struct.pack_into("<I", header, 0x1C, 0)          # e_phoff
    struct.pack_into("<I", header, 0x20, shoff)      # e_shoff
    struct.pack_into("<HHHHHH", header, 0x28, 52, 0, 0, 40, len(headers), 5)
    return bytes(header) + bytes(body)


def split_region(region_obj: Path, region_name: str, region_vram: int,
                 carves: list[dict], out_dir: Path) -> tuple[list[tuple[str, str]], dict[str, int]]:
    """Split an assembled data-region object around the pinned spans.

    Returns (layout, absolute_symbols). layout is the ordered LCF sequence:
    ("piece", path) for a region piece's .text, ("rodata", func) for a pinned
    object's .rodata. absolute_symbols are region globals that sat inside a
    carved span; they are defined absolutely in the LCF so references resolve
    to the same address.
    """
    obj = ElfObj(region_obj)
    text = obj.index_of(".text")
    if text is None:
        raise ValueError(f"{region_obj}: no .text")
    blob = obj.raw(text)
    size = len(blob)
    carves = sorted(carves, key=lambda c: c["start"])
    spans = []  # region-relative (lo, hi, kind, payload)
    cursor = 0
    for c in carves:
        lo, hi = c["start"] - region_vram, c["end"] - region_vram
        if lo < cursor or hi > size:
            raise ValueError(f"{region_name}: carve {c['name']} 0x{c['start']:08x} overlaps "
                             f"another carve or leaves the region")
        if lo % 4 or hi % 4:
            raise ValueError(f"{region_name}: carve {c['name']} is not word aligned")
        spans.append((cursor, lo, "piece", None))
        spans.append((lo, hi, "rodata", c["name"]))
        cursor = hi
    spans.append((cursor, size, "piece", None))

    relocs = obj.relocs(text)
    for off, kind, si in relocs:
        for lo, hi, kind_, _ in spans:
            if lo < off + 4 and off < hi and not (lo <= off and off + 4 <= hi):
                raise ValueError(f"{region_name}: relocation at +0x{off:x} straddles a split")
        if obj.syms[si]["shndx"] == text and obj.syms[si]["bind"] == 0:
            raise ValueError(f"{region_name}: section-relative relocation at +0x{off:x}")

    out_dir.mkdir(parents=True, exist_ok=True)
    layout: list[tuple[str, str]] = []
    absolute: dict[str, int] = {}
    defined = [s for s in obj.syms if s["shndx"] == text and s["type"] != 3 and s["bind"] != 0]
    locals_abs = [s for s in obj.syms if s["shndx"] == SHN_ABS and s["bind"] == 0 and s["name"]]
    piece_no = 0
    for lo, hi, kind, payload in spans:
        is_last = hi == size
        in_span = [s for s in defined if lo <= s["value"] < hi or (is_last and s["value"] == hi)]
        if kind == "rodata":
            for s in in_span:
                if "." not in s["name"]:
                    absolute[s["name"]] = region_vram + s["value"]
            layout.append(("rodata", payload))
            continue
        if hi == lo:
            # An empty piece can only hold symbols at a carve boundary, which the
            # neighbouring carve/piece owns; nothing to emit.
            for s in in_span:
                if "." not in s["name"]:
                    absolute[s["name"]] = region_vram + s["value"]
            continue
        symbols = [dict(name="", value=0, size=0, bind=0, type=3, shndx=1)]
        symbols += [dict(s, shndx=SHN_ABS) for s in locals_abs]
        index: dict[str, int] = {}
        for s in in_span:
            symbols.append(dict(s, value=s["value"] - lo, shndx=1))
            index[s["name"]] = len(symbols)
        piece_relocs = []
        for off, kind_, si in relocs:
            if not (lo <= off < hi):
                continue
            sname = obj.syms[si]["name"]
            if sname not in index:
                symbols.append(dict(name=sname, value=0, size=0, bind=1, type=0, shndx=SHN_UNDEF))
                index[sname] = len(symbols)
            piece_relocs.append((off - lo, kind_, index[sname]))
        # locals first (the section symbol and the ABS locals), then globals/weaks.
        order = sorted(range(len(symbols)), key=lambda i: symbols[i]["bind"] != 0)
        remap = {old + 1: new + 1 for new, old in enumerate(order)}
        symbols = [symbols[i] for i in order]
        piece_relocs = [(o, k, remap[si]) for o, k, si in piece_relocs]
        path = out_dir / f"{region_name}__lrod{piece_no:02d}.o"
        path.write_bytes(_pack_object(obj, blob[lo:hi], 4, symbols, piece_relocs))
        layout.append(("piece", str(path)))
        piece_no += 1
    return layout, absolute
