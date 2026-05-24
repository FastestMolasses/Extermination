#!/usr/bin/env python3
"""tools/overlay/gen_asm_void.py

Bulk asm-void match generator for overlay functions, modeled on
/tmp/gen_hybrid.py and /tmp/gen_nonleaf_broad.py (boot-ELF versions).

For each overlay AREAXX:
  - Walks build/overlays/AREAXX/asm/matchings/AREAXX/code/*.s
  - Skips functions already matched (src/overlays/AREAXX/<name>.c exists)
  - Filters out functions with data relocs (%hi/%lo/%gp_rel), jalr/syscall/j-to-symbol,
    and size-out-of-range candidates
  - Generates src/overlays/AREAXX/<name>.c using the hybrid asm-void pattern:
      .word 0xXXXX for branches (mwcc rejects PC-relative labels)
      jal SYM      for direct calls (real reloc via extern decl)
      real mnemonic for everything else (with $tN -> $N renaming)
  - Compiles it with mwccmips in the toolchain container
  - Compares the compiled .o against an assembled-from-asm reference .o via
    objdiff-cli; keeps the .c only if .text match_percent == 100.0

Usage (must be run inside the toolchain container so mwccmips + GNU as are
available directly without container-launch overhead per candidate):
    container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c \\
        "python3 tools/overlay/gen_asm_void.py --all"
    container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c \\
        "python3 tools/overlay/gen_asm_void.py --area AREA00"

objdiff-cli is an arm64-macOS host binary — when running inside the linux/amd64
container we cannot call it. Instead this script compares the .text bytes of
the assembled-from-asm reference .o and the mwcc-compiled candidate .o
directly (extracted via mipsel-linux-gnu-objcopy / elf parsing).

After running, link each overlay end-to-end to confirm byte-identity:
    container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c \\
        "python3 tools/overlay/build.py --all --no-extract --no-yaml --no-splat"
"""
from __future__ import annotations
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDIFF = ROOT / "tools" / "bin" / "objdiff-cli"
PRELUDE = ROOT / "config" / "asm_prelude.inc"

HDR = ("// Hybrid asm void: real mnemonics where mwcc accepts them,\n"
       "// .word for branch instructions (mwcc rejects PC-relative labels).\n"
       "// CFLAGS: -O4,p -sdatathreshold 4\n")

ALL_OVERLAYS = [
    "AREA00", "AREA01", "AREA02", "AREA03", "AREA04",
    "AREA06", "AREA07", "AREA08", "AREA11", "AREA13",
    "AREA14", "AREA15", "AREA16", "AREA17", "AREA18",
    "AREA19", "AREA20", "AREA21", "AREA22",
]

INSN_LINE = re.compile(
    r"^\s*/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*(.+?)\s*$"
)

BRANCH_OPS = {
    "beq",  "bne",  "beqz", "bnez",
    "bgez", "blez", "bgtz", "bltz",
    "beql", "bnel", "beqzl","bnezl",
    "bgezl","blezl","bgtzl","bltzl",
    "b",    "bal",
    "bc1f", "bc1t", "bc1fl","bc1tl",
    "bgezal","bltzal","bgezall","bltzall",
}

T_MAP = {f"$t{i}": f"${8+i}" for i in range(8)}
T_MAP.update({"$t8": "$24", "$t9": "$25"})

SYMBOLIC_PREFIX_RE = re.compile(r"func_[0-9A-Fa-f]+|func_overlay_|sce[A-Z]|[A-Z][A-Za-z]")


def rr(s: str) -> str:
    for k, v in T_MAP.items():
        s = re.sub(rf"\{re.escape(k)}\b", v, s)
    return s


