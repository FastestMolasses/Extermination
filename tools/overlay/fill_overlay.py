#!/usr/bin/env python3
"""
tools/overlay/fill_overlay.py — Assemble per-function .s files for an overlay into
build/overlays/AREAXX/filler/*.o ready for mwldmips.

Mirrors tools/decomp/fill_unmatched.py but for a single overlay.  At 0% match,
every function is assembled from splat's .s.  As C decompilations are added to
src/overlays/AREAXX/, compiled .o files from build/overlays/AREAXX/obj/ take
priority over the assembled originals.

Usage (inside exterm-toolchain container, repo at /work):
    python3 tools/overlay/fill_overlay.py AREA18 [--jobs N] [--clean]

Or via container CLI from host:
    container run --rm -v "$PWD:/work" -w /work exterm-toolchain \\
        python3 tools/overlay/fill_overlay.py AREA18
"""

from __future__ import annotations

import argparse
import re
import shutil
import struct
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STRIP_SCRIPT = ROOT / "tools" / "decomp" / "strip_sections.py"
PRELUDE = ROOT / "config" / "asm_prelude.inc"
ARENA_BASE = 0x00823500
MWO3_HDR_SIZE = 0x40


def _read_mwo3_header(name: str) -> dict | None:
    """Read the MWo3 header from the original overlay BIN file."""
    bin_path = ROOT / "extract" / "OVERLAY" / f"{name}.BIN"
    if not bin_path.exists():
        return None
    data = bin_path.read_bytes()
    if data[:4] != b'MWo3':
        return None
    u32 = lambda o: struct.unpack_from('<I', data, o)[0]
    return {
        'load_address': u32(0x08),
        'text_size':    u32(0x0C),
        'data_size':    u32(0x10),
    }

_NONMATCHING_RE = re.compile(r"^nonmatching\s+\S+,\s+(0x[0-9A-Fa-f]+|\d+)")


def _vram_from_asm(path: Path) -> int:
    """Extract the VRAM address from the first /* offset vram ... */ comment."""
    for line in path.read_text(errors="replace").splitlines()[:30]:
        if "/*" in line:
            try:
                comment = line.split("/*")[1].split("*/")[0].strip()
                parts = comment.split()
                if len(parts) >= 2:
                    return int(parts[1], 16)
            except (ValueError, IndexError):
                pass
    # Fallback: parse 8-hex-digit suffix from filename
    m = re.search(r'([0-9A-Fa-f]{8})$', path.stem)
    if m:
        return int(m.group(1), 16)
    return 0


def _nonmatching_size(asm_path: Path) -> int:
    """Return the declared original function size from the nonmatching directive."""
    try:
        for line in asm_path.read_text(errors="replace").splitlines()[:5]:
            m = _NONMATCHING_RE.match(line.strip())
            if m:
                sz_str = m.group(1)
                return int(sz_str, 16) if sz_str.startswith("0x") else int(sz_str)
    except Exception:
        pass
    return 0


def _needs_rebuild(src: Path, dst: Path) -> bool:
    if not dst.exists():
        return True
    return dst.stat().st_mtime < src.stat().st_mtime


