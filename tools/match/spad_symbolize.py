#!/usr/bin/env python3
"""Convert PS2 scratchpad address literals in src/*.c to symbol references.

WHY (s86). Two scratchpad globals — 0x70003B6C (a pointer) and 0x70003B8D (a
mode byte) — are in no symbol list, so our C spells them as address literals:

    *(unsigned char **)0x70003B6C

The original build referenced them as ordinary externs, so every access is a
%hi/%lo RELOCATION pair. The target's disassembly proves it: it reloads $at
with `lui` before EVERY access and never reuses it, which is what a relocated
address forces. Given a literal instead, mwcc keeps 0x7000 live in a register,
CSEs it across blocks, and speculates the `lui` into delay slots the original
leaves as `nop` — the single root cause behind a large family of parked
near-misses.

THE DECLARATION FORM IS THE WHOLE TRICK. Measured on func_00201720:

    literal (baseline)                                    98.79%
    extern char D_70003B6C[];  + (T **)D_70003B6C         94.21%   WORSE
    extern unsigned char *D_70003B6C;  + D_70003B6C      100.00%

Casting an array symbol makes mwcc materialise the address in a register first
(lui/addiu/lw) where the target uses lui/lw. Only declaring the symbol as a
VARIABLE OF THE POINTED-TO TYPE reproduces the target's two-instruction access.
Earlier attempts at this fix failed because they got the symbol right and the
declaration wrong.

So the rewrite is: for a dereferenced cast `*(X *)0xADDR`, declare
`extern X D_ADDR;` and replace the whole expression with `D_ADDR`. Stripping
exactly one trailing `*` off the cast preserves qualifiers by construction —
`char * volatile *` becomes `char * volatile`, keeping the volatility that
distinguishes several of the files.

Where one file dereferences the same address at two different types, the most
frequent type wins the declaration and the others become VALUE casts
(`(T2 *)D_ADDR`), never address-of — taking the address would put back the
register materialisation this fix exists to remove.

The matching change on the target side lives in build.py's normalize_asm(), and
is deliberately PER-FILE OPT-IN: it symbolizes a function's .s only when that
function's C already references the symbol. A global rewrite would hand
relocations to the expected objects of the 35 already-matched functions that
still spell these addresses as literals, breaking every one of them.

Usage: spad_symbolize.py <func> [<func> ...]     rewrite src/<func>.c in place
       spad_symbolize.py --list                  show candidates and their status
"""
import re
import sys
import os
import collections

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)

ADDRS = ("0x70003B6C", "0x70003B8D")
SYM = {a: "D_" + a[2:] for a in ADDRS}

# `*(<type> *)0xADDR` — a dereferenced cast. The captured type EXCLUDES the
# final star, which is exactly the declared type of the symbol.
DEREF = re.compile(r'\*\(\s*([A-Za-z_][\w \*]*?)\s*\*\s*\)\s*(' + "|".join(ADDRS) + r')\b')
# Any use of the literal as a cast operand, to detect leftovers.
ANY = re.compile(r'\)\s*(' + "|".join(ADDRS) + r')\b')


def leading_comment_end(text: str) -> int:
    """Offset of the first line that is neither blank nor a `//` comment.

    The `// COMPILER:` / `// CFLAGS:` directives live in that leading block and
    build.py stops scanning at the first non-comment line, so declarations have
    to be inserted after the block — never before it.
    """
    off = 0
    for line in text.splitlines(keepends=True):
        s = line.strip()
        if s == "" or s.startswith("//"):
            off += len(line)
            continue
        return off
    return off


def declare(typ: str, sym: str) -> str:
    """`extern <typ> <sym>;` with the star glued to the name when typ ends in *."""
    return f"extern {typ}{'' if typ.endswith('*') else ' '}{sym};"


def convert(text: str):
    """Returns (new_text, n_rewritten, {sym: declared_type})."""
    types = collections.defaultdict(collections.Counter)
    for m in DEREF.finditer(text):
        types[m.group(2)][m.group(1).strip()] += 1
    if not types:
        return text, 0, {}

    chosen = {a: c.most_common(1)[0][0] for a, c in types.items()}

    def rep(m):
        typ, addr = m.group(1).strip(), m.group(2)
        sym = SYM[addr]
        if typ == chosen[addr]:
            return sym
        # Different type at the same address: cast the VALUE, not the address.
        return f"({typ} *)" + sym if chosen[addr].endswith("*") else f"({typ}){sym}"

    out, n = DEREF.subn(rep, text)

    decls = ""
    for addr, typ in sorted(chosen.items()):
        sym = SYM[addr]
        if not re.search(r'^\s*extern\b.*\b' + sym + r'\b', out, re.M):
            decls += f"{declare(typ, sym):<52}/* PS2 scratchpad @ {addr} */\n"
    if decls:
        i = leading_comment_end(out)
        out = out[:i] + decls + "\n" + out[i:]
    return out, n, {SYM[a]: t for a, t in chosen.items()}


def status(text: str) -> str:
    if "// NEARMISS" in text:
        return "NEARMISS"
    return "STUB" if "INCLUDE_ASM" in text else "MATCHED"


def main(argv):
    if not argv or argv[0] == "--list":
        import glob
        for p in sorted(glob.glob("src/*.c")):
            t = open(p, errors="ignore").read()
            if not ANY.search(t):
                continue
            n = len(ANY.findall(t))
            d = len(DEREF.findall(t))
            print(f"{os.path.basename(p)[:-2]:16} {status(t):9} refs={n:3} deref={d:3}"
                  f"{'  <-- NON-DEREF USES' if n != d else ''}")
        return 0

    done = 0
    for f in argv:
        p = f if f.endswith(".c") else f"src/{f}.c"
        t = open(p).read()
        out, n, decls = convert(t)
        if not n:
            print(f"{os.path.basename(p):28} no deref-form refs — skipped")
            continue
        left = len(ANY.findall(out))
        open(p, "w").write(out)
        done += 1
        d = " ".join(f"{k}:{v}" for k, v in decls.items())
        print(f"{os.path.basename(p):28} {n:3} refs -> {d}"
              f"{f'   ({left} LEFTOVER)' if left else ''}")
    print(f"\nrewrote {done} file(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
