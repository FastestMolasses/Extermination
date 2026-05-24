#!/usr/bin/env python3
"""
tools/overlay/link_overlay.py — Link one overlay via GNU ld to a raw binary.

mwldmips (the period-correct Metrowerks linker) segfaults for small overlay
links under qemu-i386 + wibo32 on aarch64 — a known toolchain limitation.
GNU mipsel-linux-gnu-ld produces byte-identical output for overlay code
because the overlay objects have no relocations that differ between the two
linkers (all GPREL16 references are pre-applied by strip_sections.py, and
the absolute symbol addresses are identical).  mwldmips continues to be used
for the boot ELF (thousands of objects; the larger link doesn't crash).

Pipeline:
  1. (Optionally) runs fill_overlay.py to assemble all .s → .o.
  2. Generates a GNU ld script at config/overlays/AREAXX.lds.
  3. Invokes mipsel-linux-gnu-ld.
  4. Extracts the loadable region (text+data at vram 0x823500) from the ELF.
  5. Prepends the 64-byte MWo3 header (copied verbatim from the original BIN).
  6. Verifies byte-identity against the original disc file.

Usage (inside exterm-toolchain container, repo at /work):
    python3 tools/overlay/link_overlay.py AREA18 [--no-fill] [--dry-run] [--no-verify]

Or via container CLI from host:
    container run --rm -v "$PWD:/work" -w /work exterm-toolchain \\
        python3 tools/overlay/link_overlay.py AREA18
"""

from __future__ import annotations

import argparse
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

# Overlay arena base — all overlays load here.
ARENA_BASE = 0x00823500
GP_VALUE   = 0x0027D370

# GNU MIPS cross-linker (runs natively on aarch64 Linux inside the container).
GNU_LD = ["mipsel-linux-gnu-ld"]

_NONMATCHING_RE = re.compile(r"^nonmatching\s+\S+,\s+(0x[0-9A-Fa-f]+|\d+)")


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _vram_from_asm(path: Path) -> int:
    for line in path.read_text(errors="replace").splitlines()[:30]:
        if "/*" in line:
            try:
                comment = line.split("/*")[1].split("*/")[0].strip()
                parts = comment.split()
                if len(parts) >= 2:
                    return int(parts[1], 16)
            except (ValueError, IndexError):
                pass
    m = re.search(r'([0-9A-Fa-f]{8})$', path.stem)
    return int(m.group(1), 16) if m else 0


def sorted_functions(asm_dir: Path) -> list[str]:
    """Return function names in vram order."""
    entries: list[tuple[int, str]] = []
    for f in asm_dir.glob("*.s"):
        vram = _vram_from_asm(f)
        entries.append((vram, f.stem))
    entries.sort()
    return [name for _, name in entries]


def data_section_stems(overlay_build: Path) -> list[str]:
    """Return filler stems for data section objects ('data_section_0', etc.)."""
    data_dir = overlay_build / "asm" / "data"
    if not data_dir.exists():
        return []
    # Match fill_overlay.py: exclude _header.s, enumerate remainder as data_section_N.
    s_files = sorted(f for f in data_dir.glob("*.s")
                     if not f.name.endswith("_header.s"))
    return [f"data_section_{i}" for i in range(len(s_files))]


# ---------------------------------------------------------------------------
# Parse MWo3 header from the original disc file
# ---------------------------------------------------------------------------

MWO3_HDR_SIZE = 0x40

def parse_mwo3_header(data: bytes) -> dict:
    if data[:4] != b'MWo3':
        raise ValueError(f"Bad MWo3 magic: {data[:4]!r}")
    u32 = lambda o: struct.unpack_from('<I', data, o)[0]
    return {
        'overlay_id':              u32(0x04),
        'load_address':            u32(0x08),
        'text_size':               u32(0x0C),
        'data_size':               u32(0x10),
        'bss_size':                u32(0x14),
        'static_init_address':     u32(0x18),
        'static_init_end_address': u32(0x1C),
        'overlay_name':            data[0x20:0x40].rstrip(b'\x00').decode('ascii', errors='replace'),
    }


# ---------------------------------------------------------------------------
# GNU ld script generation
# ---------------------------------------------------------------------------

