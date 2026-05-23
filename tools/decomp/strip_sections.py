#!/usr/bin/env python3
"""Strip GNU-as-specific and empty sections from ELF object files for mwldmips.

mwldmips rejects several types of sections that GNU-as produces but that the
CodeWarrior linker does not understand:
  - Empty .text / .data / .bss sections (size == 0)
  - .pdr (MIPS Procedure Description Records — GNU extension)
  - .MIPS.abiflags (GNU ABI flags for MIPS)
  - .reginfo (MIPS register info)
  - .gnu.attributes (GNU ABI attributes)

Also handles two GNU-as vs mwldmips incompatibilities:
  - .text section alignment: GNU-as with -march=r5900 emits 16-byte alignment;
    mwldmips inserts padding to satisfy it, shifting function vram addresses.
    Fixed by forcing .text alignment to 4 via objcopy.
  - R_MIPS_GPREL16 relocations against absolute symbols: mwldmips rejects these
    because it can only apply GPREL16 to sdata-section symbols.  Fixed by
    pre-applying the relocations in Python (patching the instruction word) and
    removing the processed relocation entries.

This script strips all of them in-place using mipsel-linux-gnu-readelf and
mipsel-linux-gnu-objcopy (both available in the exterm-toolchain container).

Usage (inside the container):
    python3 tools/decomp/strip_sections.py <path/to/file.o> [...]

Run from repo root with repo bind-mounted at /work.
"""
from __future__ import annotations

import re
import struct
import subprocess
import sys
from pathlib import Path

# $gp is hard-coded by the original crt0: lui 0x28 / addiu -0x2C90 = 0x0027D370.
# We must use the same value when pre-applying R_MIPS_GPREL16 relocations.
GP_VALUE = 0x0027D370

# Sections mwldmips chokes on when empty (zero size).
EMPTY_KILL = {".text", ".data", ".bss"}

# Sections mwldmips doesn't understand at all — strip unconditionally when
# they are present (GNU-as extensions not used/created by CodeWarrior).
ALWAYS_KILL = {".pdr", ".MIPS.abiflags", ".reginfo", ".gnu.attributes"}


def unwanted_sections(obj: Path) -> list[str]:
    """Return names of sections to remove from obj."""
    try:
        out = subprocess.check_output(
            ["mipsel-linux-gnu-readelf", "-S", str(obj)],
            stderr=subprocess.DEVNULL,
        ).decode(errors="replace")
    except subprocess.CalledProcessError:
        return []

    result = []
    for line in out.splitlines():
        line = line.strip()
        # readelf -S output looks like:
        # [ 2] .data             PROGBITS  00000000 000044 000000 ...
        if not line.startswith("[") or "[Nr]" in line:
            continue
        # Remove leading [ N] bracket
        rest = line.split("]", 1)
        if len(rest) < 2:
            continue
        fields = rest[1].split()
        if len(fields) < 5:
            continue
        name = fields[0]
        # Size is field index 4 (hex)
        try:
            size = int(fields[4], 16)
        except ValueError:
            continue

        # Kill unconditionally (GNU extensions mwldmips doesn't understand)
        if name in ALWAYS_KILL:
            result.append(name)
            continue
        # Kill empty core sections
        if name in EMPTY_KILL and size == 0:
            result.append(name)
    return result


def strip(obj: Path, sections: list[str]) -> None:
    # Do NOT pass -I/-O format flags: the GNU-as output for -march=r5900 is
    # elf32-tradlittlemips (not elf32-littlemips), and explicit -I would fail.
    # Let objcopy auto-detect the format.
    cmd = ["mipsel-linux-gnu-objcopy"]
    for s in sections:
        cmd.append(f"--remove-section={s}")
    cmd.append(str(obj))
    subprocess.run(cmd, check=True)


