#!/usr/bin/env python3
"""Respell the expected object's address references the way the compiled C does.

objdiff compares relocations by symbol name. Three spellings of the same
address reach the linked ELF as identical bytes but count as mismatches:

  * the C accesses an aggregate (a vec4, a struct, a short table) and every
    access relocates against the aggregate's symbol plus an offset
    (`D_007A5570+0x10`), while splat names each accessed word as its own
    symbol (`D_007A5580`);
  * the C names a scratchpad word as an extern (`D_700038A0[2]`) while splat
    leaves the site a bare literal (`0x700038A8`), or the reverse;
  * splat cannot pair a hoisted `%hi` with its `%lo` and leaves a bare
    constant on one side.

This tool reads the COMPILED object's HI16/LO16 relocations for one function
and, instruction by instruction, rewrites the matching site of the expected
`.s` to the compiled spelling, but only when both sides resolve to the SAME
address and the instruction words agree outside the 16-bit immediate. It
never touches an instruction that differs, so a real mismatch stays a
mismatch; it only removes name differences that the linker erases anyway.
Only `D_<8 hex>` symbols (whose address is their name) and literals are
handled; any other symbol is left alone.

Usage (host Python, no dependencies):
    reloc_normalize.py <compiled.o> <func> <expected.s>   rewrites in place,
    exit 0 = changed, 3 = unchanged, other = error.
"""
from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

R_MIPS_HI16, R_MIPS_LO16 = 5, 6
_DSYM = re.compile(r"^D_([0-9A-Fa-f]{8})$")
_LINE = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+([0-9A-Fa-f]{8})\s*\*/")
_REF = re.compile(r"%(hi|lo)\((D_[0-9A-Fa-f]{8})(?:\s*([+-])\s*(0x[0-9A-Fa-f]+|\d+))?\)")
_LIT = re.compile(r"\((0x[0-9A-Fa-f]+) (>> 16|& 0xFFFF)\)")


def _sext16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def _hi(addr: int) -> int:
    return ((addr + 0x8000) >> 16) & 0xFFFF


def read_function(obj: Path, func: str):
    """Return (words by function-relative offset, {offset: (type, symname)}) or None."""
    data = obj.read_bytes()
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        return None
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    secs = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    shstr = secs[shstrndx]

    def cstr(off: int) -> str:
        end = data.index(b"\0", off)
        return data[off:end].decode("latin-1")

    symtab = next((s for s in secs if s[1] == 2), None)
    if symtab is None:
        return None
    strtab = secs[symtab[6]]
    syms = []
    for i in range(symtab[5] // 16):
        name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", data, symtab[4] + i * 16)
        syms.append((cstr(strtab[4] + name), value, size, shndx))
    fn = next((s for s in syms if s[0] == func and s[3] not in (0, 0xFFF1, 0xFFF2)), None)
    if fn is None:
        return None
    _, fval, fsize, fsec = fn
    text = secs[fsec]
    body = data[text[4] + fval: text[4] + fval + (fsize or text[5] - fval)]
    words = {o: struct.unpack_from("<I", body, o)[0] for o in range(0, len(body) - 3, 4)}
    relocs: dict[int, tuple[int, str]] = {}
    for s in secs:
        if s[1] != 9 or s[7] != fsec:  # SHT_REL applying to the function's section
            continue
        for i in range(s[5] // 8):
            off, info = struct.unpack_from("<II", data, s[4] + i * 8)
            typ, symi = info & 0xFF, info >> 8
            if typ in (R_MIPS_HI16, R_MIPS_LO16) and fval <= off < fval + len(body):
                relocs[off - fval] = (typ, syms[symi][0])
    return words, relocs


def compiled_targets(relocs: dict) -> dict[int, tuple[str, str]]:
    """{offset: ('hi'|'lo', symbol)} for HI16/LO16 relocations against D_ symbols."""
    return {off: ("hi" if typ == R_MIPS_HI16 else "lo", sym)
            for off, (typ, sym) in relocs.items() if _DSYM.match(sym)}


def spell(part: str, sym: str, addr: int) -> str:
    base = int(sym[2:], 16)
    off = addr - base
    inner = sym if off == 0 else f"{sym}{'+' if off > 0 else '-'}0x{abs(off):X}"
    return f"%{part}({inner})"


def normalize(obj: Path, func: str, src: str) -> str | None:
    got = read_function(obj, func)
    if got is None:
        return None
    words, relocs = got
    targets = compiled_targets(relocs)
    lines = src.splitlines(keepends=True)
    start = None
    changed = False
    for i, line in enumerate(lines):
        m = _LINE.search(line)
        if not m:
            continue
        vram = int(m.group(1), 16)
        if start is None:
            start = vram
        off = vram - start
        if off not in words:
            continue
        b = bytes.fromhex(m.group(2))
        want = struct.unpack("<I", b)[0]
        have = words[off]
        if (want ^ have) & 0xFFFF0000:
            continue  # different instruction: leave the mismatch visible
        code = line[m.end():]
        ref = _REF.search(code)
        lit = _LIT.search(code)
        if ref:
            part = ref.group(1)
            eaddr = int(ref.group(2)[2:], 16)
            if ref.group(3):
                d = int(ref.group(4), 0)
                eaddr += d if ref.group(3) == "+" else -d
            span = ref.span()
        elif lit:
            part = "hi" if lit.group(2) == ">> 16" else "lo"
            eaddr = int(lit.group(1), 16)
            span = lit.span()
        else:
            continue
        imm = have & 0xFFFF
        if off in targets:
            # REL objects keep the addend in the immediate: the compiled site
            # resolves to csym + addend. Respell the expected site as
            # csym + (its own address - csym); it assembles to the same
            # immediate exactly when both sites resolve to the same address.
            cpart, csym = targets[off]
            delta = eaddr - int(csym[2:], 16)
            if cpart != part or not -0x8000 <= delta <= 0x7FFF:
                continue
            if (part == "hi" and imm != _hi(delta)) or (part == "lo" and imm != delta & 0xFFFF):
                continue
            new = spell(part, csym, eaddr)
        elif off in relocs:
            continue  # relocated against something this tool does not model
        else:
            # compiled has a bare constant here: spell the expected side literally
            if (part == "hi" and imm != _hi(eaddr)) or (part == "lo" and imm != eaddr & 0xFFFF):
                continue
            new = f"(0x{eaddr:08X} >> 16)" if part == "hi" else f"(0x{eaddr:08X} & 0xFFFF)"
        old = code[span[0]:span[1]]
        if old == new:
            continue
        code = code[:span[0]] + new + code[span[1]:]
        lines[i] = line[:m.end()] + code
        changed = True
    return "".join(lines) if changed else None


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print(__doc__)
        return 2
    obj, func, s_path = Path(argv[0]), argv[1], Path(argv[2])
    if not obj.exists() or not s_path.exists():
        return 2
    out = normalize(obj, func, s_path.read_text(errors="ignore"))
    if out is None:
        return 3
    s_path.write_text(out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
