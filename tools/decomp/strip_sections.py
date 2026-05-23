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

        # Patch the instruction.
        # For REL relocations (not RELA), the addend is embedded in the
        # instruction's immediate field.  Read it as a signed 16-bit value
        # and add it to the symbol's GP offset so that expressions like
        # %gp_rel(D_00275B00 + 0xC) are resolved correctly.
        instr_off  = text_off + r_offset
        instr      = struct.unpack_from("<I", data, instr_off)[0]
        addend     = instr & 0xFFFF
        if addend >= 0x8000:
            addend -= 0x10000   # sign-extend
        final_offset = gp_offset + addend

        if not (-32768 <= final_offset <= 32767):
            print(f"[strip_sections] GPREL16+addend overflow: {name}+{addend} "
                  f"offset={final_offset} in {obj}", file=sys.stderr)
            keep.append(i)
            continue

        instr     = (instr & 0xFFFF0000) | (final_offset & 0xFFFF)
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


def resize_text(obj: Path, target_size: int) -> bool:
    """Resize .text section content to exactly target_size bytes.

    This handles two directions:

    Truncation (text_size > target_size):
        GNU-as with -march=r5900 emits .text padded to the next 16-byte
        boundary.  When target_size < padded size, the extra zero bytes are
        removed so that mwldmips places the next function at the correct vram.

    Extension (text_size < target_size):
        When the original binary has trailing zero-nop padding between
        functions that is larger than what GNU-as emits (i.e., the inter-
        function gap exceeds the 16-byte rounding), we extend .text by
        appending zero bytes in the ELF file and updating sh_size.
        In practice this occurs for at most one function.

    The function also trims the .rel.text table to remove any relocation
    entries that point beyond target_size (only relevant when truncating).

    Returns True if the file was modified.
    """
    if target_size <= 0:
        return False

    data = bytearray(obj.read_bytes())

    e_shoff     = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum     = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx  = struct.unpack_from("<H", data, 50)[0]

    def sh(idx: int) -> tuple:
        off = e_shoff + idx * e_shentsize
        return struct.unpack_from("<10I", data, off)

    shstr_sh    = sh(e_shstrndx)
    shstr_data  = data[shstr_sh[4]: shstr_sh[4] + shstr_sh[5]]

    def sh_name(idx: int) -> str:
        name_off = sh(idx)[0]
        end = shstr_data.index(b"\x00", name_off)
        return shstr_data[name_off:end].decode("ascii", errors="replace")

    text_idx = rel_text_idx = -1
    for i in range(e_shnum):
        n = sh_name(i)
        if n == ".text":
            text_idx = i
        elif n == ".rel.text":
            rel_text_idx = i

    if text_idx < 0:
        return False

    text_sh = sh(text_idx)
    text_file_off = text_sh[4]   # sh_offset
    text_size     = text_sh[5]   # sh_size

    if text_size == target_size:
        return False  # Already correct — nothing to do.

    if text_size > target_size:
        # --- Truncation path ---
        # Zero the extra bytes (trailing nop padding) in the file.
        for i in range(text_file_off + target_size, text_file_off + text_size):
            data[i] = 0

        # Update sh_size in the .text section header.
        text_sh_off = e_shoff + text_idx * e_shentsize
        struct.pack_into("<I", data, text_sh_off + 20, target_size)

        # Truncate .rel.text to remove entries that point beyond target_size.
        if rel_text_idx >= 0:
            rel_sh      = sh(rel_text_idx)
            rel_off     = rel_sh[4]   # sh_offset
            rel_size    = rel_sh[5]   # sh_size
            rel_entsz   = rel_sh[9] or 8

            n_entries = rel_size // rel_entsz
            keep = []
            for i in range(n_entries):
                entry_off = rel_off + i * rel_entsz
                r_offset = struct.unpack_from("<I", data, entry_off)[0]
                if r_offset < target_size:
                    keep.append(i)

            if len(keep) < n_entries:
                new_rel = bytearray()
                for i in keep:
                    entry_off = rel_off + i * rel_entsz
                    new_rel += data[entry_off: entry_off + rel_entsz]
                old_end = rel_off + rel_size
                data[rel_off: old_end] = new_rel + bytes(rel_size - len(new_rel))
                rel_sh_off = e_shoff + rel_text_idx * e_shentsize
                struct.pack_into("<I", data, rel_sh_off + 20, len(new_rel))

        obj.write_bytes(bytes(data))
        return True
    else:
        # --- Extension path: target_size > text_size ---
        # Insert zero bytes immediately after .text's current data in the file,
        # then fix up all ELF header fields that reference positions after the
        # insertion point (section offsets and the section-header-table offset).
        extra = target_size - text_size
        insert_at = text_file_off + text_size

        # Collect all section offsets BEFORE modifying the data array.
        sh_offsets = []
        for i in range(e_shnum):
            sh_fields = struct.unpack_from("<10I", data, e_shoff + i * e_shentsize)
            sh_offsets.append(sh_fields[4])  # sh_offset field

        # Insert the extra zero bytes.
        data = data[:insert_at] + bytes(extra) + data[insert_at:]

        # e_shoff may have moved if the SHT came after insert_at.
        new_e_shoff = e_shoff + extra if e_shoff >= insert_at else e_shoff
        struct.pack_into("<I", data, 32, new_e_shoff)

        # Update sh_size for .text (using the NEW e_shoff).
        text_sh_base = new_e_shoff + text_idx * e_shentsize
        struct.pack_into("<I", data, text_sh_base + 20, target_size)

        # Fix up sh_offset for all sections whose data starts at or after insert_at,
        # skipping .text itself (its offset hasn't changed, only its size has).
        for i in range(e_shnum):
            old_off = sh_offsets[i]
            if i == text_idx:
                continue  # .text offset unchanged; we only changed sh_size
            if old_off >= insert_at:
                new_off = old_off + extra
                sh_base = new_e_shoff + i * e_shentsize
                struct.pack_into("<I", data, sh_base + 16, new_off)

        obj.write_bytes(bytes(data))
        return True


