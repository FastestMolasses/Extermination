#!/usr/bin/env python3
"""Drop same-TU static callee copies from a compiled object (`// TU_STATIC:`).

WHY (decomp lane round 7, 2026-10-20). The original compiler saw several
functions in one translation unit. When a caller's callee was a `static`
function earlier in the same file, CodeWarrior knew which registers that callee
uses and kept values in caller-saved registers across the call: `$a0` still
holding the pointer that the callee read, or a byte loaded into `$a1` because
`$a0` was still live. A one-function source file cannot reproduce that; the
caller then reloads or spills, and the residual looks like a colouring or
branch-likely wall (MATCHING_GUIDE "Same-TU callee register knowledge").
mwcc 2.3.3 reproduces the original code once the callee is a `static`
definition placed before the caller in the same file, and it emits every
function into its own `.text` section, so the callee copy is separable.

HOW. A caller source opts in with

    // TU_STATIC: func_00179680
    ...
    static void func_00179680(unsigned char *p);   /* internal linkage */
    #include "src/func_00179680.c"                  /* the callee's own unit */

(the prototype gives the included non-static definition internal linkage,
C99 6.2.2; the include path is relative to the compiler's working directory,
the repository root). After compiling, this script rewrites the object:

  1. each named callee must be a LOCAL FUNC symbol alone in its own `.text`
     section; that section, its `.mwcats` catalog and their relocation
     sections are removed;
  2. GUARD: the removed bytes must equal the ORIGINAL callee, i.e. the callee's
     bytes and relocation offsets/types in build/expected/<callee>.o (the
     splat-assembled target). This proves the static copy compiled to exactly
     the original function, so the register knowledge mwcc applied is the
     original callee's;
  3. the callee symbol becomes an undefined GLOBAL, so the caller's calls are
     ordinary external relocations resolved to the callee's own unit at link
     time (that unit stays a normal, separately compiled object).

The result is an ordinary one-function object: objdiff, fill_unmatched,
link.py and audit_link_provenance treat it like any other compiled unit.
Nothing is added or substituted; only the separately linked callee's
duplicate section is removed. On any failed check the object is DELETED and
the script exits non-zero, so the linker falls back to the splat .s and the
function is not counted as matched.

usage: tu_static.py OBJECT CALLER CALLEE [CALLEE ...]
       tu_static.py --src CALLER     (prints the TU_STATIC names of src/CALLER.c)
"""
from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXPECTED = ROOT / "build" / "expected"
SHT_SYMTAB, SHT_REL, SHT_RELA, SHT_NOBITS = 2, 9, 4, 8
DIRECTIVE = re.compile(r"^//\s*TU_STATIC:(.*)$", re.M)


def tu_static_names(src_text: str) -> list[str]:
    out = []
    for m in DIRECTIVE.finditer(src_text):
        out += [t for t in m.group(1).split() if re.fullmatch(r"[A-Za-z_]\w*", t)]
    return out


class Obj:
    def __init__(self, data: bytes):
        if data[:6] != b"\x7fELF\x01\x01":
            raise ValueError("not an ELF32 little-endian object")
        self.data = data
        shoff = struct.unpack_from("<I", data, 0x20)[0]
        shent, shnum, self.shstrndx = struct.unpack_from("<HHH", data, 0x2E)
        self.sh = [list(struct.unpack_from("<10I", data, shoff + i * shent)) for i in range(shnum)]
        self.body = [data[h[4]:h[4] + h[5]] if h[1] != SHT_NOBITS else b"" for h in self.sh]
        self.symtab = next(i for i, h in enumerate(self.sh) if h[1] == SHT_SYMTAB)
        st = self.sh[self.symtab]
        self.strtab = self.body[st[6]]
        raw = self.body[self.symtab]
        self.syms = [list(struct.unpack_from("<IIIBBH", raw, k)) for k in range(0, len(raw), 16)]

    def name(self, off: int, table: bytes | None = None) -> str:
        t = self.strtab if table is None else table
        return t[off:t.index(b"\0", off)].decode("ascii", "replace")

    def secname(self, i: int) -> str:
        return self.name(self.sh[i][0], self.body[self.shstrndx])

    def relocs(self, i: int) -> list[tuple]:
        h = self.sh[i]
        step = 12 if h[1] == SHT_RELA else 8
        return [struct.unpack_from("<III" if step == 12 else "<II", self.body[i], k)
                for k in range(0, len(self.body[i]), step)]

    def find(self, name: str) -> int | None:
        for k, s in enumerate(self.syms):
            if k and self.name(s[0]) == name:
                return k
        return None


def expected_callee(name: str) -> tuple[bytes, list[tuple[int, int]]]:
    path = EXPECTED / f"{name}.o"
    if not path.is_file():
        raise ValueError(f"guard: {path} is missing (the callee must be a compiled unit)")
    e = Obj(path.read_bytes())
    k = e.find(name)
    if k is None:
        raise ValueError(f"guard: {name} is not defined in its expected object")
    value, size, shndx = e.syms[k][1], e.syms[k][2], e.syms[k][5]
    code = e.body[shndx][value:value + size]
    rel = []
    for i, h in enumerate(e.sh):
        if h[1] in (SHT_REL, SHT_RELA) and h[7] == shndx:
            rel += [(r[0] - value, r[1] & 0xFF) for r in e.relocs(i) if value <= r[0] < value + size]
    return code, sorted(rel)


