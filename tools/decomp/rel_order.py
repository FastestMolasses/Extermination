#!/usr/bin/env python3
"""Align the ORDER of an expected object's .rel.text entries with the compiled
object's, when the two already agree on everything else.

objdiff reads REL-style MIPS relocations and pairs each R_MIPS_HI16 with a
following R_MIPS_LO16 to recover the high half's addend. Two assemblers can
emit the same set of relocations in a different order (GNU as moves each HI16
in front of the LO16 it guesses belongs to it), so two objects with identical
.text bytes and the identical set of (offset, type, symbol) relocations can
still score below 100% (func_00116598: two %hi loads of D_0027CCC0 paired with
different %lo halves).

align(expected, compiled) rewrites the expected object's .rel.text in the
compiled object's order, and only when:
  - both objects have one .text with byte-identical contents, and
  - the multisets of (offset, type, symbol name) in .rel.text are equal.
Anything else is left alone, so a real code or relocation difference is never
hidden. The expected object is the objdiff target only; it is never linked.
"""
import struct
import sys
from collections import Counter
from pathlib import Path


def _sections(d):
    shoff = struct.unpack_from("<I", d, 0x20)[0]
    shnum = struct.unpack_from("<H", d, 0x30)[0]
    shstr = struct.unpack_from("<H", d, 0x32)[0]
    sh = [struct.unpack_from("<IIIIIIIIII", d, shoff + i * 40) for i in range(shnum)]
    so = sh[shstr][4]
    names = [d[so + s[0]:d.index(b"\0", so + s[0])].decode() for s in sh]
    return sh, names


def _read(path):
    d = bytearray(Path(path).read_bytes())
    sh, names = _sections(d)
    if names.count(".text") != 1 or names.count(".rel.text") != 1:
        return None
    text = sh[names.index(".text")]
    rel = sh[names.index(".rel.text")]
    sym = sh[rel[6]]                      # sh_link of .rel.text -> .symtab
    strs = sh[sym[6]]

    def symname(i):
        o = strs[4] + struct.unpack_from("<I", d, sym[4] + i * 16)[0]
        return d[o:d.index(b"\0", o)].decode()

    entries = []
    for k in range(rel[5] // 8):
        off, info = struct.unpack_from("<II", d, rel[4] + k * 8)
        entries.append(((off, info & 0xFF, symname(info >> 8)), (off, info)))
    return d, bytes(d[text[4]:text[4] + text[5]]), rel, entries


def align(expected, compiled):
    e = _read(expected)
    c = _read(compiled)
    if e is None or c is None:
        return False
    ed, etext, erel, eent = e
    _, ctext, _, cent = c
    if etext != ctext:
        return False
    ekeys = [k for k, _ in eent]
    ckeys = [k for k, _ in cent]
    if ekeys == ckeys or Counter(ekeys) != Counter(ckeys):
        return False
    pool = {}
    for k, raw in eent:
        pool.setdefault(k, []).append(raw)
    for i, k in enumerate(ckeys):
        off, info = pool[k].pop(0)
        struct.pack_into("<II", ed, erel[4] + i * 8, off, info)
    Path(expected).write_bytes(bytes(ed))
    return True


if __name__ == "__main__":
    print(align(sys.argv[1], sys.argv[2]))
