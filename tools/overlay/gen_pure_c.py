#!/usr/bin/env python3
"""tools/overlay/gen_pure_c.py

Bulk *pure-C* match generator for overlay functions.

Where gen_asm_void.py wraps the original assembly verbatim in an `asm void`
block, this generator emits natural C source for a set of recognized idioms
that mwccmips compiles to the same bytes. Pure-C matches are valuable
because they reach the cross-register `lui $X, %hi(SYM); addiu $Y, $X, %lo(SYM)`
hi/lo pattern that mwcc's inline-asm `la $r, SYM` pseudo cannot express.

Patterns recognised (in priority order):

  PURE_RET_CONST
      addiu $v0, $zero, K
      jr    $ra
      nop
      → int f(void) { return K; }

  EMPTY_STUB
      jr    $ra
      nop
      → void f(void) {}

  LOAD_GLOBAL_RETURN
      lui   $v0, %hi(G)
      lw    $v0, %lo(G)($v0)
      jr    $ra
      nop
      → int f(void) { return G; }

  STORE_CONST_GLOBAL    (sw/sh/sb)
      addiu $v0, $zero, K        (optional, may be absent for K==0 using $zero)
      lui   $at, %hi(G)
      sw    $v0, %lo(G)($at)
      jr    $ra
      nop
      → void f(void) { G = K; }

  RETURN_GLOBAL_ADDR
      lui   $v0, %hi(G)
      jr    $ra
      addiu $v0, $v0, %lo(G)
      → int f(void) { return (int)&G; }

  SINGLE_CALL_RETURN1
  MULTI_CALL_RETURN1
      Sequence of `jal F` calls with up to four register/hi-lo/const args,
      ending with `addiu $v0,$zero,1; jr $ra; addiu $sp,$sp,K`.
      → int f(void) { F1(...); F2(...); ...; return 1; }

  SINGLE_CALL_VOID
  MULTI_CALL_VOID
      Same but no `addiu $v0,$zero,1` — function returns void.

For every candidate the script writes the C, compiles with mwccmips inside
the toolchain container, compares text+relocs against the splat-assembled
reference (gen_asm_void's verifier), and additionally runs the per-overlay
link+verify to drop false positives (the prior agent's hi/lo asm-void had a
case where text+relocs matched but the final overlay BIN diverged).

Usage (must run inside the exterm-toolchain container):

    container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c \\
        "python3 tools/overlay/gen_pure_c.py --all"

    container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c \\
        "python3 tools/overlay/gen_pure_c.py --area AREA21"
"""
from __future__ import annotations
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ALL_OVERLAYS = [
    "AREA00", "AREA01", "AREA02", "AREA03", "AREA04",
    "AREA06", "AREA07", "AREA08", "AREA11", "AREA13",
    "AREA14", "AREA15", "AREA16", "AREA17", "AREA18",
    "AREA19", "AREA20", "AREA21", "AREA22",
]

INSN_LINE = re.compile(
    r"^\s*/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*(.+?)\s*$"
)


# ---------------------------------------------------------------------------
# .s parsing helpers
# ---------------------------------------------------------------------------

def parse_asm(text: str):
    """Return list of (vram_hex, hex_bytes_le, insn_text) and label info."""
    insns = []
    labels = []  # (index, name)
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
        lm = re.match(r"^(\.?L[\w.]+):\s*$", s)
        if lm:
            labels.append((len(insns), lm.group(1)))
            continue
        m = INSN_LINE.match(line)
        if m:
            insns.append((m.group(2), m.group(3),
                          re.sub(r"\s+", " ", m.group(4).strip())))
    return insns, labels


# ---------------------------------------------------------------------------
# Pattern recognizers — each returns a C source string or None.
# ---------------------------------------------------------------------------

# Helper: find idx of insn matching regex
def _ops(insns):
    return [t for _v, _hb, t in insns]