def strip(data: bytes, caller: str, callees: list[str]) -> bytes:
    o = Obj(data)
    drop: set[int] = set()
    helper_syms: list[int] = []
    ck = o.find(caller)
    if ck is None or (o.syms[ck][3] >> 4) != 1 or (o.syms[ck][3] & 0xF) != 2:
        raise ValueError(f"{caller} is not a global function in the object")
    for name in callees:
        k = o.find(name)
        if k is None:
            raise ValueError(f"{name}: not in the object (no static copy compiled)")
        s = o.syms[k]
        if s[5] == 0 and (s[3] >> 4) == 1:
            continue  # already an undefined global: this object was stripped before
        if (s[3] >> 4) != 0 or (s[3] & 0xF) != 2 or not (0 < s[5] < 0xFF00):
            raise ValueError(f"{name}: not a defined local function (declare it static before including it)")
        sec = s[5]
        if o.secname(sec) != ".text" or sec == o.syms[ck][5]:
            raise ValueError(f"{name}: not alone in its own .text section")
        others = [j for j, t in enumerate(o.syms) if j and j != k and t[5] == sec and (t[3] & 0xF) != 3]
        if others or s[1] != 0 or s[2] != o.sh[sec][5]:
            raise ValueError(f"{name}: its .text section holds other code")
        code = o.body[sec]
        rel = []
        for i, h in enumerate(o.sh):
            if h[1] in (SHT_REL, SHT_RELA) and h[7] == sec:
                for r in o.relocs(i):
                    t = o.syms[r[1] >> 8]
                    if (t[3] >> 4) == 0 and t[5] != sec and (t[3] & 0xF) != 2:
                        raise ValueError(f"{name}: references local data of this object")
                    rel.append((r[0], r[1] & 0xFF))
        want_code, want_rel = expected_callee(name)
        if code != want_code or sorted(rel) != want_rel:
            raise ValueError(f"guard: the static copy of {name} differs from the original callee")
        drop.add(sec)
        helper_syms.append(k)
    if not helper_syms:
        return data
    # Close over sections tied to a dropped one (its .mwcats catalog, relocation sections).
    changed = True
    while changed:
        changed = False
        for i, h in enumerate(o.sh):
            if i in drop or h[1] == SHT_SYMTAB:
                continue
            tied = (h[1] in (SHT_REL, SHT_RELA) and h[7] in drop) or (h[1] not in (SHT_REL, SHT_RELA) and h[6] in drop)
            if tied:
                drop.add(i)
                changed = True
    for i, h in enumerate(o.sh):
        if i in drop or h[1] not in (SHT_REL, SHT_RELA):
            continue
        for r in o.relocs(i):
            j = r[1] >> 8
            if j not in helper_syms and 0 < o.syms[j][5] < 0xFF00 and o.syms[j][5] in drop:
                raise ValueError(f"kept section {o.secname(i)} references a dropped section")
    # Symbol table: null, kept locals, globals, then the callees as undefined globals.
    keep_local = [j for j, s in enumerate(o.syms) if j and (s[3] >> 4) == 0 and j not in helper_syms
                  and not (0 < s[5] < 0xFF00 and s[5] in drop)]
    globals_ = [j for j, s in enumerate(o.syms) if j and (s[3] >> 4) != 0]
    order = [0] + keep_local + globals_ + helper_syms
    symmap = {old: new for new, old in enumerate(order)}
    kept = [i for i in range(len(o.sh)) if i not in drop]
    secmap = {old: new for new, old in enumerate(kept)}
    syms = []
    for old in order:
        s = list(o.syms[old])
        if old in helper_syms:
            s = [s[0], 0, 0, (1 << 4) | 0, 0, 0]  # GLOBAL NOTYPE, undefined
        elif 0 < s[5] < 0xFF00:
            s[5] = secmap[s[5]]
        syms.append(s)
    body = list(o.body)
    body[o.symtab] = b"".join(struct.pack("<IIIBBH", *s) for s in syms)
    for i in kept:
        h = o.sh[i]
        if h[1] in (SHT_REL, SHT_RELA):
            out = b""
            for r in o.relocs(i):
                info = (symmap[r[1] >> 8] << 8) | (r[1] & 0xFF)
                out += struct.pack("<III", r[0], info, r[2]) if len(r) == 3 else struct.pack("<II", r[0], info)
            body[i] = out
    # Re-lay the file: ELF header, section bodies in order, then the section headers.
    hdr = bytearray(data[:0x34])
    blob = bytearray(hdr)
    newsh = []
    for i in kept:
        h = list(o.sh[i])
        if i == 0:
            newsh.append([0] * 10)
            continue
        if h[1] != SHT_NOBITS:
            al = max(h[8], 1)
            while len(blob) % al:
                blob.append(0)
            h[4] = len(blob)
            h[5] = len(body[i])
            blob += body[i]
        if h[6]:
            h[6] = secmap[h[6]]
        if h[1] in (SHT_REL, SHT_RELA):
            h[7] = secmap[h[7]]
        elif h[1] == SHT_SYMTAB:
            h[7] = 1 + len(keep_local)
        newsh.append(h)
    while len(blob) % 4:
        blob.append(0)
    shoff = len(blob)
    for h in newsh:
        blob += struct.pack("<10I", *h)
    struct.pack_into("<I", blob, 0x20, shoff)
    struct.pack_into("<HHH", blob, 0x2E, 40, len(newsh), secmap[o.shstrndx])
    return bytes(blob)


def main(argv: list[str]) -> int:
    if len(argv) == 2 and argv[0] == "--src":
        print(" ".join(tu_static_names((ROOT / "src" / f"{argv[1]}.c").read_text(errors="replace"))))
        return 0
    if len(argv) < 3:
        print(__doc__)
        return 2
    path, caller, callees = Path(argv[0]), argv[1], argv[2:]
    try:
        path.write_bytes(strip(path.read_bytes(), caller, callees))
    except (ValueError, OSError, StopIteration, KeyError) as e:
        print(f"tu_static: {caller}: {e}; object removed", file=sys.stderr)
        try:
            path.unlink()
        except OSError:
            pass
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
