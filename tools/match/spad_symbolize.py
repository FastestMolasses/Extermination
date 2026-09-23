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
frequent type wins the declaration and the others become VALUE casts, never
address-of — taking the address would put back the register materialisation
this fix exists to remove. `*(T2 *)0xADDR` is a value of type T2, so its value
cast is `(T2)D_ADDR` (s87 fix: this used to emit `(T2 *)D_ADDR`, i.e.
`(unsigned short * *)D_70003B6C` on func_001AC480, a pointer-to-pointer that
does not even type-check against the `unsigned short *` it was assigned to).
A value cast only reproduces the original load when both types are pointers
(same 4-byte `lw`) and the use is not an lvalue (a cast is not assignable).
Any other second-type use — a scalar of a different width, or a store /
increment / address-of through the second type — is LEFT AS THE LITERAL and
reported as a leftover for a hand fix, rather than rewritten into something
that reads a different width or does not compile.

Under a nonzero `-sdatathreshold` a 4-byte scalar extern becomes gp-relative,
which the target never is (the scratchpad is nowhere near $gp). The symbol is
then over-declared as an array (idiom #20, `extern T D_ADDR[16];`) and every
use becomes `D_ADDR[0]` — the form func_001AC480 was hand-fixed to. Only an
EXPLICIT `-sdatathreshold 0` gets the plain scalar extern (the func_00201720
form above, which declares `-sdatathreshold 0`). A file with no `// CFLAGS:`
line, or one whose CFLAGS omit the flag, compiles under mwcc's DEFAULT
threshold, which is 8 bytes — measured s87 on mwccmips 991202, mwccps2 2.3.3
and mwccps2 2.4 with bare `-O4,p`: extern char arrays of 1/2/4/5/8 bytes got
R_MIPS_GPREL16, 9 bytes and up got R_MIPS_HI16/LO16, identical to an explicit
`-sdatathreshold 8`. So the default also takes the array form. (Measured for
mwcc only; no `// COMPILER: eegcc` file references these addresses.)

The lvalue test is syntactic and deliberately conservative: when in doubt the
use is left as a literal (a LEFTOVER to hand-fix), never rewritten into code
that could compile to something else. Grouping parentheses around the
expression are peeled first, so `(*(T **)X)++` is seen as an increment. A
leading `&` counts as address-of only in unary position (not after an
operand, and not the second `&` of `&&`). An assignment after the expression
does not make it an lvalue when a `*` directly precedes it — in
`**(T **)X = v` the store goes through the outer dereference and
`*(T *)D_X = v` is valid. Run `spad_symbolize.py --selftest` for the cases.

The matching change on the target side lives in build.py's normalize_asm(), and
is deliberately PER-FILE OPT-IN: it symbolizes a function's .s only when that
function's C already references the symbol. A global rewrite would hand
relocations to the expected objects of the 35 already-matched functions that
still spell these addresses as literals, breaking every one of them.

Usage: spad_symbolize.py <func> [<func> ...]     rewrite src/<func>.c in place
       spad_symbolize.py --list                  show candidates and their status
       spad_symbolize.py --selftest              check the rewrite rules (<1 s)
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


def declare(typ: str, sym: str, dim: str = "") -> str:
    """`extern <typ> <sym><dim>;` with the star glued to the name when typ ends in *."""
    return f"extern {typ}{'' if typ.endswith('*') else ' '}{sym}{dim};"


CFLAGS_LINE = re.compile(r'^//\s*CFLAGS:(.*)$')
SDATA = re.compile(r'-sdatathreshold\s+(\d+)')
# mwcc's threshold when none is given: a file with no `// CFLAGS:` line
# (build.py's global "-O4,p") or with CFLAGS that omit the flag. Measured s87
# on all three mwcc builds with bare -O4,p: objects of <= 8 bytes are gprel,
# >= 9 bytes are %hi/%lo — the same split as an explicit -sdatathreshold 8.
DEFAULT_SDATA = 8


def sdata_threshold(text: str) -> int:
    """-sdatathreshold from the leading-block `// CFLAGS:` line (build.py rules)."""
    for line in text.splitlines():
        s = line.strip()
        if not s:
            continue
        if not s.startswith("//"):
            break
        m = CFLAGS_LINE.match(s)
        if m:
            t = SDATA.search(m.group(1))
            return int(t.group(1)) if t else DEFAULT_SDATA
    return DEFAULT_SDATA


def is_ptr(typ: str) -> bool:
    return typ.rstrip().endswith("*")


# An lvalue use: the cast expression is assigned, incremented, or has its
# address taken. A value cast is not an lvalue, so these cannot be rewritten.
_ASSIGN_AFTER = re.compile(r'\s*(?:=(?!=)|[-+*/%&|^]=|<<=|>>=)')
_POSTFIX_AFTER = re.compile(r'\s*(?:\+\+|--)')
_OPERAND_END = re.compile(r'[\w\)\]]$')
_KEYWORDS_BEFORE_UNARY = ("return", "case", "sizeof", "else", "do")


def _operand_before(before: str) -> bool:
    """True when `before` (already rstripped) ends with an operand, i.e. a
    following `&`/`*`/`(` is binary / a call rather than unary / grouping."""
    if not _OPERAND_END.search(before):
        return False
    w = re.search(r'([A-Za-z_]\w*)$', before)
    return not (w and w.group(1) in _KEYWORDS_BEFORE_UNARY)


def is_lvalue_use(s: str, start: int, end: int) -> bool:
    """Is the expression s[start:end] used as an lvalue (store, ++/--, &)?"""
    # Peel grouping parentheses — only a `(` in unary position, so the parens
    # of a call `f(...)` or of `if (...)` are not mistaken for grouping.
    i, j = start, end
    while True:
        before = s[:i].rstrip()
        k = j
        while k < len(s) and s[k].isspace():
            k += 1
        if (before.endswith("(") and k < len(s) and s[k] == ")"
                and not _operand_before(before[:-1].rstrip())):
            i, j = len(before) - 1, k + 1
            continue
        break
    before = s[:i].rstrip()
    if _POSTFIX_AFTER.match(s, j):
        return True                # postfix binds tighter than any prefix op
    if before.endswith("++") or before.endswith("--"):
        return True
    if (before.endswith("&") and not before.endswith("&&")
            and not _operand_before(before[:-1].rstrip())):
        return True                # unary address-of
    if _ASSIGN_AFTER.match(s, j):
        # `**(T **)X = v`: the store goes through the outer `*`.
        return not before.endswith("*")
    return False


def convert(text: str):
    """Returns (new_text, n_rewritten, {sym: declared_type}).

    Second-type uses that cannot be rewritten faithfully are left as literals;
    main() reports them as LEFTOVER.
    """
    types = collections.defaultdict(collections.Counter)
    for m in DEREF.finditer(text):
        types[m.group(2)][m.group(1).strip()] += 1
    if not types:
        return text, 0, {}

    chosen = {a: c.most_common(1)[0][0] for a, c in types.items()}
    thr = sdata_threshold(text)
    # Over-declare past the threshold so mwcc keeps %hi/%lo (idiom #20).
    dim = f"[{16 if thr < 16 else thr + 1}]" if thr > 0 else ""
    ref = (lambda s: s + "[0]") if dim else (lambda s: s)
    n = 0

    def rep(m):
        nonlocal n
        typ, addr = m.group(1).strip(), m.group(2)
        sym = ref(SYM[addr])
        if typ == chosen[addr]:
            n += 1
            return sym
        # Different type at the same address. `*(typ *)ADDR` is a `typ` value,
        # so the faithful value cast is `(typ)sym` — valid only pointer-to-
        # pointer (same load width) and only as an rvalue.
        lvalue = is_lvalue_use(m.string, m.start(), m.end())
        if is_ptr(typ) and is_ptr(chosen[addr]) and not lvalue:
            n += 1
            return f"({typ}){sym}"
        return m.group(0)          # leave the literal; reported as LEFTOVER

    out = DEREF.sub(rep, text)

    decls = ""
    for addr, typ in sorted(chosen.items()):
        sym = SYM[addr]
        if not re.search(r'^\s*extern\b.*\b' + sym + r'\b', out, re.M):
            decls += f"{declare(typ, sym, dim):<52}/* PS2 scratchpad @ {addr} */\n"
    if decls:
        i = leading_comment_end(out)
        out = out[:i] + decls + "\n" + out[i:]
    return out, n, {SYM[a]: t + dim for a, t in chosen.items()}


def status(text: str) -> str:
    if "// NEARMISS" in text:
        return "NEARMISS"
    return "STUB" if "INCLUDE_ASM" in text else "MATCHED"


def selftest() -> int:
    """Rewrite rules on synthetic inputs; no files touched, well under 1 s."""
    hdr0 = "// CFLAGS: -O4,p -sdatathreshold 0\n"
    lit = "0x70003B6C"
    main_ty = f"*(char **){lit}; *(char **){lit}; "
    cases = [
        # (header, statement using the second type, expected substring)
        (hdr0, f"x = *(short **){lit};", "(short *)D_70003B6C;"),
        (hdr0, f"x = (*(short **){lit})[2];", "((short *)D_70003B6C)[2]"),
        (hdr0, f"*(short **){lit} = p;", f"*(short **){lit} = p;"),
        (hdr0, f"(*(short **){lit})++;", f"(*(short **){lit})++;"),
        (hdr0, f"p = &*(short **){lit};", f"&*(short **){lit}"),
        (hdr0, f"return &*(short **){lit};", f"&*(short **){lit}"),
        (hdr0, f"x = a & *(short **){lit};", "a & (short *)D_70003B6C"),
        (hdr0, f"x = a && *(short **){lit};", "a && (short *)D_70003B6C"),
        (hdr0, f"**(short **){lit} = 1;", "*(short *)D_70003B6C = 1;"),
        (hdr0, f"if (*(short **){lit}) ++x;", "if ((short *)D_70003B6C) ++x;"),
        (hdr0, f"f(*(short **){lit});", "f((short *)D_70003B6C);"),
        (hdr0, f"x = *(int *){lit};", f"*(int *){lit}"),      # width differs
        # threshold: explicit 0 -> scalar; absent / flag-less / nonzero -> array
        (hdr0, "", "extern char *D_70003B6C;"),
        ("", "", "extern char *D_70003B6C[16];"),
        ("// CFLAGS: -O4,p\n", "", "extern char *D_70003B6C[16];"),
        ("// CFLAGS: -O4,p -sdatathreshold 4\n", "", "extern char *D_70003B6C[16];"),
        ("// CFLAGS: -O4,p -sdatathreshold 32\n", "", "extern char *D_70003B6C[33];"),
        ("", f"x = *(short **){lit};", "(short *)D_70003B6C[0];"),
    ]
    bad = 0
    for hdr, stmt, want in cases:
        out, _, _ = convert(hdr + "void f(void) { " + main_ty + stmt + " }\n")
        if want not in out:
            bad += 1
            print(f"FAIL {hdr.strip() or '(no CFLAGS)'} | {stmt!r}\n  want {want!r}\n  got  {out!r}")
    print(f"selftest: {len(cases) - bad}/{len(cases)} passed")
    return 1 if bad else 0


def main(argv):
    if argv and argv[0] == "--selftest":
        return selftest()
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