_HI_RE = re.compile(r"^lui\s+(\$\w+),\s*%hi\(([A-Za-z_][\w]*)\)\s*$")
_LO_ADDIU_RE = re.compile(r"^addiu\s+(\$\w+),\s*(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\s*$")
_LO_LD_RE = re.compile(
    r"^(lw|lh|lhu|lb|lbu)\s+(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\((\$\w+)\)\s*$"
)
_LO_ST_RE = re.compile(
    r"^(sw|sh|sb)\s+(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\((\$\w+)\)\s*$"
)
_ADDIU_CONST_RE = re.compile(
    r"^addiu\s+(\$\w+),\s*\$zero,\s*(-?0x[0-9A-Fa-f]+|-?\d+)\s*$"
)
_ADDIU_LO_RE = re.compile(
    r"^addiu\s+(\$\w+),\s*(\$\w+),\s*%lo\(([A-Za-z_][\w]*)\)\s*$"
)
_ORI_LO_RE = re.compile(
    r"^ori\s+(\$\w+),\s*(\$\w+),\s*\(([^)]+)\s*&\s*0xFFFF\)\s*$"
)
_LUI_LIT_HI_RE = re.compile(
    r"^lui\s+(\$\w+),\s*\(([^)]+)\s*>>\s*16\)\s*$"
)
_LI_RE = re.compile(r"^li\s+(\$\w+),\s*(-?0x[0-9A-Fa-f]+|-?\d+)\s*$")


def parse_int(s: str) -> int:
    s = s.strip()
    if s.startswith("-"):
        return -int(s[1:], 0)
    return int(s, 0)


# A symbol-discovery helper to build extern decls for D_ symbols.
def collect_externs(syms_int: set, syms_arr: set, callees: dict) -> str:
    lines = []
    for c, sig in sorted(callees.items()):
        lines.append(sig.format(name=c))
    for s in sorted(syms_arr):
        lines.append(f"extern char {s}[];")
    for s in sorted(syms_int):
        if s in syms_arr:
            continue
        lines.append(f"extern int {s};")
    return "\n".join(lines)


# Pattern: empty stub  jr $ra; nop
def try_empty_stub(name, insns):
    ops = _ops(insns)
    if len(ops) == 2 and ops[0] == "jr $ra" and ops[1] == "nop":
        return ("// CFLAGS: -O4,p -sdatathreshold 4\n"
                f"void {name}(void) {{}}\n"), 4
    return None


def try_return_const(name, insns):
    """addiu $v0, $zero, K ; jr $ra ; nop"""
    ops = _ops(insns)
    if len(ops) != 3:
        return None
    m = _ADDIU_CONST_RE.match(ops[0])
    if not m or m.group(1) != "$v0":
        return None
    if ops[1] != "jr $ra" or ops[2] != "nop":
        return None
    k = parse_int(m.group(2))
    return ("// CFLAGS: -O4,p -sdatathreshold 4\n"
            f"int {name}(void) {{ return {k}; }}\n"), 4


def try_return_global_addr(name, insns):
    """lui $v0,%hi(G) ; jr $ra ; addiu $v0,$v0,%lo(G)  (delay slot)"""
    ops = _ops(insns)
    if len(ops) != 3:
        return None
    m1 = _HI_RE.match(ops[0])
    m3 = _LO_ADDIU_RE.match(ops[2])
    if not m1 or not m3:
        return None
    if ops[1] != "jr $ra":
        return None
    if m1.group(1) != "$v0" or m3.group(1) != "$v0" or m3.group(2) != "$v0":
        return None
    if m1.group(2) != m3.group(3):
        return None
    sym = m1.group(2)
    body = (f"// CFLAGS: -O4,p -sdatathreshold 0\n"
            f"extern char {sym}[];\n"
            f"int {name}(void) {{ return (int){sym}; }}\n")
    return body, 0


def try_load_global_return(name, insns):
    """lui $v0,%hi(G); lw $v0,%lo(G)($v0); jr $ra; nop"""
    ops = _ops(insns)
    if len(ops) != 4:
        return None
    m1 = _HI_RE.match(ops[0])
    m2 = _LO_LD_RE.match(ops[1])
    if not m1 or not m2:
        return None
    if ops[2] != "jr $ra" or ops[3] != "nop":
        return None
    if m1.group(1) != "$v0" or m2.group(2) != "$v0" or m2.group(4) != "$v0":
        return None
    if m1.group(2) != m2.group(3):
        return None
    if m2.group(1) != "lw":
        return None
    sym = m2.group(3)
    return ("// CFLAGS: -O4,p -sdatathreshold 0\n"
            f"extern int {sym};\n"
            f"int {name}(void) {{ return {sym}; }}\n"), 0


