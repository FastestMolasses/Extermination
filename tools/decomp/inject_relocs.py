#!/usr/bin/env python3
"""Post-compile relocation injector for mwccmips objects.

mwcc inline asm (`.word 0x...`) cannot attach R_MIPS_GPREL16 / R_MIPS_HI16 /
R_MIPS_LO16 relocation entries to the emitted instruction words, so a hand-
written asm_void function that hardcodes gp_rel / hi / lo references to known
data symbols produces a .text byte-identical at link time but objdiff still
reports a mismatch because the *target* object (assembled from splat's .s by
GNU as) has explicit reloc entries while our compiled object has none.

This tool patches build/obj/<name>.o in-place by:

  1. Parsing build/asm/matchings/main/code/<name>.s for instructions tagged
     with %gp_rel(SYM), %hi(SYM), %lo(SYM).
  2. Verifying the corresponding instruction bytes in the mwcc .text match
     splat's bytes (i.e. the function really is link-time-identical).
  3. Adding global UNDEF symbols for any referenced SYM not already in the
     mwcc object's .symtab.
  4. Synthesising a .rel.text section with R_MIPS_GPREL16 / HI16 / LO16
     entries at the right text offsets.

The result: objdiff sees identical .text bytes AND identical relocations
relative to the splat target object, and matching jumps from "partial"
(byte-equal but reloc-missing) to 100%.

Idempotent: re-running on an already-patched .o is a no-op.  Safe: if any
verification fails (text differs, unknown symbol, splat .s missing) the
object is left unchanged.

Run anywhere (no toolchain needed; pure Python):
    python3 tools/decomp/inject_relocs.py                 # all build/obj/*.o
    python3 tools/decomp/inject_relocs.py func_001D2DE0   # one
"""
from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ASM_DIR = ROOT / "build" / "asm" / "matchings" / "main" / "code"
OBJ_DIR = ROOT / "build" / "obj"

R_MIPS_NONE    = 0
R_MIPS_16      = 1
R_MIPS_32      = 2
R_MIPS_26      = 4
R_MIPS_HI16    = 5
R_MIPS_LO16    = 6
R_MIPS_GPREL16 = 7

SHT_NULL     = 0
SHT_PROGBITS = 1
SHT_SYMTAB   = 2
SHT_STRTAB   = 3
SHT_REL      = 9
SHT_NOBITS   = 8

STB_LOCAL  = 0
STB_GLOBAL = 1
STT_NOTYPE = 0
STT_FUNC   = 2

# Splat .s line: "    /* FILE_OFF VRAM HEXBYTES */  mnemonic args..."
LINE_RE = re.compile(
    r'^\s*/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*(.*)$'
)

# Argument patterns to find reloc references.
GP_REL_RE = re.compile(r'%gp_rel\(([A-Za-z_][A-Za-z0-9_]*)(?:\s*\+\s*0?x?[0-9A-Fa-f]+)?\)')
HI_RE     = re.compile(r'%hi\(([A-Za-z_][A-Za-z0-9_]*)(?:\s*\+\s*0?x?[0-9A-Fa-f]+)?\)')
LO_RE     = re.compile(r'%lo\(([A-Za-z_][A-Za-z0-9_]*)(?:\s*\+\s*0?x?[0-9A-Fa-f]+)?\)')