def fix_vu0_instructions(asm_dir: Path) -> int:
    """Replace VU0 macro-mode mnemonics with raw .word directives.

    GNU as (mipsel-linux-gnu-as) does not support VU0 coprocessor instructions
    even with -march=r5900.  Splat emits them as decoded mnemonics but includes
    the raw 32-bit opcode in the comment: /* offset vram XXXXXXXX */

    We replace any line that fails to assemble VU0 instructions with:
        .word 0xXXXXXXXX  /* original mnemonic line */

    The raw opcode is extracted from the existing comment.

    Returns the number of lines replaced.
    """
    # VU0 mnemonic prefixes that GNU as does not understand.
    _VU0_PREFIXES = (
        'vmulax', 'vmadday', 'vmaddaz', 'vmaddaw',
        'vaddax', 'vadday', 'vaddaz', 'vaddaw',
        'vsubax', 'vsubay', 'vsubaz', 'vsubaw',
        'vmulaq', 'vmaddq', 'vsubq', 'vaddq',
        'vmulai', 'vmaddai', 'vsubai', 'vaddai',
        'vmsuba', 'vmulax', 'vmuly', 'vmulz', 'vmulw',
        'vmul', 'vadd', 'vsub', 'vmadd', 'vmsub',
        'vabs', 'vclipw', 'vftoi', 'vitof', 'vmini', 'vmax',
        'visub', 'viadd', 'viand', 'vior',
        'viswr', 'vilwr', 'visqrt', 'vrsqrt', 'vwaitq',
        'vcallms', 'vcallmsr', 'vnop', 'vsqrt',
    )

    # Match splat's disassembly format: "    /* offset vram OPCODE */  mnemonic ..."
    # The raw opcode is group 1, the mnemonic is group 2.
    _line_re = re.compile(
        r'^(\s+)/\*[^*]*([0-9A-Fa-f]{8})\s*\*/\s+(v[a-z][a-z0-9.]*)(.*)$')

    total = 0
    for f in sorted(asm_dir.glob("*.s")):
        content = f.read_text(errors="replace")
        lines = content.splitlines(keepends=True)
        changed = False
        new_lines = []
        for line in lines:
            m = _line_re.match(line)
            if m:
                indent, opcode, mnemonic, rest = m.group(1), m.group(2), m.group(3), m.group(4)
                if any(mnemonic.startswith(pfx) for pfx in _VU0_PREFIXES):
                    # Splat's comment shows the raw memory bytes as a hex string
                    # (e.g. "5808C24B" = bytes [0x58, 0x08, 0xC2, 0x4B] in file).
                    # GNU as .word X stores X as LE int32: bytes [X&FF, X>>8, X>>16, X>>24].
                    # To produce bytes [0x58, 0x08, 0xC2, 0x4B], we need X = 0x4BC20858,
                    # which is int.from_bytes([0x58, 0x08, 0xC2, 0x4B], 'little').
                    opcode_word = int.from_bytes(bytes.fromhex(opcode), 'little')
                    # Replace the instruction with a .word directive.
                    new_line = f"{indent}.word 0x{opcode_word:08X}  /* {mnemonic}{rest.rstrip()} */\n"
                    new_lines.append(new_line)
                    total += 1
                    changed = True
                    continue
            new_lines.append(line)
        if changed:
            f.write_text("".join(new_lines))

    return total


def fix_cross_file_local_labels(asm_dir: Path) -> int:
    """Rename .L labels that are defined in one .s file but referenced in
    another.  GNU as local labels (.L...) are not exported to the object's
    symbol table so a partial-link (-r) cannot resolve cross-object .L refs.
    We rename them to non-local labels (drop the leading '.') in both their
    definition and all reference sites.

    This modifies the .s files in-place.  Returns the number of labels renamed.
    """
    import re as _re
    _def_re  = _re.compile(r'^\s*(\.L\S+?):')
    _ref_re  = _re.compile(r'(?<![a-zA-Z0-9_])(\.L[a-zA-Z0-9_]+)')

    # Pass 1: build defined-label → file map and reference → files map.
    files = sorted(asm_dir.glob("*.s"))
    defined: dict[str, str] = {}    # label → defining filename
    refs:    dict[str, set] = {}    # label → set of filenames that reference it

    for f in files:
        content = f.read_text(errors="replace")
        for line in content.splitlines():
            m = _def_re.match(line)
            if m:
                lbl = m.group(1)
                defined[lbl] = f.name
            for m2 in _ref_re.finditer(line):
                lbl = m2.group(1)
                refs.setdefault(lbl, set()).add(f.name)

    # Cross-file: defined in file A but referenced in file B ≠ A.
    cross = {lbl for lbl, dfile in defined.items()
             if lbl in refs and refs[lbl] - {dfile}}

    if not cross:
        return 0

    # Pass 2: in every file, replace each cross-file .L label with a global name.
    # Strategy:
    #   - Drop the leading '.' to remove the GNU-as "local" prefix.
    #   - In the defining file, add a ".globl <newname>" directive immediately
    #     before the label definition so GNU as exports it.
    #   - In referencing files, just do a text rename.
    renames = {lbl: lbl[1:] for lbl in cross}   # ".Lfoo" → "Lfoo"

    for f in files:
        content = f.read_text(errors="replace")
        new_content = content

        # First rename all .L occurrences to the non-local form (both def & ref).
        for old, new in renames.items():
            new_content = new_content.replace(old, new)

        # Then add .globl directive before each definition that lives in this file.
        # A label definition looks like "  Lfoo:" (possibly with leading whitespace).
        defs_in_this_file = {renames[lbl] for lbl, dfile in defined.items()
                             if dfile == f.name and lbl in renames}
        if defs_in_this_file:
            lines = new_content.splitlines(keepends=True)
            out_lines = []
            label_def_plain = _re.compile(r'^\s*(%s):' % '|'.join(
                _re.escape(n) for n in sorted(defs_in_this_file, key=len, reverse=True)))
            for line in lines:
                m = label_def_plain.match(line)
                if m:
                    nm = m.group(1)
                    if f'.globl {nm}' not in ''.join(out_lines[-2:]):
                        out_lines.append(f'    .globl {nm}\n')
                out_lines.append(line)
            new_content = ''.join(out_lines)

        if new_content != content:
            f.write_text(new_content)

    return len(cross)


