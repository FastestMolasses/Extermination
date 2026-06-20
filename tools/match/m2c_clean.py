#!/usr/bin/env python3
"""Make m2c --valid-syntax output compile under CodeWarrior mwccmips.

m2c emits GCC-isms that mwcc's stricter front-end rejects:
  - `void *` pointers used in arithmetic (`p + 0x10`) -> mwcc "illegal type".
    GCC treats void* as size-1, so a byte pointer (s8 *) is the exact equivalent.
  - `*(void *)ADDR = val` width-unknown deref-stores -> mwcc "cannot convert".
    These are word stores in practice; default the cast to s32.
Both transforms preserve byte-offset / store-width semantics, so codegen is
unchanged for the matching case (a wrong width just fails to match and is triaged out).
"""
import re, sys

def clean(t):
    # width-unknown deref casts default to a word store/load
    t = t.replace("*(void *)", "*(s32 *)")
    t = t.replace("*(void**)", "*(s32 *)").replace("*(void *) ", "*(s32 *) ")
    # remaining void* pointers -> byte pointers (GCC void* arithmetic == +1/byte)
    t = re.sub(r"\bvoid \*", "s8 *", t)
    return t

if __name__ == "__main__":
    sys.stdout.write(clean(sys.stdin.read()))
