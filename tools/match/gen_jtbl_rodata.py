#!/usr/bin/env python3
"""Generate build/jtblrodata/<func>.s — each function's own switch jump table(s).

A switch jump table is .rodata of the SAME translation unit as its function
(proven s85: 159 adjacent function->table pairs, table addresses monotonic in
function addresses, ZERO inversions). Assembling a function without its table
leaves jtbl_XXXX undefined-external in the expected object, so the dispatch
relocation can never match the local table mwcc necessarily emits — the artifact
that was recorded for two sessions as the "jr-table external-dispatch wall".

MULTI-TABLE FUNCTIONS ARE STILL UNRESOLVED (2 known: func_001CFBE0,
func_0022B7A0, both ~99.96%). Two agents independently diagnosed it as a section
layout problem: mwcc emits each switch table as its own local .rodata object at
offset 0, whereas concatenating them into one .rodata puts the second table at a
nonzero offset, so its dispatch mismatches by the addend.

That diagnosis was TESTED and the fix does NOT work, so it is not applied.
A/B with each file compiled at its own recorded CFLAGS:

    function        concatenated   split into per-table sections
    func_001CFBE0      99.961              99.922
    func_0022B7A0      99.959              99.918

Emitting each table into its own uniquely-named section (.rodata.jtbl_XXXX)
apparently stops objdiff pairing it with mwcc's anonymous local section, which
costs slightly more than the addend it fixes. The concatenated form below is the
better-measuring one and is what we keep; these two functions stay at ~99.96%.

Method note for anyone retrying: measure with the CFLAGS from each file's own
header. A first pass at this comparison used -sdatathreshold 0 for a file whose
header says 4 and produced a bogus 97.05, which nearly got written down as a
large regression. Compile flags are part of the measurement.

Usage: gen_jtbl_rodata.py [func ...]     (no args = every jtbl-referencing func)
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(ROOT)
ASM = "build/asm/matchings/main/code"
OUT = "build/jtblrodata"


def targets():
    out = []
    for f in glob.glob(f"{ASM}/func_*.s"):
        b = os.path.basename(f)[:-2]
        m = re.match(r"func_([0-9A-Fa-f]{8})$", b)
        if not m or int(m.group(1), 16) >= 0x240000:
            continue
        with open(f, errors="ignore") as fh:
            if "jtbl_" in fh.read():
                out.append(b)
    return sorted(out)


def convert(raw: str) -> str:
    """Rewrite jtbl_prep's rodata dump into one assembler-acceptable .rodata block.

    `glabel` becomes a plain label (it expands to .ent, which is illegal outside
    .text), and splat's nonmatching/dlabel/endlabel bookkeeping is dropped.
    """
    body = re.sub(r"^\s*glabel\s+(\w+)\s*$", r"\1:", raw, flags=re.M)
    body = re.sub(r"^\s*(nonmatching|dlabel|endlabel).*$", "", body, flags=re.M)
    return body


def main() -> None:
    names = sys.argv[1:] or targets()
    os.makedirs(OUT, exist_ok=True)
    ok = multi = 0
    for b in names:
        subprocess.run([sys.executable, "tools/match/jtbl_prep.py", b],
                       capture_output=True, text=True)
        src = f"build/match/jtbl/{b}.jtbl.s"
        if not os.path.exists(src):
            continue
        text = convert(open(src, errors="ignore").read())
        n = text.count(".section .rodata.")
        if n > 1:
            multi += 1
        open(f"{OUT}/{b}.s", "w").write(text)
        ok += 1
    print(f"wrote {ok} jtbl rodata files ({multi} with multiple tables)")


if __name__ == "__main__":
    main()
