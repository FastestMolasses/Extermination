#!/usr/bin/env python3
"""Build driver for the Extermination (SCUS-97112) matching decompilation.

Commands:
  setup                 run splat over the boot ELF + write objdiff.json   [macOS]
  expected              assemble splat disassembly -> build/expected/*.o    [container]
  compile               compile src/*.c -> build/obj/*.o                    [container]
  build                 expected + compile                                 [container]
  objdiff               (re)write objdiff.json from src/                    [macOS]
  --single-file PATH    compile one object (objdiff custom-make hook)        [container]

The period-correct compiler (Metrowerks mwccmips, a 32-bit Windows PE) cannot
run under Rosetta on Apple Silicon, so the toolchain steps run inside the
`exterm-toolchain` container (Apple `container` CLI): a 32-bit wibo executes
mwccmips under qemu-i386 emulation, while the MIPS assembler is arm64-native.
See docker/Dockerfile. Run setup with the project venv's Python (for splat):
    .venv/bin/python tools/decomp/build.py setup
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config"
SRC = ROOT / "src"
BUILD = ROOT / "build"

SPLAT_YAML = CONFIG / "SCUS_971.12.yaml"
OBJDIFF_JSON = ROOT / "objdiff.json"

IMAGE = "exterm-permuter"  # superset of exterm-toolchain: + i386 libs (ee-gcc) + permuter deps
# splat writes each auto-detected function's disassembly here.
ASM_DIR = "build/asm/matchings/main/code"
# mwccmips flags. -O4,p (speed) matches the trivial functions decompiled so
# far; expect to tune per translation unit as matching proceeds.
CFLAGS = "-O4,p"


def is_asm_stub(path: Path) -> bool:
    """True if src/<name>.c's bytes come from the splat .s, not from compiling it.

    Two kinds qualify, both marked by a first-line comment:
      • `// INCLUDE_ASM` — undecompiled placeholder (no readable C body).
      • `// NEARMISS`    — readable decompilation that is NOT byte-identical (a
        genuine compiler artifact: register coloring / scheduling / branch-likely
        / etc. that no source change can fix). The readable C body IS present as
        faithful ground truth for the port, but the linker fills the function from
        the splat .s (byte-identical), and it is NOT compiled or counted as an
        objdiff unit (so matched_code measures only true byte-match attempts).
        Each NEARMISS file documents its objdiff % and the precise divergence; the
        global registry is docs/NEARMISS.md.
    Both are skipped by compile/expected/objdiff; the linker assembles their .s.
    """
    try:
        with path.open() as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                return line.startswith("// INCLUDE_ASM") or line.startswith("// NEARMISS")
    except OSError:
        pass
    return False


def units() -> list[str]:
    """Function names with real C source in src/ AND a splat .s in build/asm/.

    Skips (a) orphan src files whose corresponding splat function has been
    renamed (no .s with that name exists), and (b) INCLUDE_ASM stubs (see
    is_asm_stub) — stubs are undecompiled placeholders the linker fulfils from
    the local .s, so they must not be compiled or treated as objdiff units.
    """
    asm_dir = ROOT / ASM_DIR
    available = {p.stem for p in asm_dir.glob("*.s")} if asm_dir.exists() else None
    out = []
    for p in SRC.glob("*.c"):
        if available is not None and p.stem not in available:
            continue
        if is_asm_stub(p):
            continue
        out.append(p.stem)
    return sorted(out)


def container(script: str) -> None:
    """Run a shell script inside the toolchain container, repo bind-mounted."""
    subprocess.run(
        ["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
         IMAGE, "sh", "-c", script],
        check=True,
    )


def assemble_cmd(name: str) -> str:
    """Assemble splat's disassembly of `name` into an objdiff target object.

    `-march=r5900` sets the EE flag in the resulting ELF header — objdiff uses
    that flag to pick its disassembler (5900 vs generic mips1), and without it
    coprocessor-2/VU instructions show as `ldc2`/`sdc2` instead of `lqc2`/`sqc2`,
    confusing the per-instruction comparison even when the bytes match.

    Normalization itself runs host-side in normalize_asm(); this only assembles
    the already-normalized file, which keeps the batched container command short.
    """
    return (f"mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc "
            f"build/macro.inc build/.asmnorm/{name}.s -o build/expected/{name}.o")


# splat emits the VU0 accumulator and divide registers bare (ACC / Q); binutils
# accepts only $ACC / $Q, so the VU0 units otherwise fail to assemble — and since
# `as` deletes its output on error, a full build silently destroys their
# build/expected/*.o and the match gate then skips with "expected stale". The
# D_FFFFF / D_20000xxx entries undo splat symbolizations that invented a symbol
# for what the real code encodes as a plain immediate (0xFFFFF is below the
# 0x100000 load address; the D_2000xxxx values are uncached-mirror offsets), which
# left a relocation in the target that no C source can reproduce. Every pattern is
# anchored to a `v<op>` mnemonic or a full `%hi(...)`/`%lo(...)` form so it cannot
# touch the many labels and constants that merely contain the same characters
# (.L0015ACC0, 0xC26ACCCD, func_001CACC0, D_0026ACC0).
_ASM_FIXUPS = [
    (re.compile(r'(v[a-z]+\.[a-z]+[ \t]+)ACC,'), r'\1$ACC,'),
    (re.compile(r'(v[a-z]+[ \t]+)Q,'), r'\1$Q,'),
    (re.compile(r',[ \t]*Q[ \t]*$', re.M), r', $Q'),
    (re.compile(r'%hi\(D_FFFFFF\)'), '0x100'),
    (re.compile(r'%lo\(D_FFFFFF\)'), '-1'),
    (re.compile(r'%hi\(D_FFFFF\)'), '0x10'),
    (re.compile(r'%lo\(D_FFFFF\)'), '-1'),
    (re.compile(r'%lo\(D_20000010\)'), '0x10'),
    (re.compile(r'%lo\(D_20000020\)'), '0x20'),
    (re.compile(r'%lo\(D_20000050\)'), '0x50'),
    (re.compile(r'%lo\(D_20000400\)'), '0x400'),
    (re.compile(r'%lo\(D_20000800\)'), '0x800'),
]



# splat prefixes every instruction with a `/* vram encoding */` comment, so the
# mnemonic is NOT at line start — anchoring on ^ silently matches nothing.
_HOIST_LUI = re.compile(r'^(.*?\blui\s+)\$(\w+),(\s*)\((0x[0-9A-Fa-f]+)\s*>>\s*16\)\s*$', re.M)
_HOIST_LO_ADD = re.compile(r'\b(?:addiu|addu|ori)\s+\$\w+,\s*\$(\w+),\s*%lo\((D_[0-9A-Fa-f]+)\)')
_HOIST_LO_MEM = re.compile(r'%lo\((D_[0-9A-Fa-f]+)\)\(\$(\w+)\)')


def _symbolize_hoisted_hi(src: str) -> str:
    """Re-symbolize a hoisted `lui $r, (0xNNNNNN >> 16)` back to `%hi(D_...)`.

    WHY (s86). splat pairs a `%hi`/`%lo` couple by proximity. When the compiler
    HOISTS the `%hi` far from its `%lo` — out of a loop, or above a branch — splat
    gives up and writes the raw immediate instead of the symbol. The expected
    object then holds a bare constant where our compiled object correctly holds an
    R_MIPS_HI16 relocation. Both assemble to the SAME word, so the linked binary is
    byte-identical either way, but objdiff compares relocations and scores the
    function a permanent near-miss it can never source-fix: func_0010FC38 sat at
    99.9919% on exactly one such instruction, while the two nearby `%hi`s of the
    same symbol WERE paired correctly.

    This is the playbook's §2a rule — build the expected object the way the
    original object was built. Rewriting the source to emit a bare literal would
    "fix" the one site and break the two that splat got right.

    The rewrite is deliberately conservative: only when a LATER instruction uses
    `%lo(D_xxxxxxxx)` through the SAME register and that symbol's `%hi` equals the
    hoisted immediate. Anything else (hardware register bases like 0x10000000,
    genuine constants) is left alone. Validated against func_0010FC38 before being
    generalized; it fires on 11 sites across 10 functions.
    """
    def hi_of(addr: int) -> int:
        return ((addr >> 16) + (1 if (addr & 0xFFFF) & 0x8000 else 0)) & 0xFFFF

    lines = src.splitlines(keepends=True)
    los: list[tuple[int, str, str]] = []   # (line, reg, symbol)
    for i, l in enumerate(lines):
        m = _HOIST_LO_ADD.search(l)
        if m:
            los.append((i, m.group(1), m.group(2)))
            continue
        m = _HOIST_LO_MEM.search(l)
        if m:
            los.append((i, m.group(2), m.group(1)))

    changed = False
    for i, l in enumerate(lines):
        m = _HOIST_LUI.match(l.rstrip("\n"))
        if not m:
            continue
        reg, imm = m.group(2), int(m.group(4), 16) >> 16
        for lj, lreg, sym in los:
            if lreg != reg or lj <= i:
                continue
            if hi_of(int(sym[2:], 16)) == imm:
                lines[i] = f"{m.group(1)}${reg},{m.group(3)}%hi({sym})\n"
                changed = True
                break
    return "".join(lines) if changed else src


# The two PS2 scratchpad globals that are in no symbol list, so splat renders
# them as address literals. See tools/match/spad_symbolize.py for the full
# argument; in brief, the original build referenced them as ordinary externs, so
# every access carries a %hi/%lo relocation, and handing mwcc a plain constant
# instead lets it CSE the `lui` and speculate it into delay slots the target
# leaves as `nop`.
_SPAD_SYMS = ("0x70003B6C", "0x70003B8D")

# TESTED AND REJECTED (s86) — do not retry without new evidence. splat leaves
# `lui`+`sw` pairs as literals even for addresses it symbolizes in the adjacent
# `lui`+`addiu` pair, so func_001BF6B0 carries both spellings of D_700038A0 and
# shows a 4-row residual there. It looks like the same fix as above, and a wave
# agent predicted symbolizing those pairs would clear it. Generalizing this
# function to symbolize ANY 0x7000xxxx literal whose D_ symbol the C references
# was measured on that exact function: 99.9577 -> 99.7531. It makes things
# WORSE — symbolizing the store sites introduces more mismatches than the four
# it resolves. The residual is also harmless: both forms relocate to the same
# bytes, so the linked ELF is already byte-identical there.


def _symbolize_scratchpad(name: str, src: str) -> str:
    """Rewrite scratchpad literals to %hi/%lo, but ONLY where the C opted in.

    PER-FILE BY DESIGN. 35 already-matched functions still spell these
    addresses as literals and match precisely because BOTH sides do: the
    expected object and the compiled object each carry a bare constant and no
    relocation. Symbolizing every .s unconditionally would give the expected
    objects relocations their C cannot produce and break all 35 at once. So a
    function's target is symbolized only once its own source references the
    symbol, which makes the migration incremental and self-consistent.
    """
    csrc = SRC / f"{name}.c"
    try:
        c = csrc.read_text(errors="ignore")
    except OSError:
        return src
    for lit in _SPAD_SYMS:
        sym = "D_" + lit[2:]
        if not re.search(r'\b' + sym + r'\b', c):
            continue
        src = src.replace(f"({lit} >> 16)", f"%hi({sym})")
        src = src.replace(f"({lit} & 0xFFFF)", f"%lo({sym})")
    return src


def normalize_asm(name: str) -> None:
    """Write build/.asmnorm/<name>.s: splat's disassembly, fixed up and with the
    function's own jump table appended.

    A switch dispatcher's jump table is .rodata of the SAME translation unit as its
    function. Proven s85: across 159 adjacent function->table pairs, table addresses
    are monotonic in function addresses with ZERO inversions — exactly what you get
    when the linker gathers each object's .text and .rodata into separate output
    regions. Assembling a function WITHOUT its table left the jtbl symbol
    undefined-external in the expected object, so its dispatch relocation could never
    match the local table mwcc necessarily emits. That artifact — not any compiler
    limitation — is what was recorded for two sessions as the "jr-table
    external-dispatch wall". Appending the table gives both sides a local one.
    """
    src = (ROOT / ASM_DIR / f"{name}.s").read_text(errors="ignore")
    for pat, rep in _ASM_FIXUPS:
        src = pat.sub(rep, src)
    src = _symbolize_hoisted_hi(src)
    src = _symbolize_scratchpad(name, src)
    jtbl = ROOT / "build" / "jtblrodata" / f"{name}.s"
    if jtbl.exists():
        src += "\n" + jtbl.read_text(errors="ignore")
    out = ROOT / "build" / ".asmnorm" / f"{name}.s"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(src)


def file_cflags(name: str) -> str:
    """Read per-file CFLAGS from a '// CFLAGS: ...' comment in src/<name>.c.

    If a C++ comment starting with '// CFLAGS:' appears anywhere in the file's
    LEADING COMMENT BLOCK, the remainder of that line replaces the default
    CFLAGS. Otherwise the global CFLAGS constant is used.

    Scan the whole leading comment block, exactly as file_compiler() does (s86).
    This used to `break` on the first comment line that was neither '// CFLAGS:'
    nor '// COMPILER:' — which is every NEARMISS file, since those open with a
    '// NEARMISS <func> ...' banner. 628 files silently fell back to the global
    '-O4,p' and lost their declared '-sdatathreshold 0'. It stayed invisible
    because NEARMISS files are not compiled by the canonical build and because
    integrate_nearmiss.py has its own (correct) directive parser, so the two
    disagreed only when a NEARMISS was re-measured through build.py.
    """
    src = SRC / f"{name}.c"
    try:
        with src.open() as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                if line.startswith("// CFLAGS:"):
                    return line[len("// CFLAGS:"):].strip()
                if line.startswith("//"):
                    continue  # keep scanning the leading comment block
                break
    except OSError:
        pass
    return "-O2" if file_compiler(name) == "eegcc" else CFLAGS


def file_compiler(name: str) -> str:
    """Per-file compiler from a '// COMPILER: ...' comment in the leading block.

    Returns 'eegcc' for Sony-SDK/crt0 functions built with the period EE GNU
    compiler (ee-gcc 2.96), or 'mwcc' (default) for the CodeWarrior game code.
    """
    src = SRC / f"{name}.c"
    try:
        with src.open() as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                if line.startswith("// COMPILER:"):
                    return line[len("// COMPILER:"):].strip()
                if line.startswith("//"):
                    continue
                break
    except OSError:
        pass
    return "mwcc"


def compile_cmd(name: str) -> str:
    """Compile src/<name>.c into an objdiff base object.

    CodeWarrior game code uses mwccmips (via wibo); Sony-SDK functions marked
    '// COMPILER: eegcc' use ee-gcc 2.96 via tools/eegcc/ee-compile.sh.
    """
    flags = file_cflags(name)
    comp = file_compiler(name)
    if comp == "eegcc":
        return f"tools/eegcc/ee-compile.sh src/{name}.c build/obj/{name}.o {flags}"
    # Per-file CodeWarrior build selection. byte-identity is what matters, not the
    # .comment string (which isn't in the loadable region) — so a later build that
    # reproduces a function's exact bytes is a valid match (same principle as the
    # eegcc SDK funcs). All user-supplied at tools/mwccps2*/ (gitignored). 991202 is
    # the default; 2.3.3 cracks the idiom-13 delay-slot family; 2.4/3.0/3.0.1 (the
    # next codegen core) crack regalloc-order / branch-lowering ties 2.3.x miss.
    MWCC = {
        "mwcc233": "tools/mwccps2-233/mwccps2.exe",
        "mwcc24":  "tools/mwccps2-24/mwccps2.exe",
        "mwcc30":  "tools/mwccps2-30/mwccps2.exe",
        "mwcc301": "tools/mwccps2-301/mwccps2.exe",
    }
    exe = MWCC.get(comp, "tools/mwccps2/mwccmips.exe")
    return (f"qemu-i386 tools/bin/wibo32 {exe} "
            f"-c {flags} -o build/obj/{name}.o src/{name}.c")


def run_splat() -> None:
    if not (CONFIG / "SCUS_971.12").exists():
        sys.exit("error: config/SCUS_971.12 (boot ELF) is missing — extract it "
                 "from your own disc first. See docs/PROGRESS.md.")
    splat = Path(sys.executable).with_name("splat")
    cmd = [str(splat) if splat.exists() else "splat", "split", str(SPLAT_YAML)]
    print("[splat]", " ".join(cmd))
    subprocess.run(cmd, cwd=ROOT, check=True)


def write_objdiff() -> None:
    """Regenerate objdiff.json from the translation units in src/."""
    unit_cfgs = [{
        "name": n,
        "target_path": f"build/expected/{n}.o",
        "base_path": f"build/obj/{n}.o",
        "metadata": {"source_path": f"src/{n}.c"},
    } for n in units()]
    cfg = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "min_version": "2.0.0",
        "custom_make": "python3",
        "custom_args": ["tools/decomp/build.py", "--single-file"],
        "build_target": False,
        "watch_patterns": ["*.c", "*.h", "*.s"],
        "units": unit_cfgs,
    }
    OBJDIFF_JSON.write_text(json.dumps(cfg, indent=2) + "\n")
    print(f"[objdiff] wrote {OBJDIFF_JSON.relative_to(ROOT)} ({len(unit_cfgs)} unit(s))")


def cmd_setup(_a: argparse.Namespace) -> None:
    BUILD.mkdir(exist_ok=True)
    run_splat()
    # splat regenerates every .s from scratch, which would undo the VU /
    # endlabel-trailer fixups. Re-apply them so the target objects stay
    # assemblable.
    subprocess.run([sys.executable, str(ROOT / "tools/decomp/asm_fixup.py")],
                   cwd=ROOT, check=True)
    write_objdiff()


CHUNK = 200  # container exec arg-length limit ~1MB; ~200 cmds * ~250 chars fits


def _run_chunked(setup: str, cmds: list[str]) -> None:
    """Run `setup; cmd1; cmd2; ...` in container, batched by CHUNK.

    Uses `;` rather than `&&` so one failing compile (e.g. an asm-void file with
    syntax mwcc rejects) doesn't kill the whole batch — the link step's
    fill_unmatched.py falls back to the splat-assembled .s for missing .o files.
    """
    for i in range(0, len(cmds), CHUNK):
        batch = cmds[i:i + CHUNK]
        container("; ".join([setup, *batch]) + "; true")


def cmd_expected(_a: argparse.Namespace) -> None:
    names = units()
    if not names:
        print("[expected] no src/*.c — nothing to assemble")
        return
    for n in names:
        normalize_asm(n)
    _run_chunked("mkdir -p build/expected build/.asmnorm",
                 [assemble_cmd(n) for n in names])
    print(f"[expected] assembled {len(names)} target object(s)")


def cmd_compile(_a: argparse.Namespace) -> None:
    names = units()
    if not names:
        print("[compile] no src/*.c — nothing to compile")
        return
    _run_chunked("mkdir -p build/obj", [compile_cmd(n) for n in names])
    print(f"[compile] compiled {len(names)} base object(s)")
    # Post-process: inject gp_rel/hi/lo relocations derived from splat .s into
    # mwcc-emitted objects (asm-void .word references can't attach relocs from
    # the compiler).  See tools/decomp/inject_relocs.py for the rationale.
    subprocess.run([sys.executable, str(ROOT / "tools/decomp/inject_relocs.py")],
                   cwd=ROOT, check=False)


def cmd_build(a: argparse.Namespace) -> None:
    cmd_expected(a)
    cmd_compile(a)


def cmd_objdiff(_a: argparse.Namespace) -> None:
    write_objdiff()


def single_file(obj: str) -> None:
    """objdiff custom-make hook: compile the base object for one unit."""
    name = Path(obj).stem
    if not (SRC / f"{name}.c").exists():
        sys.exit(f"error: no src/{name}.c for {obj}")
    container(f"mkdir -p build/obj && {compile_cmd(name)}")
    if file_compiler(name) != "eegcc":  # ee-gcc emits standard ELF relocs; no raw-cast injection
        subprocess.run([sys.executable, str(ROOT / "tools/decomp/inject_relocs.py"), name],
                       cwd=ROOT, check=False)


def main(argv: list[str]) -> int:
    if argv and argv[0] == "--single-file":
        if len(argv) < 2:
            sys.exit("error: --single-file needs an object path")
        single_file(argv[1])
        return 0

    p = argparse.ArgumentParser(description="Extermination decomp build driver")
    sub = p.add_subparsers(dest="cmd", required=True)
    sub.add_parser("setup", help="run splat over the boot ELF + write objdiff.json")
    sub.add_parser("expected", help="assemble splat disassembly -> target objects")
    sub.add_parser("compile", help="compile src/*.c -> base objects")
    sub.add_parser("build", help="expected + compile")
    sub.add_parser("objdiff", help="(re)write objdiff.json from src/")

    args = p.parse_args(argv)
    {"setup": cmd_setup, "expected": cmd_expected, "compile": cmd_compile,
     "build": cmd_build, "objdiff": cmd_objdiff}[args.cmd](args)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
