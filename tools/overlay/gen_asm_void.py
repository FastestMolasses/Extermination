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

HDR_HILO = ("// CFLAGS: -O4,p -sdatathreshold 0\n"
            "// Hybrid asm void: real mnemonics where mwcc accepts them,\n"
            "// .word for branch instructions, `la $r, SYM' for %hi/%lo pairs.\n")

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
        s = re.sub(re.escape(k) + r"\b", v, s)
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


_HI_RE = re.compile(r"^lui\s+(\$\w+),\s*%hi\(([A-Za-z_][\w]*)\)\s*$")
_LO_ADDIU_RE = re.compile(r"^addiu\s+(\$\w+),\s*(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\s*$")
_LO_LDST_RE = re.compile(
    r"^(lw|sw|lh|sh|lhu|lb|sb|lbu|lwc1|swc1|ld|sd|lq|sq)\s+"
    r"(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\((\$\w+)\)\s*$"
)
_ANY_HILO_RE = re.compile(r"%hi|%lo|%gp_rel")
_SYM_RE = re.compile(r"\b(D_[0-9A-Fa-f]{6,8}|D_overlay_AREA\d\d_[0-9A-Fa-f]{8}|jtbl_[\w]+)\b")


def hilo_transform(insns):
    """Return (out_lines, sym_names, ok). out_lines is list of asm body lines
    (each properly indented). sym_names is the set of D_/jtbl_ symbols that need
    extern decls. ok is False if any %hi/%lo couldn't be paired into a clean
    `la $r, SYM` form (different scratch reg, split load, etc.)."""
    out: list[str] = []
    syms: set[str] = set()
    i = 0
    while i < len(insns):
        hb, text = insns[i]
        m_hi = _HI_RE.match(text)
        if m_hi:
            hi_reg, hi_sym = m_hi.group(1), m_hi.group(2)
            # Look at next insn
            if i + 1 < len(insns):
                hb2, text2 = insns[i + 1]
                m_addiu = _LO_ADDIU_RE.match(text2)
                if (m_addiu and m_addiu.group(1) == hi_reg and
                        m_addiu.group(2) == hi_reg and m_addiu.group(3) == hi_sym):
                    out.append(f"    la {rr(hi_reg)}, {hi_sym}")
                    syms.add(hi_sym)
                    i += 2
                    continue
                # lui+load(%lo) only safe when scratch == $at AND dst != scratch...
                # actually mwcc's `lw $X, SYM` form forces $at, so we can match only
                # patterns where the original used $at as scratch.
                m_ld = _LO_LDST_RE.match(text2)
                if (m_ld and hi_reg == "$at" and m_ld.group(4) == "$at"
                        and m_ld.group(3) == hi_sym):
                    op = m_ld.group(1)
                    dst = rr(m_ld.group(2))
                    out.append(f"    {op} {dst}, {hi_sym}")
                    syms.add(hi_sym)
                    i += 2
                    continue
            return [], set(), False
        # Standalone %hi/%lo (not paired into recognized form)
        if _ANY_HILO_RE.search(text):
            return [], set(), False
        toks = text.split()
        op = toks[0] if toks else ""
        if op in BRANCH_OPS:
            out.append(f"    {word(hb)}")
        elif op == "j" and len(toks) > 1:
            out.append(f"    {word(hb)}")
        else:
            out.append(f"    {rr(text)}")
        i += 1
    return out, syms, True


def has_hilo(insns) -> bool:
    return any(_ANY_HILO_RE.search(t) for _h, t in insns)


def should_skip_hilo(insns) -> bool:
    """Skip criteria for the hi/lo-enabled pass: same as should_skip but
    allowing %hi/%lo. Still rejects jalr/syscall/j-to-sym/%gp_rel."""
    for _hb, text in insns:
        toks = text.split()
        if not toks:
            continue
        op = toks[0]
        if "%gp_rel" in text:
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


def generate_c_hilo(name: str, insns) -> str | None:
    """Generate hybrid asm-void body that handles `lui+addiu %hi/%lo` pairs
    via the mwcc `la $r, SYM` pseudo. Returns None if not all %hi/%lo can be
    cleanly collapsed."""
    body, syms, ok = hilo_transform(insns)
    if not ok:
        return None
    callees = []
    for _hb, text in insns:
        if text.startswith("jal "):
            callee = text.split()[1]
            callees.append(callee)
    decl_lines = []
    for c in sorted(set(callees)):
        decl_lines.append(f"extern void {c}(int, int, int, int);")
    for s in sorted(syms):
        decl_lines.append(f"extern int {s};")
    decls = "\n".join(decl_lines)
    prefix = HDR_HILO + (decls + "\n\n" if decls else "\n")
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
    out_hilo = []
    for f in all_files:
        name = f.stem
        if (src_dir / f"{name}.c").exists():
            continue
        insns, _labels = parse_asm(f.read_text())
        if not insns:
            continue
        if not (3 <= len(insns) <= 300):
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

        if not should_skip(insns):
            out.append((name, insns, f))
        elif has_hilo(insns) and not should_skip_hilo(insns):
            out_hilo.append((name, insns, f))
    return out, out_hilo


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


def build_candidate(area: str, name: str, sdatathreshold: int = 4) -> str | None:
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
        "-c", "-O4,p", "-sdatathreshold", str(sdatathreshold),
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
    candidates, hilo_candidates = scan_overlay(area)
    print(f"[{area}] candidates: no-hilo={len(candidates)} hilo={len(hilo_candidates)}")
    matched = []
    dropped = []
    # First pass: plain asm-void (no hi/lo).
    for i, (name, insns, _f) in enumerate(candidates):
        c_path = src_dir / f"{name}.c"
        c_path.write_text(generate_c(name, insns))
        pct = build_candidate(area, name, sdatathreshold=4)
        if pct == "100.0":
            matched.append(name)
        else:
            c_path.unlink(missing_ok=True)
            stale_obj = ROOT / "build" / "overlays" / area / "obj" / f"{name}.o"
            stale_obj.unlink(missing_ok=True)
            stale_filler = ROOT / "build" / "overlays" / area / "filler" / f"{name}.o"
            stale_filler.unlink(missing_ok=True)
            dropped.append((name, pct or "build-fail"))
        if (i + 1) % 25 == 0:
            print(f"  [{area}] no-hilo {i+1}/{len(candidates)}  matched={len(matched)} dropped={len(dropped)}")
    # Second pass: hi/lo-aware (lui+addiu → la $r,SYM).
    hilo_matched = 0
    hilo_dropped = 0
    for i, (name, insns, _f) in enumerate(hilo_candidates):
        c_path = src_dir / f"{name}.c"
        body = generate_c_hilo(name, insns)
        if body is None:
            hilo_dropped += 1
            continue
        c_path.write_text(body)
        pct = build_candidate(area, name, sdatathreshold=0)
        if pct == "100.0":
            matched.append(name)
            hilo_matched += 1
        else:
            c_path.unlink(missing_ok=True)
            stale_obj = ROOT / "build" / "overlays" / area / "obj" / f"{name}.o"
            stale_obj.unlink(missing_ok=True)
            stale_filler = ROOT / "build" / "overlays" / area / "filler" / f"{name}.o"
            stale_filler.unlink(missing_ok=True)
            dropped.append((name, pct or "build-fail"))
            hilo_dropped += 1
        if (i + 1) % 25 == 0:
            print(f"  [{area}] hilo {i+1}/{len(hilo_candidates)}  matched={hilo_matched} dropped={hilo_dropped}")
    print(f"[{area}] matched={len(matched)} (hilo +{hilo_matched}) dropped={len(dropped)}")
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