def collect_functions(asm_dir: Path) -> list[tuple[int, str, Path]]:
    """Return sorted list of (vram, name, asm_path) for all .s in asm_dir."""
    entries: list[tuple[int, str, Path]] = []
    for f in asm_dir.glob("*.s"):
        vram = _vram_from_asm(f)
        entries.append((vram, f.stem, f))
    entries.sort()
    return entries


def compute_slot_sizes(entries: list[tuple[int, str, Path]],
                       text_end_vram: int = 0) -> dict[str, int]:
    """Return name → slot_size mapping (next_vram - this_vram).

    text_end_vram: vram of the end of the text section (load_address + text_size).
    If provided, the last function's slot extends to this address, capturing any
    trailing nop-pad bytes the original linker placed after the last function.
    """
    slot_sizes: dict[str, int] = {}
    for i, (vram, name, asm_path) in enumerate(entries):
        if i + 1 < len(entries):
            next_vram = entries[i + 1][0]
        else:
            # Last function: slot extends to end of text section if known,
            # otherwise fall back to the nonmatching declared size.
            if text_end_vram > vram:
                next_vram = text_end_vram
            else:
                nm = _nonmatching_size(asm_path)
                next_vram = vram + (nm if nm > 0 else 0)
        slot_sizes[name] = next_vram - vram
    return slot_sizes


def _assemble(asm_path: Path, out_path: Path, macro_inc: Path,
              slot_size: int) -> Exception | None:
    """Assemble asm_path → out_path, then strip/fix. Returns error or None."""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    cmd = [
        "mipsel-linux-gnu-as",
        "-march=r5900",
        "-L",  # keep .L local labels in symbol table so partial-link can resolve
               # cross-function .L branch targets (splat splits at func boundaries,
               # but branches in MIPS can legally jump into adjacent functions).
        "-I", str(macro_inc.parent),  # so ".include macro.inc" resolves
        str(PRELUDE),
        str(macro_inc),
        str(asm_path),
        "-o", str(out_path),
    ]
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        return e

    # strip_sections.py: remove GNU-as sections, fix alignment, pre-apply GPREL16,
    # and resize .text to slot_size so inter-function gap bytes are included.
    strip_cmd = [sys.executable, str(STRIP_SCRIPT), str(out_path)]
    if slot_size > 0:
        strip_cmd += ["--expected-size", str(slot_size)]
    try:
        subprocess.run(strip_cmd, check=True, capture_output=True)
    except subprocess.CalledProcessError:
        pass  # Non-fatal; mwldmips will surface the real error.
    return None


def _normalize_mwcc_abi(obj_path: Path) -> None:
    """Patch a mwcc-compiled ELF's e_flags so GNU ld will link it alongside
    GNU-as-assembled overlay objects.

    mwccmips emits Flags=0x20924001 (eabi64), while GNU as with -march=r5900
    emits Flags=0x20921101 (o32+32bitmode).  GNU ld refuses to merge the two
    ABIs.  The two flag words differ only in the ABI-selection bits:
      EF_MIPS_ABI_O32    = 0x00001000   (set in GNU-as output)
      EF_MIPS_32BITMODE  = 0x00000100   (set in GNU-as output)
      EF_MIPS_ABI_EABI64 = 0x00004000   (set in mwcc output)

    The .text bytes and relocations don't change between the two ABIs for the
    instructions we use, so it's safe to rewrite the flags to the o32 form.
    Boot-ELF builds link with mwldmips (which accepts eabi64 directly) and
    don't run this codepath.
    """
    data = bytearray(obj_path.read_bytes())
    if data[:4] != b'\x7fELF':
        return
    # e_flags is at offset 36 in ELF32 header.
    flags = struct.unpack_from('<I', data, 36)[0]
    # Clear EABI64 bit, set O32 + 32bitmode.
    new_flags = (flags & ~0x00006000) | 0x00001100
    if new_flags != flags:
        struct.pack_into('<I', data, 36, new_flags)
        obj_path.write_bytes(bytes(data))