def fix_text_alignment(obj: Path) -> bool:
    """Force .text section alignment to 4 bytes.

    GNU as with -march=r5900 emits .text with 16-byte alignment (MIPS EE ABI
    default).  mwldmips honours section alignment requirements and inserts
    padding between consecutive functions whenever the next function's .text
    is not at a 16-byte boundary, shifting all subsequent vram addresses.

    The original CodeWarrior-compiled binary has functions at 4-byte boundaries
    only, so we must force the GNU-as objects down to Al=4 before linking.
    """
    # Read current .text alignment from readelf output.
    try:
        out = subprocess.check_output(
            ["mipsel-linux-gnu-readelf", "-S", str(obj)],
            stderr=subprocess.DEVNULL,
        ).decode(errors="replace")
    except subprocess.CalledProcessError:
        return False

    needs_fix = False
    for line in out.splitlines():
        line = line.strip()
        if not line.startswith("[") or "[Nr]" in line:
            continue
        rest = line.split("]", 1)
        if len(rest) < 2:
            continue
        fields = rest[1].split()
        if not fields:
            continue
        name = fields[0]
        if name != ".text":
            continue
        # The last field of a readelf -S line is the alignment in bytes.
        try:
            align = int(fields[-1])
        except (ValueError, IndexError):
            continue
        if align > 4:
            needs_fix = True
        break

    if not needs_fix:
        return False

    cmd = ["mipsel-linux-gnu-objcopy",
           "--set-section-alignment", ".text=4",
           str(obj)]
    subprocess.run(cmd, check=True)
    return True