def parse_asm(text: str):
    """Return (insns, labels) for everything strictly between glabel and endlabel.

    insns is list of (hex_bytes_le, mnemonic_text) tuples.
    labels is dict mapping label_name (e.g. ".Loverlay_AREA00_...") -> insn index
    (number of instructions before that label is reached, i.e. the word offset).
    """
    insns: list[tuple[str, str]] = []
    labels: dict[str, int] = {}
    in_func = False
    for line in text.splitlines():
        s = line.strip()
        if s.startswith("glabel "):
            in_func = True
            continue
        if s.startswith("endlabel"):
            in_func = False
            continue
        if not in_func:
            continue
        # Label line like ".Loverlay_...:"
        m_lbl = re.match(r"^(\.[A-Za-z_][\w.]*):\s*$", s)
        if m_lbl:
            labels[m_lbl.group(1)] = len(insns)
            continue
        m = INSN_LINE.match(line)
        if m:
            insns.append((m.group(3), re.sub(r"\s+", " ", m.group(4).strip())))
    return insns, labels


def should_skip(insns) -> bool:
    for _hb, text in insns:
        toks = text.split()
        if not toks:
            continue
        op = toks[0]
        if "%hi" in text or "%lo" in text or "%gp_rel" in text:
            return True
        if op in ("jalr", "syscall"):
            return True
        if op == "j" and len(toks) > 1:
            target = toks[1]
            if SYMBOLIC_PREFIX_RE.match(target):
                return True
    return False


def word(hex_le: str) -> str:
    val = int.from_bytes(bytes.fromhex(hex_le), "little")
    return f".word 0x{val:08x}"


def generate_c(name: str, insns) -> str:
    callees = []
    for _hb, text in insns:
        if text.startswith("jal "):
            callee = text.split()[1]
            callees.append(callee)
    decls = "\n".join(f"extern void {c}(int, int, int, int);"
                       for c in sorted(set(callees)))
    body = []
    for hb, text in insns:
        toks = text.split()
        op = toks[0] if toks else ""
        if op in BRANCH_OPS:
            body.append(f"    {word(hb)}")
        elif op == "j" and len(toks) > 1:
            body.append(f"    {word(hb)}")
        else:
            body.append(f"    {rr(text)}")
    prefix = HDR + (decls + "\n\n" if decls else "\n")
    return prefix + f"asm void {name}(void) {{\n" + "\n".join(body) + "\n}\n"


_LABEL_REF_RE = re.compile(r'(?<![A-Za-z0-9_.])(\.L[A-Za-z0-9_]+|L[A-Za-z0-9_]+)')
_LABEL_DEF_RE = re.compile(r'^\s*(\.?L[A-Za-z0-9_]+):')


def _labels_defined_and_referenced(path: Path):
    """Return (defined, referenced) sets of label names in this .s file."""
    defined, referenced = set(), set()
    for line in path.read_text(errors="replace").splitlines():
        m = _LABEL_DEF_RE.match(line)
        if m:
            defined.add(m.group(1))
            continue
        # references appear after comments in operand position
        # strip the comment block first
        body = line.split("*/", 1)[1] if "*/" in line else line
        for r in _LABEL_REF_RE.findall(body):
            referenced.add(r)
    return defined, referenced


def scan_overlay(area: str):
    asm_dir = ROOT / "build" / "overlays" / area / "asm" / "matchings" / area / "code"
    src_dir = ROOT / "src" / "overlays" / area
    if not asm_dir.exists():
        return []

    # Build map: label_name -> defining file; and per-file referenced labels.
    file_defined: dict[str, set[str]] = {}
    file_referenced: dict[str, set[str]] = {}
    all_files = sorted(asm_dir.glob("*.s"))
    for f in all_files:
        d, r = _labels_defined_and_referenced(f)
        file_defined[f.name] = d
        file_referenced[f.name] = r

    # For each candidate's file, compute set of labels defined here that are
    # referenced by some OTHER file in the same overlay. If that set is
    # non-empty, the candidate cannot be silently replaced by mwcc output
    # (the labels would disappear from the symbol table).
    out = []
    for f in all_files:
        name = f.stem
        if (src_dir / f"{name}.c").exists():
            continue
        insns, _labels = parse_asm(f.read_text())
        if not insns:
            continue
        if not (3 <= len(insns) <= 300):
            continue
        if should_skip(insns):
            continue

        my_defs = file_defined[f.name]
        externally_used = False
        for other_name, refs in file_referenced.items():
            if other_name == f.name:
                continue
            if refs & my_defs:
                externally_used = True
                break
        if externally_used:
            continue
        out.append((name, insns, f))
    return out