def assemble_one(name: str, asm_path: Path, out_path: Path,
                 obj_path: Path | None, macro_inc: Path,
                 slot_size: int) -> tuple[str, str, Exception | None]:
    """Assemble or copy one object. Returns (name, status, err)."""
    # Prefer compiled obj if available (future: matched C decompilation).
    if obj_path is not None and obj_path.exists():
        if _needs_rebuild(obj_path, out_path):
            shutil.copy2(obj_path, out_path)
            # Normalize the mwcc-emitted EABI64 flags to o32 so GNU ld will
            # link this object alongside GNU-as-produced overlay objects.
            _normalize_mwcc_abi(out_path)
            strip_cmd = [sys.executable, str(STRIP_SCRIPT), str(out_path)]
            if slot_size > 0:
                strip_cmd += ["--expected-size", str(slot_size)]
            subprocess.run(strip_cmd, check=True, capture_output=True)
        return name, "copied", None

    # Assemble from splat .s
    if not _needs_rebuild(asm_path, out_path):
        return name, "cached", None
    err = _assemble(asm_path, out_path, macro_inc, slot_size)
    return name, "assembled" if err is None else "asm_error", err


def assemble_code_as_single_unit(
        entries: list[tuple[int, str, Path]],
        slot_sizes: dict[str, int],
        filler_dir: Path,
        macro_inc: Path,
        mwo3_text_size: int = 0,
) -> Path | None:
    """Assemble all code .s files as ONE concatenated translation unit.

    This resolves all cross-function branch targets (including .L local labels
    and R_MIPS_PC16 relocations) within a single GNU-as pass, so the linker
    sees a single code object with no intra-overlay relocations to apply.

    Returns the path to the assembled code object, or None on error.

    Side effect: individual per-function .o files are NOT produced (the code
    is all in one object).  The LDS file must reference this single object
    rather than per-function objects.
    """
    # Build a concatenated .s that includes all per-function .s files in order.
    # Each file gets a .text subsection with no alignment between functions
    # (we rely on strip_sections.py having padded each function to its slot size).
    concat_s = filler_dir / "_all_code.s"
    out_o    = filler_dir / "_all_code.o"

    lines = []
    lines.append("    .text\n")
    lines.append("    .set noreorder\n")
    lines.append("    .set noat\n")
    lines.append("\n")

    for vram, name, asm_path in entries:
        lines.append(f"    /* === {name} (vram 0x{vram:08x}) === */\n")
        lines.append(asm_path.read_text(errors="replace"))
        lines.append("\n")

    concat_s.write_text("".join(lines))

    # Assemble the concatenated source.
    cmd = [
        "mipsel-linux-gnu-as",
        "-march=r5900",
        "-I", str(macro_inc.parent),
        str(PRELUDE),
        str(macro_inc),
        str(concat_s),
        "-o", str(out_o),
    ]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        print(f"  ERROR in single-unit assembly:\n{r.stderr.strip()}",
              file=sys.stderr)
        return None

    # Run strip_sections.py to clean up GNU-as sections.
    # For the single-unit, expected size = mwo3_text_size (total text).
    strip_cmd = [sys.executable, str(STRIP_SCRIPT), str(out_o)]
    if mwo3_text_size > 0:
        strip_cmd += ["--expected-size", str(mwo3_text_size)]
    subprocess.run(strip_cmd, capture_output=True)

    return out_o


def assemble_data_sections(overlay_build: Path, filler_dir: Path,
                            macro_inc: Path) -> list[tuple[str, Path]]:
    """
    Assemble all .s files in asm/data/ into filler .o files.

    Returns list of (link_stem, out_path) pairs to be added to the link order,
    placed after all text objects.

    Note: splat names data files like '100.data.s' (starts with a digit, has
    a dot in the stem).  Both are illegal as mwldmips object filenames.  We
    rename them to 'data_section_N.o' for the filler directory.
    """
    data_asm_dir = overlay_build / "asm" / "data"
    if not data_asm_dir.exists():
        return []

    # Exclude the MWo3 header blob (not code/data to link — we prepend it ourselves).
    s_files = sorted(f for f in data_asm_dir.glob("*.s")
                     if not f.name.endswith("_header.s"))

    results = []
    for i, s_file in enumerate(s_files):
        link_stem = f"data_section_{i}"
        out_path = filler_dir / f"{link_stem}.o"
        if not _needs_rebuild(s_file, out_path):
            results.append((link_stem, out_path))
            continue

        cmd = [
            "mipsel-linux-gnu-as",
            "-march=r5900",
            "-I", str(macro_inc.parent),  # so ".include macro.inc" resolves
            str(PRELUDE),
            str(macro_inc),
            str(s_file),
            "-o", str(out_path),
        ]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            print(f"  ERROR assembling data {s_file.name}:\n{r.stderr.strip()}",
                  file=sys.stderr)
        else:
            # Strip GNU-as-specific sections (no slot-size resize for data).
            strip_cmd = [sys.executable, str(STRIP_SCRIPT), str(out_path)]
            subprocess.run(strip_cmd, capture_output=True)
            results.append((link_stem, out_path))
            print(f"  assembled data {s_file.name} -> {link_stem}.o")

    return results