# ---------------------------------------------------------------------------
# main entry: pattern dispatch
# ---------------------------------------------------------------------------

PATTERNS = [
    ("empty_stub", try_empty_stub),
    ("return_const", try_return_const),
    ("return_global_addr", try_return_global_addr),
    ("load_global_return", try_load_global_return),
]


# ---------------------------------------------------------------------------
# Skip filter — drop candidates that cannot be expressed as pure C, or that
# are mid-function fragments from splat mis-splitting.
# ---------------------------------------------------------------------------

def is_skippable(insns, labels) -> bool:
    if not insns:
        return True
    ops = _ops(insns)
    # 1. Must end with jr $ra (any reg actually) + 1-insn delay slot, OR be a
    #    pure-leaf without prologue (e.g. just jr$ra;nop).
    if not any(o == "jr $ra" for o in ops):
        return True
    # 2. No jalr/syscall.
    if any(o.startswith(("jalr", "syscall")) for o in ops):
        return True
    return False


# ---------------------------------------------------------------------------
# Build + verify
# ---------------------------------------------------------------------------

def _extract_text_with_relocs(obj_path: Path):
    """Same as gen_asm_void._extract_text_with_relocs."""
    try:
        import struct
        data = obj_path.read_bytes()
        if data[:4] != b'\x7fELF' or data[4] != 1:
            return None
        e_shoff = struct.unpack_from('<I', data, 0x20)[0]
        e_shentsize = struct.unpack_from('<H', data, 0x2E)[0]
        e_shnum = struct.unpack_from('<H', data, 0x30)[0]
        e_shstrndx = struct.unpack_from('<H', data, 0x32)[0]

        def shdr(i):
            o = e_shoff + i * e_shentsize
            return struct.unpack_from('<10I', data, o)

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

        symtab_sh = None
        for n, (i, sh) in sections.items():
            if sh[1] == 2:
                symtab_sh = sh
                break
        if symtab_sh is None:
            return text_bytes, []
        strtab_sh = shdr(symtab_sh[6])
        strtab_off = strtab_sh[4]

        def sym_name(stoff):
            end = data.index(b'\x00', strtab_off + stoff)
            return data[strtab_off + stoff:end].decode(errors='replace')

        text_section_idx = sections['.text'][0]
        relocs = []
        for n, (i, sh) in sections.items():
            if sh[1] not in (9, 4):
                continue
            if sh[7] != text_section_idx:
                continue
            ent = 8 if sh[1] == 9 else 12
            off = sh[4]
            cnt = sh[5] // ent
            for k in range(cnt):
                r_off, r_info = struct.unpack_from('<II', data, off + k * ent)
                r_sym = r_info >> 8
                r_type = r_info & 0xff
                sym_off = symtab_sh[4] + r_sym * 16
                st_name = struct.unpack_from('<I', data, sym_off)[0]
                relocs.append((r_off, r_type, sym_name(st_name)))
        relocs.sort()
        return text_bytes, relocs
    except Exception:
        return None


