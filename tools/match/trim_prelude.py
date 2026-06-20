#!/usr/bin/env python3
"""Trim the inlined m2c prelude in a matched src file down to the symbols actually used.

A matched file is: [// CFLAGS line] + [exterm_prelude.h block ending at its #endif] + [body].
Removing UNUSED typedefs/#defines cannot change codegen, so the objdiff match is preserved
(re-verified by the caller). Produces a lean, readable file.
"""
import re, sys

BASE_INTS = ["s8", "u8", "s16", "u16", "s32", "u32", "s64", "u64"]
TYPEDEF = {
    "s8": "typedef signed char        s8;",
    "u8": "typedef unsigned char      u8;",
    "s16": "typedef short              s16;",
    "u16": "typedef unsigned short     u16;",
    "s32": "typedef int                s32;",
    "u32": "typedef unsigned int       u32;",
    "s64": "typedef long long          s64;",
    "u64": "typedef unsigned long long u64;",
    "f32": "typedef float              f32;",
    "f64": "typedef double             f64;",
}
M2C_MACROS = {
    "M2C_FIELD": "#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))",
    "M2C_BITWISE": "#define M2C_BITWISE(type, expr) ((type)(expr))",
    "M2C_ERROR": "#define M2C_ERROR(desc) (0)",
    "M2C_TRAP_IF": "#define M2C_TRAP_IF(cond) (0)",
    "M2C_CARRY": "#define M2C_CARRY 0",
    "M2C_OVERFLOW": "#define M2C_OVERFLOW(a) (0)",
    "M2C_LWL": "#define M2C_LWL(expr) (expr)",
    "M2C_UNALIGNED32": "#define M2C_UNALIGNED32(expr) (expr)",
    "M2C_FIRST3BYTES": "#define M2C_FIRST3BYTES(expr) (expr)",
    "MULT_HI": "#define MULT_HI(a, b) (0)",
    "MULTU_HI": "#define MULTU_HI(a, b) (0)",
    "CLZ": "#define CLZ(x) (0)",
}
M2C_UNK = {
    "M2C_UNK": "typedef s32 M2C_UNK;",
    "M2C_UNK8": "typedef s8  M2C_UNK8;",
    "M2C_UNK16": "typedef s16 M2C_UNK16;",
    "M2C_UNK32": "typedef s32 M2C_UNK32;",
    "M2C_UNK64": "typedef s64 M2C_UNK64;",
}

def trim(text):
    lines = text.splitlines()
    cflags = lines[0] if lines and lines[0].startswith("// CFLAGS:") else None
    # body = everything after the prelude's closing #endif (EXTERM_M2C_PRELUDE_H)
    end = None
    for i, l in enumerate(lines):
        if "EXTERM_M2C_PRELUDE_H" in l and l.strip().startswith("#endif"):
            end = i; break
    if end is None:
        return text  # not our prelude shape; leave as-is
    body = "\n".join(lines[end + 1:]).strip("\n")
    used = lambda sym: re.search(r"\b" + re.escape(sym) + r"\b", body) is not None
    pre = []
    # base int/float types
    needs_int = any(used(s) for s in BASE_INTS) or any(used(m) for m in M2C_UNK) or any(used(m) for m in M2C_MACROS)
    for t in BASE_INTS:
        if needs_int: pre.append(TYPEDEF[t])
    for t in ("f32", "f64"):
        if used(t): pre.append(TYPEDEF[t])
    if used("s128") or used("u128"):
        pre.append("typedef int   s128 __attribute__((mode(TI)));")
        pre.append("typedef unsigned u128 __attribute__((mode(TI)));")
    if used("NULL"):
        pre.append("#define NULL ((void *)0)")
    if used("memcpy"):
        pre.append("extern void *memcpy(void *, const void *, unsigned int);")
    for name in ("M2C_UNK", "M2C_UNK8", "M2C_UNK16", "M2C_UNK32", "M2C_UNK64"):
        if used(name): pre.append(M2C_UNK[name])
    for name, line in M2C_MACROS.items():
        if used(name): pre.append(line)
    out = []
    if cflags: out.append(cflags)
    if pre: out += pre + [""]
    out.append(body)
    return "\n".join(out) + "\n"

if __name__ == "__main__":
    for path in sys.argv[1:]:
        t = open(path).read()
        open(path, "w").write(trim(t))
    print(f"trimmed {len(sys.argv)-1} file(s)")
