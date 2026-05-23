#!/usr/bin/env python3
"""Patch splat's per-function disassembly so GNU `as` can assemble it.

`spimdisasm` emits VU0 macro-mode instructions in Sony's syntax
(`vdiv Q, $vf0w, $vf5x`, `vmulq.xyzw $vf4, $vf4, Q`). GNU `mipsel-linux-gnu-as`
recognizes the VU mnemonics in r5900 mode but rejects this exact operand form
(it wants its own field-selector spelling). Rather than translate, we just
replace any unassembleable VU line with a `.word 0xHEX` directive — the raw
machine code from splat's own comment is in every line.

The result: the same bytes hit `build/expected/*.o`, the assembler is happy,
and our objdiff base side (mwcc inline asm) can also use `.word` if needed.

Usage:
    .venv/bin/python tools/decomp/asm_fixup.py           # patch all .s files
    .venv/bin/python tools/decomp/asm_fixup.py --check   # just count, no write
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ASM = ROOT / "build/asm/matchings/main/code"

# Splat line format (after .set noreorder etc.):
#     /* fileoff vram HEXBYTES */  mnemonic operands
# Match the comment + the instruction text after it.
INSN_LINE = re.compile(
    r"^(\s*)/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*(.+?)\s*$"
)

# Mnemonics GNU `as` r5900 mode does *not* accept from spimdisasm output.
# (Empirically determined; expand as we hit more.) Anything matching these
# prefixes/patterns will be neutralized via `.word`.
VU_PATTERNS = [
    re.compile(r"^v[a-z]+\."),                  # v<op>.<field>: vadd.xyz, vmulq.x...
    re.compile(r"^v(div|mul|add|sub|sqr|sqrt|rsqrt|rnext|rxor|rget|rinit|"
               r"nop|waitq|move|mr32|ftoi[0-9]?|itof[0-9]?|min|max|abs|"
               r"clip|cmpwq|opmula|opmsub|maddq|msubq|callms|callmsr|"
               r"sqrt|iadd|isub|iand|ior|iaddi|iaddiu|sqi|lqi|ilw|isw|"
               r"mfir|mtir)\b"),
]


def is_vu(insn_text: str) -> bool:
    # First token is the mnemonic.
    mnemonic = insn_text.split(None, 1)[0]
    return any(p.match(mnemonic) for p in VU_PATTERNS)


def patch(text: str) -> tuple[str, int]:
    """Return (patched_text, lines_changed).

    Also strips any lines AFTER `endlabel` — splat appends a trailing
    instruction (typically a `nop`) of context, but GNU `as` would assemble
    it into .text, inflating the section size past the real function and
    breaking objdiff's section-level match check.
    """
    out = []
    changed = 0
    seen_endlabel = False
    for line in text.splitlines(keepends=False):
        if seen_endlabel:
            # Drop trailing instruction lines past endlabel.
            if INSN_LINE.match(line):
                changed += 1
                continue
            out.append(line)
            continue
        if line.lstrip().startswith("endlabel "):
            seen_endlabel = True
            out.append(line)
            continue
        m = INSN_LINE.match(line)
        if m and is_vu(m.group(5)):
            indent, foff, vram, hex_bytes, insn = m.groups()
            # splat prints the four bytes little-endian; .word emits them as a
            # 32-bit word — same endianness, same bytes.
            word = int.from_bytes(bytes.fromhex(hex_bytes), "little")
            out.append(f"{indent}/* {foff} {vram} {hex_bytes} */  "
                       f".word 0x{word:08x}  /* was: {insn} */")
            changed += 1
        else:
            out.append(line)
    return "\n".join(out) + ("\n" if text.endswith("\n") else ""), changed


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--check", action="store_true",
                   help="report what would change, don't write")
    p.add_argument("--dir", type=Path, default=ASM,
                   help=f"directory of .s files to patch (default: {ASM})")
    args = p.parse_args(argv)

    if not args.dir.exists():
        sys.exit(f"error: {args.dir} not found — run `build.py setup` first")
    total_files = 0
    total_lines = 0
    patched_files: list[tuple[str, int]] = []
    for f in sorted(args.dir.glob("*.s")):
        total_files += 1
        original = f.read_text()
        patched, n = patch(original)
        if n:
            total_lines += n
            patched_files.append((f.name, n))
            if not args.check:
                f.write_text(patched)
    verb = "would patch" if args.check else "patched"
    print(f"{verb} {len(patched_files)} of {total_files} files "
          f"({total_lines} VU instructions -> .word)")
    for name, n in patched_files[:20]:
        print(f"  {n:3} lines  {name}")
    if len(patched_files) > 20:
        print(f"  ... and {len(patched_files) - 20} more files")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