def apply_gprel16(obj: Path) -> bool:
    """Pre-apply R_MIPS_GPREL16 relocations against name-encoded absolute symbols.

    mwldmips applies GPREL16 only to symbols in .sdata/.sbss.  When the splat
    disassembly uses %gp_rel(D_XXXXXXXX) or %gp_rel(func_XXXXXXXX), GNU as
    emits R_MIPS_GPREL16 relocs against those names.  Since we define them as
    absolute values in the LCF (not as sdata members), mwldmips reports "GP
    offset overflow" and aborts the link.

    This function reads the ELF32 object in Python, patches the instruction
    words with the correct 16-bit GP-relative offset, and removes the processed
    relocation entries from .rel.text so mwldmips does not try to re-apply them.

    Only symbols whose names encode their address (D_XXXXXXXX / func_XXXXXXXX)
    are handled — if an unknown symbol has a GPREL16 reloc, a warning is printed
    and the entry is left intact.

    Returns True if any relocations were applied and the file was modified.
    """
    R_MIPS_GPREL16 = 7
    _name_re = re.compile(r'^(?:D_|func_)([0-9A-Fa-f]{4,8})$')

    data = bytearray(obj.read_bytes())

    # Parse ELF32 LE header fields we need.
    e_shoff     = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum     = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx  = struct.unpack_from("<H", data, 50)[0]

    def sh(idx: int) -> tuple:
        off = e_shoff + idx * e_shentsize
        return struct.unpack_from("<10I", data, off)

    # shstrtab
    shstr_sh   = sh(e_shstrndx)
    shstr_data = data[shstr_sh[4]: shstr_sh[4] + shstr_sh[5]]

    def sh_name(idx: int) -> str:
        name_off = sh(idx)[0]
        end = shstr_data.index(b"\x00", name_off)
        return shstr_data[name_off:end].decode("ascii", errors="replace")

    # Locate .symtab + .strtab + .text + .rel.text
    symtab_idx = text_idx = rel_text_idx = strtab_idx = -1
    for i in range(e_shnum):
        n = sh_name(i)
        t = sh(i)[1]
        if t == 2:            # SHT_SYMTAB
            symtab_idx = i
        elif t == 3 and n == ".strtab":
            strtab_idx = i
        elif t == 1 and n == ".text":   # SHT_PROGBITS
            text_idx = i
        elif t == 9 and n == ".rel.text":  # SHT_REL
            rel_text_idx = i

    if symtab_idx < 0 or rel_text_idx < 0 or text_idx < 0:
        return False  # No relocation table or no text section — nothing to do.

    # Parse symbol table.
    sym_sh      = sh(symtab_idx)
    sym_off     = sym_sh[4]
    sym_size    = sym_sh[5]
    sym_entsz   = sym_sh[9] or 16
    str_sh      = sh(strtab_idx)
    strtab_data = data[str_sh[4]: str_sh[4] + str_sh[5]]

    def sym_name(idx: int) -> str:
        name_off = struct.unpack_from("<I", data, sym_off + idx * sym_entsz)[0]
        end = strtab_data.index(b"\x00", name_off)
        return strtab_data[name_off:end].decode("ascii", errors="replace")

    # Parse and patch .rel.text entries.
    rel_sh    = sh(rel_text_idx)
    rel_off   = rel_sh[4]
    rel_size  = rel_sh[5]
    rel_entsz = rel_sh[9] or 8
    text_off  = sh(text_idx)[4]

    n_entries   = rel_size // rel_entsz
    keep        = []   # indices of relocation entries to KEEP
    applied     = 0

    for i in range(n_entries):
        entry_off              = rel_off + i * rel_entsz
        r_offset, r_info       = struct.unpack_from("<II", data, entry_off)
        r_sym_idx              = r_info >> 8
        r_type                 = r_info & 0xFF

        if r_type != R_MIPS_GPREL16:
            keep.append(i)
            continue

        name = sym_name(r_sym_idx)
        m = _name_re.match(name)
        if not m:
            print(f"[strip_sections] GPREL16 on unknown symbol '{name}' in {obj} — left intact",
                  file=sys.stderr)
            keep.append(i)
            continue

        sym_addr   = int(m.group(1), 16)
        gp_offset  = sym_addr - GP_VALUE

        if not (-32768 <= gp_offset <= 32767):
            print(f"[strip_sections] GPREL16 overflow: {name} offset={gp_offset} in {obj}",
                  file=sys.stderr)
            keep.append(i)
            continue

        # Patch the instruction: clear low 16 bits, insert gp_offset.
        instr_off = text_off + r_offset
        instr     = struct.unpack_from("<I", data, instr_off)[0]
        instr     = (instr & 0xFFFF0000) | (gp_offset & 0xFFFF)
        struct.pack_into("<I", data, instr_off, instr)
        applied  += 1

    if applied == 0:
        return False  # No GPREL16 entries found — file unchanged.

    # Compact .rel.text: keep only the non-GPREL16 entries.
    new_rel = bytearray()
    for i in keep:
        entry_off = rel_off + i * rel_entsz
        new_rel  += data[entry_off: entry_off + rel_entsz]

    # Overwrite the old .rel.text data with the compacted version.
    # The compacted block is smaller; zero-fill the remainder.
    old_rel_data_end = rel_off + rel_size
    data[rel_off: old_rel_data_end] = new_rel + bytes(rel_size - len(new_rel))

    # Update sh_size in the .rel.text section header.
    rel_sh_off = e_shoff + rel_text_idx * e_shentsize
    # sh_size is at offset 20 within the section header (field index 5).
    struct.pack_into("<I", data, rel_sh_off + 20, len(new_rel))

    obj.write_bytes(bytes(data))
    return True


def process(obj: Path) -> bool:
    """Strip unwanted sections, fix .text alignment, and pre-apply GPREL16.

    Returns True if the object was modified in any way.
    """
    bad = unwanted_sections(obj)
    changed = False
    if bad:
        strip(obj, bad)
        changed = True
    if fix_text_alignment(obj):
        changed = True
    if apply_gprel16(obj):
        changed = True
    return changed


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("usage: strip_sections.py <file.o> [...]")
    for arg in sys.argv[1:]:
        p = Path(arg)
        if not p.exists():
            print(f"[strip_sections] warning: {p} does not exist", file=sys.stderr)
            continue
        if process(p):
            print(f"[strip_sections] stripped sections from {p}")