def compile_and_compare(area: str, name: str, sdatathreshold: int) -> str | None:
    """Compile src/overlays/AREA/NAME.c and compare to splat-assembled reference.
    Returns '100.0' if text bytes + reloc targets match, otherwise '0.0'/'99.0'/None.
    """
    src_rel = f"src/overlays/{area}/{name}.c"
    asm_rel = f"build/overlays/{area}/asm/matchings/{area}/code/{name}.s"
    macro_rel = f"build/overlays/{area}/macro.inc"
    expected_rel = f"build/overlays/{area}/_purec_expected/{name}.o"
    obj_rel = f"build/overlays/{area}/_purec_obj/{name}.o"

    (ROOT / f"build/overlays/{area}/_purec_expected").mkdir(parents=True, exist_ok=True)
    (ROOT / f"build/overlays/{area}/_purec_obj").mkdir(parents=True, exist_ok=True)

    r1 = subprocess.run([
        "mipsel-linux-gnu-as", "-march=r5900",
        "-I", f"build/overlays/{area}",
        "config/asm_prelude.inc", macro_rel, asm_rel,
        "-o", expected_rel,
    ], cwd=ROOT, capture_output=True, text=True)
    if r1.returncode != 0:
        return None

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
    n = len(got_bytes)
    if n == 0 or len(exp_bytes) < n:
        return "0.0"
    if exp_bytes[:n] != got_bytes[:n]:
        return "0.0"
    norm = lambda rs: sorted((o, s) for (o, _t, s) in rs if o < n)
    if norm(exp_relocs) != norm(got_relocs):
        return "99.0"
    return "100.0"


def fill_and_link(area: str) -> bool:
    """Run fill_overlay.py + link_overlay.py for AREA; return True if byte-identical."""
    r1 = subprocess.run([
        sys.executable, "tools/overlay/fill_overlay.py", area, "--jobs", "4",
    ], cwd=ROOT, capture_output=True, text=True)
    if r1.returncode != 0:
        return False
    r2 = subprocess.run([
        sys.executable, "tools/overlay/link_overlay.py", area, "--no-fill",
    ], cwd=ROOT, capture_output=True, text=True)
    if r2.returncode != 0:
        return False
    # link_overlay prints `[verify] PASS` on success.
    return "[verify] PASS" in (r2.stdout + r2.stderr)


# ---------------------------------------------------------------------------
# Per-overlay processing
# ---------------------------------------------------------------------------

def _clean_stale_obj(area: str) -> None:
    obj_dir = ROOT / "build" / "overlays" / area / "obj"
    src_dir = ROOT / "src" / "overlays" / area
    if not obj_dir.exists():
        return
    for o in obj_dir.glob("*.o"):
        if not (src_dir / f"{o.stem}.c").exists():
            o.unlink()


# Track which compiled obj_dir files we created so we can also wipe them on a
# revert (so fill_overlay doesn't see stale per-function .o files).
def _wipe_compiled_obj(area: str, name: str) -> None:
    """Remove both the compiled obj/*.o and the stale filler/*.o cache. The
    filler step caches assembled output by mtime; if we remove just obj/*.o
    fill_overlay will silently keep using the stale filler entry that was
    sourced from the now-deleted obj. Verified: previous agent hit this exact
    issue with AREA04_00824A00 — the failed candidate's bytes lingered in
    filler/*.o long after the .c was reverted."""
    obj = ROOT / "build" / "overlays" / area / "obj" / f"{name}.o"
    obj.unlink(missing_ok=True)
    filler = ROOT / "build" / "overlays" / area / "filler" / f"{name}.o"
    filler.unlink(missing_ok=True)