# Keep the old name as an alias for backward compatibility.
def truncate_text(obj: Path, expected_size: int) -> bool:
    """Alias for resize_text (truncation direction only)."""
    return resize_text(obj, expected_size)


def fix_pc16_addend(obj: Path) -> bool:
    """Zero out the addend in all R_MIPS_PC16 branch instructions for mwldmips.

    GNU-as encodes the addend for R_MIPS_PC16 (PC-relative branch) relocations
    as -1 (0xffff) in the instruction word's low 16 bits.  mwldmips applies
    R_MIPS_PC16 as:

        result = (symbol_value + addend - (branch_pc + 4)) / 4

    This is one extra pipeline-slot subtraction compared to what GNU-ld does
    (which uses branch_pc, not branch_pc+4).  With addend=-1:

        mwldmips: (S - 1 - (P + 4)) / 4 = (S - P - 5) / 4   ← off by one instruction

    Setting addend=0 corrects this:

        mwldmips: (S + 0 - (P + 4)) / 4 = (S - P - 4) / 4   ← correct MIPS branch formula

    This function reads .rel.text, finds every R_MIPS_PC16 entry, and sets the
    corresponding instruction's low 16 bits to 0.

    Returns True if any instruction was patched.
    """
    R_MIPS_PC16 = 10

    data = bytearray(obj.read_bytes())

    e_shoff     = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum     = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx  = struct.unpack_from("<H", data, 50)[0]

    def sh(idx: int) -> tuple:
        off = e_shoff + idx * e_shentsize
        return struct.unpack_from("<10I", data, off)

    shstr_sh   = sh(e_shstrndx)
    shstr_data = data[shstr_sh[4]: shstr_sh[4] + shstr_sh[5]]

    def sh_name(idx: int) -> str:
        name_off = sh(idx)[0]
        end = shstr_data.index(b"\x00", name_off)
        return shstr_data[name_off:end].decode("ascii", errors="replace")

    text_idx = rel_text_idx = -1
    for i in range(e_shnum):
        n = sh_name(i)
        if n == ".text":
            text_idx = i
        elif n == ".rel.text":
            rel_text_idx = i

    if text_idx < 0 or rel_text_idx < 0:
        return False

    text_sh = sh(text_idx)
    text_off = text_sh[4]
    text_size = text_sh[5]

    rel_sh    = sh(rel_text_idx)
    rel_off   = rel_sh[4]
    rel_size  = rel_sh[5]
    rel_entsz = rel_sh[9] or 8

    n_entries = rel_size // rel_entsz
    patched = 0

    for i in range(n_entries):
        entry_off = rel_off + i * rel_entsz
        r_offset, r_info = struct.unpack_from("<II", data, entry_off)
        r_type = r_info & 0xFF

        if r_type != R_MIPS_PC16:
            continue
        if r_offset + 4 > text_size:
            continue

        # Zero the low 16 bits (addend field) of the branch instruction.
        instr_off = text_off + r_offset
        instr = struct.unpack_from("<I", data, instr_off)[0]
        new_instr = instr & 0xFFFF0000
        if new_instr != instr:
            struct.pack_into("<I", data, instr_off, new_instr)
            patched += 1

    if patched == 0:
        return False

    obj.write_bytes(bytes(data))
    return True


def process(obj: Path, expected_text_size: int = 0) -> bool:
    """Strip unwanted sections, fix .text alignment, resize, pre-apply GPREL16, fix PC16.

    expected_text_size: if > 0, resize .text to this exact byte count.
        For GNU-as assembled objects (slot_size < 16-byte-padded size), this
        removes trailing padding.  For mwcc objects with trailing gap bytes,
        this appends zero bytes to fill the inter-function slot.

    Returns True if the object was modified in any way.
    """
    bad = unwanted_sections(obj)
    changed = False
    if bad:
        strip(obj, bad)
        changed = True
    if fix_text_alignment(obj):
        changed = True
    if expected_text_size > 0 and resize_text(obj, expected_text_size):
        changed = True
    if apply_gprel16(obj):
        changed = True
    if fix_pc16_addend(obj):
        changed = True
    return changed


if __name__ == "__main__":
    import argparse as _argparse
    ap = _argparse.ArgumentParser(
        description="Strip/fix ELF object files for mwldmips compatibility"
    )
    ap.add_argument("files", nargs="+", metavar="file.o")
    ap.add_argument(
        "--expected-size", type=int, default=0, metavar="N",
        help="truncate .text to exactly N bytes (removes GNU-as 16-byte alignment padding)"
    )
    args = ap.parse_args()
    for arg in args.files:
        p = Path(arg)
        if not p.exists():
            print(f"[strip_sections] warning: {p} does not exist", file=sys.stderr)
            continue
        if process(p, expected_text_size=args.expected_size):
            print(f"[strip_sections] stripped/fixed {p}")
