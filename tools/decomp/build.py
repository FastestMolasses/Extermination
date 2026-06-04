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
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config"
SRC = ROOT / "src"
BUILD = ROOT / "build"

SPLAT_YAML = CONFIG / "SCUS_971.12.yaml"
OBJDIFF_JSON = ROOT / "objdiff.json"

IMAGE = "exterm-toolchain"
# splat writes each auto-detected function's disassembly here.
ASM_DIR = "build/asm/matchings/main/code"
# mwccmips flags. -O4,p (speed) matches the trivial functions decompiled so
# far; expect to tune per translation unit as matching proceeds.
CFLAGS = "-O4,p"


def is_asm_stub(path: Path) -> bool:
    """True if src/<name>.c is an INCLUDE_ASM stub (undecompiled placeholder).

    A stub is a committed per-function file whose byte-identical machine code is
    supplied by the locally-assembled splat disassembly (fill_unmatched.py), NOT
    by compiling this C.  It exists so every function is a claimable unit without
    committing the disassembly.  Marked by a first-line `// INCLUDE_ASM` comment.
    Stubs are skipped by compile/expected/objdiff; the linker assembles their .s.
    """
    try:
        with path.open() as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                return line.startswith("// INCLUDE_ASM")
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
    """
    return (f"mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc "
            f"build/macro.inc {ASM_DIR}/{name}.s -o build/expected/{name}.o")


def file_cflags(name: str) -> str:
    """Read per-file CFLAGS from a '// CFLAGS: ...' comment in src/<name>.c.

    If the first non-blank line of the file is a C++ comment starting with
    '// CFLAGS:', the remainder of that line replaces the default CFLAGS.
    Otherwise the global CFLAGS constant is used.
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
                break
    except OSError:
        pass
    return CFLAGS


def compile_cmd(name: str) -> str:
    """Compile src/<name>.c into an objdiff base object via mwccmips."""
    flags = file_cflags(name)
    return (f"qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe "
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
    _run_chunked("mkdir -p build/expected", [assemble_cmd(n) for n in names])
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