def _extract_text_with_relocs(obj_path: Path) -> tuple[bytes, list] | None:
    """Return (.text bytes, list of (offset, r_type, sym_name)) for obj_path.

    Compares both the raw .text bytes and a normalized list of relocations so
    that two functionally-equivalent functions only match if their relocation
    targets line up (otherwise a 'jal func_X' vs 'jal func_Y' would look the
    same to a byte compare — both encode jal with target=0).
    """
    try:
        import struct
        data = obj_path.read_bytes()
        if data[:4] != b'\x7fELF' or data[4] != 1:  # ELF32 only
            return None
        # ELF32 header
        e_shoff = struct.unpack_from('<I', data, 0x20)[0]
        e_shentsize = struct.unpack_from('<H', data, 0x2E)[0]
        e_shnum = struct.unpack_from('<H', data, 0x30)[0]
        e_shstrndx = struct.unpack_from('<H', data, 0x32)[0]

        def shdr(i):
            o = e_shoff + i * e_shentsize
            return struct.unpack_from('<10I', data, o)
        # shstrtab
        sh_str = shdr(e_shstrndx)
        sh_str_off = sh_str[4]

        def name(i):
            shn = shdr(i)
            n = shn[0]
            end = data.index(b'\x00', sh_str_off + n)
            return data[sh_str_off + n:end].decode()

        sections = {name(i): (i, shdr(i)) for i in range(e_shnum)}
        if '.text' not in sections:
            return None
        _, text_sh = sections['.text']
        text_bytes = data[text_sh[4]:text_sh[4] + text_sh[5]]

        # Find symbol table
        symtab_idx = None
        for n, (i, sh) in sections.items():
            if sh[1] == 2:  # SHT_SYMTAB
                symtab_idx = i
                symtab_sh = sh
                break
        # strtab linked from symtab sh_link
        strtab_sh = shdr(symtab_sh[6])
        strtab_off = strtab_sh[4]
        def sym_name(stoff):
            end = data.index(b'\x00', strtab_off + stoff)
            return data[strtab_off + stoff:end].decode(errors='replace')

        # Parse relocations targeting .text section
        text_section_idx = sections['.text'][0]
        relocs = []
        for n, (i, sh) in sections.items():
            if sh[1] not in (9, 4):  # SHT_REL=9, SHT_RELA=4
                continue
            if sh[7] != text_section_idx:  # sh_info = section to which relocs apply
                continue
            ent = 8 if sh[1] == 9 else 12
            off = sh[4]
            cnt = sh[5] // ent
            for k in range(cnt):
                r_off, r_info = struct.unpack_from('<II', data, off + k * ent)
                r_sym = r_info >> 8
                r_type = r_info & 0xff
                # Look up symbol name
                sym_off = symtab_sh[4] + r_sym * 16
                st_name = struct.unpack_from('<I', data, sym_off)[0]
                relocs.append((r_off, r_type, sym_name(st_name)))
        relocs.sort()
        return text_bytes, relocs
    except Exception:
        return None


