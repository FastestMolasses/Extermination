#!/usr/bin/env python3
"""Generate build/jtblrodata/<func>.s — each function's own switch jump table(s).

A switch jump table is .rodata of the SAME translation unit as its function
(proven s85: 159 adjacent function->table pairs, table addresses monotonic in
function addresses, ZERO inversions). Assembling a function without its table
leaves jtbl_XXXX undefined-external in the expected object, so the dispatch
relocation can never match the local table mwcc necessarily emits — the artifact
that was recorded for two sessions as the "jr-table external-dispatch wall".

MULTI-TABLE FUNCTIONS: SOLVED (s85). A function with N switch tables needs N
SEPARATE sections, each ALSO literally named `.rodata`, each starting at offset 0
with align 2**4 — that is exactly what mwcc emits (one local object per table).
GNU as produces distinct same-named ELF sections with the `unique,<id>` form:

    .section .rodata,"a",@progbits,unique,0
    .align 4
    jtbl_XXXXXXXX:

Getting there took two wrong turns worth recording, because both look right:

  1. Concatenating every table into ONE .rodata puts the 2nd and later tables at
     a nonzero offset. objdiff resolves a local data reloc by (section, offset),
     so only the first table's dispatch matches. Caps at ~99.96%.
  2. Splitting into UNIQUELY-NAMED sections (.rodata.jtbl_XXXX) also fails, and
     slightly worse than (1) — renaming stops objdiff pairing the section with
     mwcc's, costing more than the addend it fixes:
         function        concatenated   renamed sections
         func_001CFBE0      99.961           99.922
         func_0022B7A0      99.959           99.918
     The name must stay `.rodata`; only the section IDENTITY may differ.

Verified: func_0020EE50 went 99.95 -> 100.0 the moment the `unique` form was used.

Method note: measure with the CFLAGS from each file's own header. A first pass at
comparison (1) vs (2) used -sdatathreshold 0 for a file whose header says 4 and
produced a bogus 97.05 that nearly got written down as a large regression.
Compile flags are part of the measurement.

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
    """Rewrite jtbl_prep's rodata dump: one `unique` .rodata section per table.

    `glabel` becomes a plain label (it expands to .ent, which is illegal outside
    .text), splat's nonmatching/dlabel/endlabel bookkeeping is dropped, and each
    table is placed at offset 0 of its own section — see the module docstring for
    why the section name must remain `.rodata`.
    """
    body = re.sub(r"^\s*glabel\s+(\w+)\s*$", r"\1:", raw, flags=re.M)
    body = re.sub(r"^\s*(nonmatching|dlabel|endlabel).*$", "", body, flags=re.M)

    chunks: list[list[str]] = []
    cur: list[str] = []
    for line in body.splitlines():
        m = re.match(r"^\s*(jtbl_[0-9A-Fa-f]+):\s*$", line)
        if m:
            chunks.append(cur)
            cur = [m.group(1)]
        elif cur:
            cur.append(line)
    chunks.append(cur)

    parts, n = [], 0
    for c in chunks:
        if not c or not c[0].startswith("jtbl_"):
            continue
        rest = [l for l in c[1:] if l.strip() and not l.strip().startswith(".section")]
        parts.append(f'.section .rodata,"a",@progbits,unique,{n}\n.align 4\n'
                     f'{c[0]}:\n' + "\n".join(rest))
        n += 1
    return "\n".join(parts) + "\n" if parts else body


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
        n = text.count(',"a",@progbits,unique,')
        if n > 1:
            multi += 1
        open(f"{OUT}/{b}.s", "w").write(text)
        ok += 1
    print(f"wrote {ok} jtbl rodata files ({multi} with multiple tables)")


if __name__ == "__main__":
    main()