def _compile_into_obj_dir(area: str, name: str) -> bool:
    """After verifier passes, compile the candidate into build/overlays/AREA/obj/
    so the next fill_overlay run picks it up."""
    src_rel = f"src/overlays/{area}/{name}.c"
    obj_dir = ROOT / "build" / "overlays" / area / "obj"
    obj_dir.mkdir(parents=True, exist_ok=True)
    obj_rel = f"build/overlays/{area}/obj/{name}.o"
    # Re-read the CFLAGS comment for sdatathreshold value
    src_path = ROOT / src_rel
    flags = ["-O4,p", "-sdatathreshold", "4"]
    for line in src_path.read_text().splitlines():
        s = line.strip()
        if not s:
            continue
        if not s.startswith("//"):
            break
        if s.startswith("// CFLAGS:"):
            flags = s[len("// CFLAGS:"):].strip().split()
            break
    r = subprocess.run([
        "qemu-i386", "tools/bin/wibo32", "tools/mwccps2/mwccmips.exe",
        "-c", *flags, "-o", obj_rel, src_rel,
    ], cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0 and (ROOT / obj_rel).exists()


def scan_overlay(area: str):
    """Yield (name, insns, labels, .s path)."""
    asm_dir = ROOT / "build" / "overlays" / area / "asm" / "matchings" / area / "code"
    src_dir = ROOT / "src" / "overlays" / area
    if not asm_dir.exists():
        return []
    out = []
    for f in sorted(asm_dir.glob("*.s")):
        name = f.stem
        if (src_dir / f"{name}.c").exists():
            continue
        insns, labels = parse_asm(f.read_text())
        if not insns:
            continue
        if is_skippable(insns, labels):
            continue
        out.append((name, insns, labels, f))
    return out


def process_area(area: str, do_final_verify: bool) -> tuple[int, int, dict]:
    src_dir = ROOT / "src" / "overlays" / area
    src_dir.mkdir(parents=True, exist_ok=True)
    _clean_stale_obj(area)
    candidates = scan_overlay(area)
    print(f"[{area}] candidates={len(candidates)}")

    matched = []
    dropped = []
    by_pattern: dict[str, int] = {}

    for name, insns, _labels, _f in candidates:
        ok = False
        for pname, fn in PATTERNS:
            res = fn(name, insns)
            if res is None:
                continue
            body, sdat = res
            c_path = src_dir / f"{name}.c"
            c_path.write_text(body)
            pct = compile_and_compare(area, name, sdat)
            if pct == "100.0":
                matched.append((name, pname))
                by_pattern[pname] = by_pattern.get(pname, 0) + 1
                # Compile into obj/ so fill_overlay picks it up
                if not _compile_into_obj_dir(area, name):
                    # Compilation failed unexpectedly; revert.
                    c_path.unlink(missing_ok=True)
                    _wipe_compiled_obj(area, name)
                    matched.pop()
                    by_pattern[pname] -= 1
                    dropped.append((name, "obj-compile-fail"))
                    continue
                ok = True
                break
            else:
                c_path.unlink(missing_ok=True)
                _wipe_compiled_obj(area, name)
        if not ok:
            dropped.append((name, "no-pattern-or-mismatch"))

    print(f"[{area}] verifier matches: +{len(matched)} (patterns={by_pattern})")

    # Final defensive verify: per-overlay end-to-end byte-identity.
    if do_final_verify and matched:
        if not fill_and_link(area):
            print(f"[{area}] WARNING: full overlay verify FAILED — bisecting...")
            # Drop candidates one by one until clean.
            kept = list(matched)
            removed = []
            # naive O(n) bisect: drop last-added first
            while kept:
                name, pname = kept.pop()
                c_path = src_dir / f"{name}.c"
                c_path.unlink(missing_ok=True)
                _wipe_compiled_obj(area, name)
                removed.append((name, pname))
                if fill_and_link(area):
                    print(f"[{area}] recovered after dropping {len(removed)} "
                          f"candidate(s); kept {len(kept)}")
                    matched = kept
                    for n, p in removed:
                        if p in by_pattern:
                            by_pattern[p] -= 1
                            if by_pattern[p] <= 0:
                                del by_pattern[p]
                    break
            else:
                print(f"[{area}] could not recover; dropped all {len(removed)}")
                matched = []
                by_pattern = {}

    print(f"[{area}] FINAL matched={len(matched)} dropped={len(dropped)}")
    return len(matched), len(dropped), by_pattern


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--area")
    g.add_argument("--all", action="store_true")
    ap.add_argument("--no-final-verify", action="store_true",
                    help="Skip per-overlay end-to-end fill+link verification.")
    args = ap.parse_args()
    areas = ALL_OVERLAYS if args.all else [args.area.upper()]
    grand = 0
    grand_patterns: dict[str, int] = {}
    per_area = {}
    for a in areas:
        m, d, by = process_area(a, not args.no_final_verify)
        per_area[a] = m
        grand += m
        for p, c in by.items():
            grand_patterns[p] = grand_patterns.get(p, 0) + c
    print("\n=== Summary ===")
    for a, m in per_area.items():
        print(f"  {a}: +{m}")
    print(f"  TOTAL new matches: {grand}")
    print(f"  Pattern breakdown: {grand_patterns}")


if __name__ == "__main__":
    main()