def build_candidate(area: str, name: str) -> str | None:
    """Compile candidate src + assemble expected; return '100.0' if byte+reloc match."""
    src_rel = f"src/overlays/{area}/{name}.c"
    asm_rel = f"build/overlays/{area}/asm/matchings/{area}/code/{name}.s"
    macro_rel = f"build/overlays/{area}/macro.inc"
    expected_rel = f"build/overlays/{area}/_genvoid_expected/{name}.o"
    obj_rel = f"build/overlays/{area}/_genvoid_obj/{name}.o"

    (ROOT / f"build/overlays/{area}/_genvoid_expected").mkdir(parents=True, exist_ok=True)
    (ROOT / f"build/overlays/{area}/_genvoid_obj").mkdir(parents=True, exist_ok=True)

    # Assemble expected from splat .s
    r1 = subprocess.run([
        "mipsel-linux-gnu-as", "-march=r5900",
        "-I", f"build/overlays/{area}",
        "config/asm_prelude.inc", macro_rel, asm_rel,
        "-o", expected_rel,
    ], cwd=ROOT, capture_output=True, text=True)
    if r1.returncode != 0:
        return None

    # Compile candidate .c with mwccmips via qemu+wibo
    r2 = subprocess.run([
        "qemu-i386", "tools/bin/wibo32", "tools/mwccps2/mwccmips.exe",
        "-c", "-O4,p", "-sdatathreshold", "4",
        "-o", obj_rel, src_rel,
    ], cwd=ROOT, capture_output=True, text=True)
    if r2.returncode != 0 or not (ROOT / obj_rel).exists():
        return None

    exp = _extract_text_with_relocs(ROOT / expected_rel)
    got = _extract_text_with_relocs(ROOT / obj_rel)
    if exp is None or got is None:
        return None
    exp_bytes, exp_relocs = exp
    got_bytes, got_relocs = got

    # mwcc-compiled .text size = exactly our asm void body in instructions × 4.
    # Expected GNU-as .text size may include trailing alignment padding past the
    # function body. Compare the overlapping prefix (= mwcc's size).
    n = len(got_bytes)
    if n == 0 or len(exp_bytes) < n:
        return "0.0"
    if exp_bytes[:n] != got_bytes[:n]:
        return "0.0"
    # Compare relocations whose offset falls within the compared range.
    norm = lambda rs: sorted((o, s) for (o, _t, s) in rs if o < n)
    if norm(exp_relocs) != norm(got_relocs):
        return "99.0"
    return "100.0"


def _clean_stale_obj(area: str) -> None:
    """Remove obj/*.o files whose corresponding src/overlays/AREA/*.c no longer
    exists. fill_overlay.py prefers obj/*.o over splat-assembled fallbacks, so
    a stale .o from a deleted candidate would silently keep using mwcc output
    that no longer reflects what's in src/."""
    obj_dir = ROOT / "build" / "overlays" / area / "obj"
    src_dir = ROOT / "src" / "overlays" / area
    if not obj_dir.exists():
        return
    for o in obj_dir.glob("*.o"):
        if not (src_dir / f"{o.stem}.c").exists():
            o.unlink()


def process_area(area: str) -> tuple[int, int]:
    src_dir = ROOT / "src" / "overlays" / area
    src_dir.mkdir(parents=True, exist_ok=True)
    _clean_stale_obj(area)
    candidates = scan_overlay(area)
    print(f"[{area}] candidates: {len(candidates)}")
    matched = []
    dropped = []
    for i, (name, insns, _f) in enumerate(candidates):
        c_path = src_dir / f"{name}.c"
        c_path.write_text(generate_c(name, insns))
        pct = build_candidate(area, name)
        if pct == "100.0":
            matched.append(name)
        else:
            c_path.unlink(missing_ok=True)
            # If a previous run cached a compiled .o for this name in obj/,
            # remove it now so fill_overlay falls back to the splat .s. Without
            # this, dropping a candidate could leave the old .o in place and
            # silently break the overlay link.
            stale_obj = ROOT / "build" / "overlays" / area / "obj" / f"{name}.o"
            stale_obj.unlink(missing_ok=True)
            dropped.append((name, pct or "build-fail"))
        if (i + 1) % 25 == 0:
            print(f"  [{area}] {i+1}/{len(candidates)}  matched={len(matched)} dropped={len(dropped)}")
    print(f"[{area}] matched={len(matched)} dropped={len(dropped)}")
    return len(matched), len(dropped)


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--area")
    g.add_argument("--all", action="store_true")
    args = ap.parse_args()
    areas = ALL_OVERLAYS if args.all else [args.area.upper()]
    totals = {}
    for a in areas:
        m, d = process_area(a)
        totals[a] = (m, d)
    print("\n=== Summary ===")
    grand = 0
    for a, (m, d) in totals.items():
        print(f"  {a}: +{m} matched, {d} dropped")
        grand += m
    print(f"  TOTAL new matches: {grand}")


if __name__ == "__main__":
    main()