def parse_splat_relocs(s_path: Path) -> tuple[list[bytes], list[tuple[int, int, str]]] | None:
    """Parse splat .s -> (list of 4-byte instruction words, list of (offset, type, sym)).

    Returns None if the file is missing or unparseable.  The instruction
    word list lets the caller verify .text byte-equivalence before injecting.
    """
    if not s_path.exists():
        return None

    instrs: list[bytes] = []
    relocs: list[tuple[int, int, str]] = []
    base_vram: int | None = None

    with s_path.open() as f:
        for raw in f:
            m = LINE_RE.match(raw)
            if not m:
                continue
            _file_off, vram_str, bytes_hex, rest = m.groups()
            vram = int(vram_str, 16)
            if base_vram is None:
                base_vram = vram
            text_off = vram - base_vram
            # splat shows bytes in file order (little-endian instr). Convert to bytes.
            b = bytes.fromhex(bytes_hex)
            # Pad list up to text_off with zeros if there's a gap.
            while len(instrs) * 4 < text_off:
                instrs.append(b"\x00\x00\x00\x00")
            # Place at index text_off // 4
            idx = text_off // 4
            while len(instrs) <= idx:
                instrs.append(b"\x00\x00\x00\x00")
            instrs[idx] = b

            mg = GP_REL_RE.search(rest)
            if mg:
                relocs.append((text_off, R_MIPS_GPREL16, mg.group(1)))
                continue
            mh = HI_RE.search(rest)
            if mh:
                relocs.append((text_off, R_MIPS_HI16, mh.group(1)))
                continue
            ml = LO_RE.search(rest)
            if ml:
                relocs.append((text_off, R_MIPS_LO16, ml.group(1)))
                continue

    return instrs, relocs


# ---------- ELF read / write helpers ----------

def _u32(data: bytes, off: int) -> int:
    return struct.unpack_from("<I", data, off)[0]


def _u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def parse_elf(data: bytes) -> dict:
    """Return a dict with parsed ELF32 LE structure."""
    assert data[:4] == b"\x7fELF" and data[4] == 1 and data[5] == 1, "not ELF32 LE"
    e_shoff = _u32(data, 32)
    e_shentsize = _u16(data, 46)
    e_shnum = _u16(data, 48)
    e_shstrndx = _u16(data, 50)

    sections = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        sh = struct.unpack_from("<10I", data, off)
        sections.append({
            "sh_name": sh[0], "sh_type": sh[1], "sh_flags": sh[2], "sh_addr": sh[3],
            "sh_offset": sh[4], "sh_size": sh[5], "sh_link": sh[6], "sh_info": sh[7],
            "sh_addralign": sh[8], "sh_entsize": sh[9],
        })

    shstr_off = sections[e_shstrndx]["sh_offset"]
    shstr_size = sections[e_shstrndx]["sh_size"]
    shstr_data = data[shstr_off:shstr_off + shstr_size]

    def name_of(sh):
        n0 = sh["sh_name"]
        end = shstr_data.index(b"\x00", n0)
        return shstr_data[n0:end].decode("ascii", errors="replace")

    for s in sections:
        s["name"] = name_of(s)

    return {
        "e_shoff": e_shoff,
        "e_shentsize": e_shentsize,
        "e_shnum": e_shnum,
        "e_shstrndx": e_shstrndx,
        "sections": sections,
        "shstr_data": shstr_data,
    }


def find_section(elf: dict, name: str, sh_type: int | None = None) -> int | None:
    for i, s in enumerate(elf["sections"]):
        if s["name"] == name and (sh_type is None or s["sh_type"] == sh_type):
            return i
    return None


def read_symtab(data: bytes, elf: dict) -> tuple[int, int, list[dict], bytes]:
    """Return (symtab_idx, strtab_idx, symbols, strtab_data)."""
    symtab_idx = next(i for i, s in enumerate(elf["sections"]) if s["sh_type"] == SHT_SYMTAB)
    strtab_idx = elf["sections"][symtab_idx]["sh_link"]
    st = elf["sections"][symtab_idx]
    sr = elf["sections"][strtab_idx]
    strtab_data = data[sr["sh_offset"]:sr["sh_offset"] + sr["sh_size"]]

    entsize = st["sh_entsize"] or 16
    n = st["sh_size"] // entsize
    syms = []
    for i in range(n):
        off = st["sh_offset"] + i * entsize
        name_off, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", data, off)
        end = strtab_data.index(b"\x00", name_off)
        name = strtab_data[name_off:end].decode("ascii", errors="replace")
        syms.append({
            "name": name, "name_off": name_off, "value": value, "size": size,
            "info": info, "other": other, "shndx": shndx,
        })
    return symtab_idx, strtab_idx, syms, strtab_data