def generate_lds(name: str, funcs: list[str], data_stems: list[str],
                 hdr: dict, filler_dir: Path,
                 merged_code_obj: Path | None = None) -> str:
    """
    Generate a GNU ld linker script for one overlay.

    When merged_code_obj is provided (after a partial -r link of all code
    objects), we use it as the single code input — this resolves cross-object
    .L local-label branch targets that splat generates between adjacent
    functions.  When not provided, individual code objects are used.

    All external symbol references (GP, EE vectors, BSS data labels) are
    defined as absolute so GNU ld can resolve them without an ELF symbol table.
    """
    load = hdr['load_address']   # 0x00823500

    # Absolute symbols block
    abs_lines = _build_abs_syms_lds(name)

    # INPUT() directive: absolute paths to all objects in link order.
    if merged_code_obj is not None:
        code_inputs = f'    "{merged_code_obj}"\n'
        code_text   = f'        "{merged_code_obj}" (.text)\n'
    else:
        code_inputs = "".join(f'    "{filler_dir}/{fn}.o"\n' for fn in funcs)
        code_text   = "".join(
            f'        "{filler_dir}/{fn}.o" (.text)\n' for fn in funcs)

    data_inputs = "".join(
        f'    "{filler_dir}/{stem}.o"\n' for stem in data_stems)
    data_data = "".join(
        f'        "{filler_dir}/{stem}.o" (.data)\n' for stem in data_stems)

    lds = f"""\
/* {name}.lds -- GNU ld linker script for Extermination overlay {name}
 * overlay_id={hdr['overlay_id']}, load=0x{load:08x}
 * GENERATED by tools/overlay/link_overlay.py -- do not edit by hand.
 */

INPUT(
{code_inputs}{data_inputs})

MEMORY
{{
    ovl (rwx) : ORIGIN = 0x{load:08x}, LENGTH = 0x{hdr['text_size'] + hdr['data_size'] + hdr['bss_size']:x}
}}

/* ---- Absolute symbols ---- */
{abs_lines}

SECTIONS
{{
    /* ---- TEXT ---- */
    .text 0x{load:08x} :
    {{
{code_text}        * (.text)
    }} > ovl

    /* ---- DATA ---- */
    .data :
    {{
{data_data}        * (.data)
        * (.rodata)
        * (.sdata)
    }} > ovl

    /* ---- BSS (not written to file) ---- */
    .bss (NOLOAD) :
    {{
        __bss_start = .;
        * (.bss)
        * (.sbss)
        * (COMMON)
        __bss_end = .;
    }} > ovl

    /* Discard everything else */
    /DISCARD/ : {{ * (.comment) * (.reginfo) * (.mwcats) * (.ctor) }}
}}
"""
    return lds