def main():
    ap = argparse.ArgumentParser(
        description="Assemble overlay .s files to filler .o for mwldmips")
    ap.add_argument("area", help="Overlay name, e.g. AREA18")
    ap.add_argument("--jobs", "-j", type=int, default=4)
    ap.add_argument("--clean", action="store_true",
                    help="Wipe filler dir before building")
    args = ap.parse_args()

    name = args.area.upper()
    overlay_build = ROOT / "build" / "overlays" / name
    asm_dir = overlay_build / "asm" / "matchings" / name / "code"
    filler_dir = overlay_build / "filler"
    obj_dir = overlay_build / "obj"
    macro_inc = overlay_build / "macro.inc"

    if not asm_dir.exists():
        sys.exit(
            f"error: {asm_dir} not found.\n"
            f"Run splat first:\n"
            f"  .venv/bin/python -m splat split config/overlays/{name}.yaml"
        )
    if not macro_inc.exists():
        sys.exit(f"error: {macro_inc} not found. Run splat first.")

    if args.clean and filler_dir.exists():
        shutil.rmtree(filler_dir)
        print(f"[fill] cleaned {filler_dir.relative_to(ROOT)}")
    filler_dir.mkdir(parents=True, exist_ok=True)

    # Fix VU0 (COP2 vector) instructions: GNU as doesn't support them.
    # Replace with .word directives using the raw opcode from splat's comments.
    n_vu0 = fix_vu0_instructions(asm_dir)
    if n_vu0:
        print(f"[fill] replaced {n_vu0} VU0 instruction(s) with .word directives")

    # Fix cross-file .L label references before assembly.
    # Splat may generate branches from one function's .s into a label defined in
    # the adjacent function's .s.  GNU as local labels (.L...) are not exported
    # to the object symbol table, so cross-object .L refs cause linker errors.
    # We rename them to non-local labels in-place (modifies the .s files).
    n_renamed = fix_cross_file_local_labels(asm_dir)
    if n_renamed:
        print(f"[fill] renamed {n_renamed} cross-file .L label(s) to non-local")

    # Gather all code .s files and compute slot sizes.
    entries = collect_functions(asm_dir)
    if not entries:
        sys.exit(f"error: no .s files found in {asm_dir}")

    # Read MWo3 header to get text_end_vram for last-function slot sizing.
    mwo3 = _read_mwo3_header(name)
    text_end_vram = 0
    if mwo3:
        text_end_vram = mwo3['load_address'] + mwo3['text_size']
        print(f"[fill] text_end_vram=0x{text_end_vram:08x} from MWo3 header")
    slot_sizes = compute_slot_sizes(entries, text_end_vram)

    print(f"[fill] {len(entries)} code functions for {name}")

    errors: list[tuple[str, Exception]] = []
    counts: dict[str, int] = {}

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = {}
        for vram, func_name, asm_path in entries:
            out_path = filler_dir / f"{func_name}.o"
            obj_path = obj_dir / f"{func_name}.o"
            slot = slot_sizes.get(func_name, 0)
            fut = pool.submit(
                assemble_one, func_name, asm_path, out_path,
                obj_path if obj_path.exists() else None,
                macro_inc, slot
            )
            futures[fut] = func_name

        for fut in as_completed(futures):
            fn, status, err = fut.result()
            counts[status] = counts.get(status, 0) + 1
            if err is not None:
                errors.append((fn, err))
                print(f"  ERROR assembling {fn}: {err}", file=sys.stderr)

    # Also assemble data sections.
    print(f"[fill] assembling data sections ...")
    data_objs = assemble_data_sections(overlay_build, filler_dir, macro_inc)
    print(f"[fill] {len(data_objs)} data section(s) assembled")

    print(
        f"[fill] done: "
        f"{counts.get('assembled', 0)} code assembled, "
        f"{counts.get('copied', 0)} copied from obj/, "
        f"{counts.get('cached', 0)} cached, "
        f"{len(data_objs)} data, "
        f"{counts.get('asm_error', 0)} errors"
    )

    if errors:
        print(f"[fill] {len(errors)} error(s) — link will likely fail",
              file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