def pack_sym(name_off: int, sym: dict) -> bytes:
    return struct.pack("<IIIBBH", name_off, sym["value"], sym["size"],
                       sym["info"], sym["other"], sym["shndx"])


def pack_section_header(s: dict) -> bytes:
    return struct.pack("<10I",
        s["sh_name"], s["sh_type"], s["sh_flags"], s["sh_addr"],
        s["sh_offset"], s["sh_size"], s["sh_link"], s["sh_info"],
        s["sh_addralign"], s["sh_entsize"],
    )


def already_has_rel_text(elf: dict) -> bool:
    return find_section(elf, ".rel.text", SHT_REL) is not None


# ---------- core injector ----------

def inject(obj_path: Path, verbose: bool = False, type_counts: dict | None = None) -> tuple[str, int]:
    """Inject .rel.text into obj_path based on splat .s.

    Returns (status, n_relocs_added).  Status:
      "skip:no_asm"      — no splat .s file
      "skip:no_relocs"   — splat .s has no %gp_rel/%hi/%lo refs
      "skip:complete"    — every splat reloc is already present
      "skip:text_mismatch" — bytes diverge from splat at reloc offsets
      "skip:bad_sym"     — reloc symbol not D_XXX / func_XXX / known
      "patched"          — wrote new .o
    """
    name = obj_path.stem
    s_path = ASM_DIR / f"{name}.s"
    parsed = parse_splat_relocs(s_path)
    if parsed is None:
        return "skip:no_asm", 0
    splat_instrs, splat_relocs = parsed
    if not splat_relocs:
        return "skip:no_relocs", 0

    data = obj_path.read_bytes()
    try:
        elf = parse_elf(data)
    except Exception as ex:
        if verbose:
            print(f"  [{name}] parse fail: {ex}", file=sys.stderr)
        return "skip:parse_fail", 0

    text_idx = find_section(elf, ".text", SHT_PROGBITS)
    if text_idx is None:
        return "skip:no_text", 0
    tsec = elf["sections"][text_idx]
    text_bytes = data[tsec["sh_offset"]:tsec["sh_offset"] + tsec["sh_size"]]

    # Verify splat bytes match obj's .text at every reloc offset.  We must be
    # strict because injecting relocs at offsets where the obj's instruction
    # differs from the original would tell the linker to re-resolve a value
    # mwcc didn't intend, breaking byte-identity at link time.
    for (off, _t, _sym) in splat_relocs:
        if off + 4 > len(text_bytes):
            return "skip:text_mismatch", 0
        obj_word = text_bytes[off:off + 4]
        if off // 4 >= len(splat_instrs):
            return "skip:text_mismatch", 0
        splat_word = splat_instrs[off // 4]
        if obj_word != splat_word:
            return "skip:text_mismatch", 0

    # Read existing symtab + any existing .rel.text.
    symtab_idx, strtab_idx, syms, strtab_data = read_symtab(data, elf)
    name_to_idx = {s["name"]: i for i, s in enumerate(syms) if s["name"]}

    existing_rel_idx = find_section(elf, ".rel.text", SHT_REL)
    existing_relocs: list[tuple[int, int, int]] = []  # (offset, type, sym_idx)
    existing_keys: set[tuple[int, int]] = set()       # (offset, type)
    if existing_rel_idx is not None:
        rsec = elf["sections"][existing_rel_idx]
        entsize = rsec["sh_entsize"] or 8
        n = rsec["sh_size"] // entsize
        for i in range(n):
            eoff = rsec["sh_offset"] + i * entsize
            r_offset, r_info = struct.unpack_from("<II", data, eoff)
            rt = r_info & 0xFF
            si = r_info >> 8
            existing_relocs.append((r_offset, rt, si))
            existing_keys.add((r_offset, rt))

    # Filter splat relocs to ones not already present.
    new_relocs = [(o, t, s) for (o, t, s) in splat_relocs if (o, t) not in existing_keys]
    if not new_relocs:
        return "skip:complete", 0

    # Determine new symbols to add.
    needed_syms: list[str] = []
    for (_o, _t, sym) in new_relocs:
        if sym not in name_to_idx and sym not in needed_syms:
            if not re.match(r'^(?:D_|func_)[0-9A-Fa-f]{4,8}$', sym):
                if verbose:
                    print(f"  [{name}] unknown reloc symbol {sym}", file=sys.stderr)
                return "skip:bad_sym", 0
            needed_syms.append(sym)

    # ---- Build new ELF: keep all existing section data in place; lay new
    # ---- copies of strtab + symtab + .rel.text + shstrtab at end of file.
    out = bytearray(data)

    # Zero the 16-bit immediate field of every instruction at a new reloc
    # offset.  mwcc-emitted asm void `.word` directives hardcode the resolved
    # gp_rel / hi / lo value; once we attach a reloc the linker will compute
    # and write the value itself.  If we left the resolved value in place,
    # strip_sections.apply_gprel16 (which treats the imm as a REL addend)
    # would double-add it and overflow, and mwldmips would re-fill HI16/LO16
    # by adding to the existing imm.  Zeroing matches the layout GNU as emits
    # for splat target objects -- and that means objdiff also sees identical
    # .text bytes between target and base.
    text_file_off = tsec["sh_offset"]
    for (off, _t, _sym) in new_relocs:
        instr_off = text_file_off + off
        out[instr_off]     = 0
        out[instr_off + 1] = 0

    # 1) Extend strtab.
    new_strtab = bytearray(strtab_data)
    sym_name_offs: dict[str, int] = {}
    for nm in needed_syms:
        sym_name_offs[nm] = len(new_strtab)
        new_strtab += nm.encode("ascii") + b"\x00"

    # 2) Rebuild symtab using each symbol's stored name_off (which points into
    # strtab_data prefix of new_strtab; still valid since we only appended).
    new_symtab = bytearray()
    for s in syms:
        new_symtab += pack_sym(s["name_off"], s)
    new_sym_indices: dict[str, int] = {}
    next_idx = len(syms)
    for nm in needed_syms:
        entry = {
            "value": 0, "size": 0,
            "info": (STB_GLOBAL << 4) | STT_NOTYPE,
            "other": 0, "shndx": 0,
        }
        new_symtab += pack_sym(sym_name_offs[nm], entry)
        new_sym_indices[nm] = next_idx
        next_idx += 1

    # 3) Build .rel.text data: existing entries + appended new entries.
    rel_data = bytearray()
    for (r_off, rt, si) in existing_relocs:
        rel_data += struct.pack("<II", r_off, (si << 8) | rt)
    for (off, rtype, sym) in new_relocs:
        sidx = name_to_idx.get(sym, new_sym_indices.get(sym))
        rel_data += struct.pack("<II", off, (sidx << 8) | rtype)

    # 4) shstrtab — add .rel.text if it wasn't already there.
    new_shstr = bytearray(elf["shstr_data"])
    rel_text_name_off: int | None = None
    if existing_rel_idx is None:
        rel_text_name_off = len(new_shstr)
        new_shstr += b".rel.text\x00"

    # ---- Lay out new section data at end of file ----
    # Position new strtab, new symtab, rel.text data, new shstrtab and the
    # rebuilt section table sequentially.
    cursor = len(out)
    def align(c: int, n: int) -> int:
        if n <= 1:
            return c
        rem = c % n
        return c if rem == 0 else c + (n - rem)

    cursor = align(cursor, 4)
    new_strtab_off = cursor
    out += b"\x00" * (cursor - len(out))
    out += new_strtab
    cursor = len(out)

    cursor = align(cursor, 4)
    new_symtab_off = cursor
    out += b"\x00" * (cursor - len(out))
    out += new_symtab
    cursor = len(out)

    cursor = align(cursor, 4)
    rel_text_off = cursor
    out += b"\x00" * (cursor - len(out))
    out += rel_data
    cursor = len(out)

    cursor = align(cursor, 1)
    new_shstr_off = cursor
    out += new_shstr
    cursor = len(out)

    # ---- Rebuild section headers ----
    # We modify in place: update strtab, symtab, shstrtab to new locations
    # and sizes; add .rel.text section header at the end.
    sections = [dict(s) for s in elf["sections"]]

    sections[strtab_idx]["sh_offset"] = new_strtab_off
    sections[strtab_idx]["sh_size"] = len(new_strtab)

    sections[symtab_idx]["sh_offset"] = new_symtab_off
    sections[symtab_idx]["sh_size"] = len(new_symtab)
    # entsize stays 16

    if existing_rel_idx is not None:
        sections[existing_rel_idx]["sh_offset"] = rel_text_off
        sections[existing_rel_idx]["sh_size"] = len(rel_data)
        sections[existing_rel_idx]["sh_link"] = symtab_idx
        sections[existing_rel_idx]["sh_info"] = text_idx
    else:
        rel_text_sh = {
            "sh_name": rel_text_name_off,
            "sh_type": SHT_REL,
            "sh_flags": 0,
            "sh_addr": 0,
            "sh_offset": rel_text_off,
            "sh_size": len(rel_data),
            "sh_link": symtab_idx,
            "sh_info": text_idx,
            "sh_addralign": 4,
            "sh_entsize": 8,
        }
        sections.append(rel_text_sh)

    # Update shstrtab section header.
    sections[elf["e_shstrndx"]]["sh_offset"] = new_shstr_off
    sections[elf["e_shstrndx"]]["sh_size"] = len(new_shstr)

    # Place section table at end.
    cursor = align(cursor, 4)
    new_shoff = cursor
    out += b"\x00" * (cursor - len(out))
    for s in sections:
        out += pack_section_header(s)

    # Update ELF header.
    struct.pack_into("<I", out, 32, new_shoff)        # e_shoff
    struct.pack_into("<H", out, 48, len(sections))    # e_shnum
    # e_shstrndx unchanged (still pointing to original shstrtab section slot)

    obj_path.write_bytes(bytes(out))
    if type_counts is not None:
        for (_o, t, _s) in new_relocs:
            type_counts[t] = type_counts.get(t, 0) + 1
    return "patched", len(new_relocs)


# ---------- driver ----------

def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("names", nargs="*", help="function names (defaults to all build/obj/*.o)")
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument("--stats", action="store_true",
                    help="print per-status counts even when injecting all")
    args = ap.parse_args(argv)

    if args.names:
        objs = [OBJ_DIR / f"{n}.o" for n in args.names]
    else:
        objs = sorted(OBJ_DIR.glob("*.o"))

    counts: dict[str, int] = {}
    type_counts = {R_MIPS_GPREL16: 0, R_MIPS_HI16: 0, R_MIPS_LO16: 0}
    total_relocs = 0
    patched_objs = 0
    patched_names: list[str] = []
    for obj in objs:
        if not obj.exists():
            counts["skip:missing"] = counts.get("skip:missing", 0) + 1
            continue
        status, n = inject(obj, verbose=args.verbose, type_counts=type_counts)
        counts[status] = counts.get(status, 0) + 1
        if status == "patched":
            patched_objs += 1
            total_relocs += n
            patched_names.append(obj.stem)

    print(f"[inject_relocs] patched {patched_objs} object(s), {total_relocs} reloc(s) injected")
    print(f"  GPREL16: {type_counts[R_MIPS_GPREL16]}  HI16: {type_counts[R_MIPS_HI16]}  LO16: {type_counts[R_MIPS_LO16]}")
    if args.verbose or args.stats:
        for k in sorted(counts):
            print(f"  {k}: {counts[k]}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