def _build_abs_syms_lds(name: str) -> str:
    """
    Build absolute symbol definitions for the GNU ld script.
    Reads from undefined_syms_auto.txt and undefined_funcs_auto.txt,
    and scans the .s files for D_XXXXXXXX / func_XXXXXXXX references.
    """
    overlay_build = ROOT / "build" / "overlays" / name
    asm_code_dir  = overlay_build / "asm" / "matchings"

    _d_re = re.compile(r'\bD_([0-9A-Fa-f]{7,8})\b')
    _f_re = re.compile(r'\bfunc_([0-9A-Fa-f]{7,8})\b')

    syms: dict[str, int] = {}

    # Standard EE syscall/exception vectors.
    syms["func_00000008"] = 0x00000008
    syms["func_0000000C"] = 0x0000000C

    # From splat-generated undefined symbol files.
    for auto_name in ("undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        auto_path = overlay_build / auto_name
        if not auto_path.exists():
            continue
        for line in auto_path.read_text().splitlines():
            m = re.match(r"^(\S+)\s*=\s*(0x[0-9A-Fa-f]+);", line)
            if m:
                syms[m.group(1)] = int(m.group(2), 16)

    # Scan .s files for address-encoded names.
    if asm_code_dir.exists():
        for s_file in asm_code_dir.rglob("*.s"):
            content = s_file.read_text(errors="replace")
            for m in _d_re.finditer(content):
                nm = m.group(0)
                if nm not in syms:
                    syms[nm] = int(m.group(1), 16)
            for m in _f_re.finditer(content):
                nm = m.group(0)
                addr = int(m.group(1), 16)
                # Only define addresses outside the overlay vram range as absolutes.
                if addr < ARENA_BASE or addr > 0x00900000:
                    if nm not in syms:
                        syms[nm] = addr

    if not syms:
        return ""
    lines = [f"{nm} = 0x{addr:08X};\n" for nm, addr in sorted(syms.items())]
    return "".join(lines)


# ---------------------------------------------------------------------------
# Fix R_MIPS_PC16 addend bias in partial-linked objects
# ---------------------------------------------------------------------------

R_MIPS_PC16 = 0x0A

def fix_pc16_addend(obj_path: Path) -> int:
    """Fix R_MIPS_PC16 relocations in a partial-linked object.

    GNU ld's R_MIPS_PC16 formula is (S - P) >> 2, but MIPS hardware branches
    compute target = P + 4 + offset*4, so the correct formula is
    (S - P - 4) >> 2.  When GNU as assembles a cross-object branch, it emits
    field=0 (addend=0), so GNU ld resolves to (S - P)/4 which is 1 too large.

    Fix: set the instruction's imm16 field (= implicit addend for REL) to -1
    (0xFFFF) for each R_MIPS_PC16 relocation whose current field is 0.  Then
    GNU ld computes (S + (-1) - P) / 4 = (S - P - 4) / 4 ✓.

    Modifies obj_path in-place.  Returns the number of relocations patched.
    """
    data = bytearray(obj_path.read_bytes())
    patched = 0

    # Parse ELF32 LE header.
    e_shoff     = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum     = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx  = struct.unpack_from("<H", data, 50)[0]

    shstr_off  = struct.unpack_from("<I", data, e_shoff + e_shstrndx * e_shentsize + 16)[0]
    shstr_size = struct.unpack_from("<I", data, e_shoff + e_shstrndx * e_shentsize + 20)[0]
    shstrtab = data[shstr_off: shstr_off + shstr_size]

    def sh_name(idx: int) -> str:
        off = struct.unpack_from("<I", data, e_shoff + idx * e_shentsize)[0]
        end = shstrtab.index(b"\x00", off)
        return shstrtab[off:end].decode("ascii", errors="replace")

    # Find all .rel.text sections and their linked .text section.
    for i in range(e_shnum):
        sh_off  = e_shoff + i * e_shentsize
        sh_type = struct.unpack_from("<I", data, sh_off + 4)[0]
        if sh_type != 9:  # SHT_REL
            continue
        name = sh_name(i)
        if ".text" not in name:
            continue
        # Get .text section file offset (sh_link → the section containing the code).
        sh_link    = struct.unpack_from("<I", data, sh_off + 28)[0]
        text_off   = struct.unpack_from("<I", data, e_shoff + sh_link * e_shentsize + 16)[0]
        # Parse relocation entries.
        rel_off     = struct.unpack_from("<I", data, sh_off + 16)[0]
        rel_size    = struct.unpack_from("<I", data, sh_off + 20)[0]
        rel_entsz   = struct.unpack_from("<I", data, sh_off + 36)[0] or 8
        n_entries   = rel_size // rel_entsz
        for j in range(n_entries):
            ent_off = rel_off + j * rel_entsz
            r_offset = struct.unpack_from("<I", data, ent_off)[0]
            r_info   = struct.unpack_from("<I", data, ent_off + 4)[0]
            r_type   = r_info & 0xFF
            if r_type != R_MIPS_PC16:
                continue
            # Read current imm16 field from the instruction.
            instr_off = text_off + r_offset
            if instr_off + 4 > len(data):
                continue
            instr = struct.unpack_from("<I", data, instr_off)[0]
            imm16 = instr & 0xFFFF
            if imm16 == 0xFFFF:
                continue  # Already patched.
            if imm16 != 0:
                # Non-zero means GNU as already baked in the offset (same-object
                # branch that survived the partial link as a reloc — unusual).
                continue
            # Set imm16 to 0xFFFF (-1) so GNU ld computes (S - P - 4) / 4.
            new_instr = (instr & 0xFFFF0000) | 0xFFFF
            struct.pack_into("<I", data, instr_off, new_instr)
            patched += 1

    obj_path.write_bytes(data)
    return patched


# ---------------------------------------------------------------------------
# ELF extraction: pull raw text+data from the GNU ld output ELF
# ---------------------------------------------------------------------------

def extract_raw_from_elf(elf_path: Path, text_size: int, data_size: int) -> bytes | None:
    """Extract the loadable text+data content from the GNU ld ELF.

    GNU ld may place a large LOAD that spans from an aligned base (e.g. 0x820000)
    to include both .text and .data.  We use section headers to find .text and
    .data precisely rather than relying on the program header.
    """
    data = elf_path.read_bytes()
    if data[:4] != b'\x7fELF':
        # Already a raw binary.
        return data

    le = data[5] == 1  # EI_DATA == ELFDATA2LSB
    u16 = lambda o: struct.unpack_from('<H' if le else '>H', data, o)[0]
    u32 = lambda o: struct.unpack_from('<I' if le else '>I', data, o)[0]

    e_shoff     = u32(32)
    e_shentsize = u16(46)
    e_shnum     = u16(48)
    e_shstrndx  = u16(50)

    # Get shstrtab
    shstr_sh = e_shoff + e_shstrndx * e_shentsize
    shstr_off  = u32(shstr_sh + 16)
    shstr_size = u32(shstr_sh + 20)
    shstrtab = data[shstr_off: shstr_off + shstr_size]

    text_offset = data_offset = None
    text_vaddr  = data_vaddr  = None

    for i in range(e_shnum):
        sh = e_shoff + i * e_shentsize
        sh_name_idx = u32(sh)
        sh_addr     = u32(sh + 12)
        sh_offset   = u32(sh + 16)
        sh_size     = u32(sh + 20)
        name = shstrtab[sh_name_idx:].split(b'\x00')[0].decode()
        if name == '.text':
            text_offset = sh_offset
            text_vaddr  = sh_addr
        elif name == '.data':
            data_offset = sh_offset
            data_vaddr  = sh_addr

    if text_offset is None:
        return None

    # Extract text; if there's a .data section extract it too.
    raw = data[text_offset: text_offset + text_size]
    if data_offset is not None and data_size > 0:
        raw += data[data_offset: data_offset + data_size]

    return raw


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(
        description="Link an overlay via GNU ld and verify byte-identity")
    ap.add_argument("area", help="Overlay name, e.g. AREA18")
    ap.add_argument("--no-fill", action="store_true",
                    help="skip fill_overlay.py (use existing filler/)")
    ap.add_argument("--dry-run", action="store_true",
                    help="generate linker script but don't invoke ld")
    ap.add_argument("--no-verify", action="store_true",
                    help="skip byte comparison against original")
    args = ap.parse_args(argv)

    name = args.area.upper()
    overlay_build = ROOT / "build" / "overlays" / name
    asm_dir       = overlay_build / "asm" / "matchings" / name / "code"
    filler_dir    = overlay_build / "filler"
    config_dir    = ROOT / "config" / "overlays"
    lds_path      = config_dir / f"{name}.lds"
    out_elf_path  = overlay_build / f"{name}_raw.elf"
    out_bin_path  = overlay_build / f"{name}.BIN"
    orig_bin_path = ROOT / "extract" / "OVERLAY" / f"{name}.BIN"

    if not orig_bin_path.exists():
        sys.exit(f"error: original {orig_bin_path} not found. Extract ISO first.")
    orig_data = orig_bin_path.read_bytes()
    hdr = parse_mwo3_header(orig_data)
    print(f"[link] {name}: overlay_id={hdr['overlay_id']}, "
          f"text=0x{hdr['text_size']:x}, data=0x{hdr['data_size']:x}, "
          f"bss=0x{hdr['bss_size']:x}")

    # Step 1: fill filler objects
    if not args.no_fill:
        print(f"[link] running fill_overlay.py {name} ...")
        r = subprocess.run(
            [sys.executable,
             str(ROOT / "tools" / "overlay" / "fill_overlay.py"),
             name, "--jobs", "8"],
            cwd=ROOT,
        )
        if r.returncode != 0:
            print("[link] fill_overlay.py failed", file=sys.stderr)
            return 1

    # Step 2: generate GNU ld script
    if not asm_dir.exists():
        sys.exit(f"error: {asm_dir} not found. Run splat + fill first.")
    funcs = sorted_functions(asm_dir)
    d_stems = data_section_stems(overlay_build)
    print(f"[link] {len(funcs)} code functions, {len(d_stems)} data section(s)")

    config_dir.mkdir(parents=True, exist_ok=True)
    lds_text = generate_lds(name, funcs, d_stems, hdr, filler_dir)
    lds_path.write_text(lds_text)
    print(f"[link] wrote {lds_path.relative_to(ROOT)}")

    if args.dry_run:
        print("[link] dry-run: stopping before linker invocation")
        return 0

    # Step 3: verify all objects exist
    missing: list[str] = []
    for fn in funcs:
        if not (filler_dir / f"{fn}.o").exists():
            missing.append(fn)
    for stem in d_stems:
        if not (filler_dir / f"{stem}.o").exists():
            missing.append(stem)
    if missing:
        print(f"[link] WARNING: {len(missing)} objects missing from filler/:",
              file=sys.stderr)
        for m in missing[:5]:
            print(f"  {m}", file=sys.stderr)

    # Step 4a: partial relocatable link of all code objects.
    # Splat can emit cross-object branch targets as .L (GNU local) labels, which
    # are undefined between object files.  A partial link (-r) merges all code
    # objects into one, resolving .L refs before the final link.
    overlay_build.mkdir(parents=True, exist_ok=True)
    code_objs = [str(filler_dir / f"{fn}.o") for fn in funcs]
    merged_code_obj = filler_dir / "_code_merged.o"

    partial_cmd = GNU_LD + ["-r", "-o", str(merged_code_obj)] + code_objs
    print(f"[link] partial link ({len(code_objs)} code objects) ...")
    partial_result = subprocess.run(partial_cmd, cwd=ROOT, capture_output=True, text=True)
    if partial_result.returncode != 0:
        print(f"[link] partial link failed:\n{partial_result.stderr}", file=sys.stderr)
        return 1

    # Fix R_MIPS_PC16 addend bias: GNU ld applies (S-P)/4 but MIPS hardware uses
    # (S-P-4)/4.  Patch instruction fields to -1 so GNU ld produces correct offsets.
    n_fixed = fix_pc16_addend(merged_code_obj)
    if n_fixed:
        print(f"[link] fixed {n_fixed} R_MIPS_PC16 addend(s) in merged object")

    # Step 4b: generate final LDS using the merged code object.
    lds_text = generate_lds(name, funcs, d_stems, hdr, filler_dir,
                            merged_code_obj=merged_code_obj)
    lds_path.write_text(lds_text)

    # Step 4c: final link with GNU ld
    cmd = GNU_LD + [
        "-nostdlib",
        "-T", str(lds_path),
        "-o", str(out_elf_path),
    ]

    print(f"[link] final link ...")
    result = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if result.stdout:
        print(result.stdout, end="")
    # Filter harmless warnings (RWX segment warning is expected for overlay)
    stderr_lines = [l for l in result.stderr.splitlines()
                    if "RWX permissions" not in l]
    if stderr_lines:
        print("\n".join(stderr_lines), file=sys.stderr)
    if result.returncode != 0:
        print(f"[link] GNU ld failed (exit {result.returncode})", file=sys.stderr)
        return 1

    ls = out_elf_path.stat().st_size
    print(f"[link] GNU ld succeeded → {out_elf_path.relative_to(ROOT)} ({ls} bytes)")

    # Step 5: extract text+data from ELF
    raw_content = extract_raw_from_elf(out_elf_path, hdr['text_size'], hdr['data_size'])
    if raw_content is None:
        print("[link] ERROR: could not extract .text/.data from ELF", file=sys.stderr)
        return 1
    expected_payload = hdr['text_size'] + hdr['data_size']
    if len(raw_content) != expected_payload:
        print(f"[link] ERROR: extracted {len(raw_content)} bytes, "
              f"expected {expected_payload}", file=sys.stderr)
        return 1
    print(f"[link] extracted {len(raw_content)} bytes (text+data)")

    # Step 6: prepend MWo3 header → final .BIN
    header_bytes = orig_data[:MWO3_HDR_SIZE]
    rebuilt = header_bytes + raw_content
    out_bin_path.write_bytes(rebuilt)
    print(f"[link] wrote {out_bin_path.relative_to(ROOT)} ({len(rebuilt)} bytes)")

    # Step 7: byte-identity check
    if not args.no_verify:
        compare_size = hdr['text_size'] + hdr['data_size']
        orig_payload    = orig_data[MWO3_HDR_SIZE: MWO3_HDR_SIZE + compare_size]
        rebuilt_payload = rebuilt[MWO3_HDR_SIZE: MWO3_HDR_SIZE + compare_size]

        if len(orig_payload) != len(rebuilt_payload):
            print(f"[verify] SIZE MISMATCH: orig text+data={len(orig_payload)}, "
                  f"rebuilt={len(rebuilt_payload)}")
            return 2

        if orig_payload == rebuilt_payload:
            print(f"[verify] PASS — {compare_size} text+data bytes are byte-identical")
            if rebuilt == orig_data:
                print(f"[verify] PASS — full file byte-identical ({len(orig_data)} bytes)")
            else:
                print(f"[verify] NOTE: header differs (unexpected — copied verbatim)")
            return 0
        else:
            diffs = sum(1 for a, b in zip(orig_payload, rebuilt_payload) if a != b)
            pct = (compare_size - diffs) / compare_size * 100
            print(f"[verify] FAIL — {diffs}/{compare_size} bytes differ "
                  f"({pct:.2f}% match)")
            for i, (a, b) in enumerate(zip(orig_payload, rebuilt_payload)):
                if a != b:
                    vram = ARENA_BASE + i
                    print(f"[verify]   first diff at vram 0x{vram:08x} "
                          f"(offset {i:#x}): rebuilt=0x{a:02x} orig=0x{b:02x}")
                    break
            return 2

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
