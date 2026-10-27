#!/usr/bin/env python3
"""Host-only tests for reloc_normalize.py on synthetic objects (no game data).

The synthetic object holds one function in .text with REL relocations, as
mwcc writes them: the addend lives in the instruction's 16-bit immediate.
The expected side is a splat-style .s with `/* rom vram word */` comments;
the tool reads only the address spelling, so the opcodes are placeholders.
"""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import reloc_normalize as RN  # noqa: E402

SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_REL = 1, 2, 3, 9
HI, LO = RN.R_MIPS_HI16, RN.R_MIPS_LO16


def elf(words, relocs, func="f"):
    """One .text section holding `words`; relocs = [(offset, type, symname)]."""
    text = b"".join(struct.pack("<I", w) for w in words)
    names = [func] + sorted({s for _, _, s in relocs})
    strtab = b"\0"
    offs = {}
    for n in names:
        offs[n] = len(strtab)
        strtab += n.encode() + b"\0"
    syms = [struct.pack("<IIIBBH", 0, 0, 0, 0, 0, 0),
            struct.pack("<IIIBBH", offs[func], 0, len(text), 0x12, 0, 1)]
    index = {func: 1}
    for n in names[1:]:
        index[n] = len(syms)
        syms.append(struct.pack("<IIIBBH", offs[n], 0, 0, 0x10, 0, 0))
    symtab = b"".join(syms)
    rel = b"".join(struct.pack("<II", o, (index[s] << 8) | t) for o, t, s in relocs)
    sections = [("", 0, 0, 0, b""), (".text", SHT_PROGBITS, 0, 0, text),
                (".symtab", SHT_SYMTAB, 3, 1, symtab), (".strtab", SHT_STRTAB, 0, 0, strtab),
                (".rel.text", SHT_REL, 2, 1, rel), (".shstrtab", SHT_STRTAB, 0, 0, b"")]
    shstr = b"\0"
    name_off = []
    for s in sections:
        name_off.append(len(shstr) if s[0] else 0)
        if s[0]:
            shstr += s[0].encode() + b"\0"
    sections[-1] = (".shstrtab", SHT_STRTAB, 0, 0, shstr)
    body = b""
    placed = []
    for s in sections:
        placed.append(52 + len(body))
        body += s[4]
    shoff = 52 + len(body)
    out = bytearray(b"\x7fELF\x01\x01\x01" + bytes(9))
    out += struct.pack("<HHIIIIIHHHHHH", 1, 8, 1, 0, 0, shoff, 0, 52, 0, 0, 40, len(sections), len(sections) - 1)
    out += body
    for i, (n, typ, link, info, data) in enumerate(sections):
        entsize = 16 if typ == SHT_SYMTAB else 8 if typ == SHT_REL else 0
        out += struct.pack("<IIIIIIIIII", name_off[i], typ, 0, 0, placed[i], len(data), link, info, 4, entsize)
    return bytes(out)


def line(vram, word, text):
    return f"    /* 0 {vram:08X} {struct.pack('<I', word).hex().upper()} */  {text}\n"


class Normalize(unittest.TestCase):
    def run_case(self, words, relocs, expected_words, texts):
        with tempfile.TemporaryDirectory() as d:
            obj = Path(d) / "f.o"
            obj.write_bytes(elf(words, relocs))
            src = "glabel f\n" + "".join(line(0x1000 + 4 * i, w, t) for i, (w, t) in enumerate(zip(expected_words, texts)))
            return RN.normalize(obj, "f", src)

    def test_split_symbol_becomes_base_plus_offset(self):
        # compiled: upper half 0, store at +0x10, both relocated against D_007A5570
        words = [0x3C010000, 0xAC220010]
        out = self.run_case(words, [(0, HI, "D_007A5570"), (4, LO, "D_007A5570")],
                            [0x3C01007A, 0xAC225580],
                            ["op1 $at, %hi(D_007A5580)", "op2 $v0, %lo(D_007A5580)($at)"])
        self.assertIn("%hi(D_007A5570+0x10)", out)
        self.assertIn("%lo(D_007A5570+0x10)($at)", out)

    def test_literal_becomes_relocated_element(self):
        words = [0x3C010000, 0xC4220008]
        out = self.run_case(words, [(0, HI, "D_700038A0"), (4, LO, "D_700038A0")],
                            [0x3C017000, 0xC42238A8],
                            ["op1 $at, (0x700038A8 >> 16)", "op2 $f2, (0x700038A8 & 0xFFFF)($at)"])
        self.assertIn("%hi(D_700038A0+0x8)", out)
        self.assertIn("%lo(D_700038A0+0x8)($at)", out)

    def test_relocation_becomes_literal_when_compiled_is_bare(self):
        words = [0x3C017000, 0xAC2038A4]
        out = self.run_case(words, [], [0x3C017000, 0xAC2038A4],
                            ["op1 $at, %hi(D_700038A4)", "op2 $zero, %lo(D_700038A4)($at)"])
        self.assertIn("(0x700038A4 >> 16)", out)
        self.assertIn("(0x700038A4 & 0xFFFF)($at)", out)

    def test_different_address_is_left_alone(self):
        # compiled resolves to D_700038A0+4, target to 0x700038A8: a real mismatch
        words = [0x3C010000, 0xC4220004]
        out = self.run_case(words, [(0, HI, "D_700038A0"), (4, LO, "D_700038A0")],
                            [0x3C017000, 0xC42238A8],
                            ["op1 $at, (0x700038A8 >> 16)", "op2 $f2, (0x700038A8 & 0xFFFF)($at)"])
        self.assertTrue(out is None or "(0x700038A8 & 0xFFFF)" in out)

    def test_different_instruction_is_left_alone(self):
        words = [0x3C010000, 0xAC230010]   # stores a different register than the target
        out = self.run_case(words, [(0, HI, "D_007A5570"), (4, LO, "D_007A5570")],
                            [0x3C01007A, 0xAC225580],
                            ["op1 $at, %hi(D_007A5580)", "op2 $v0, %lo(D_007A5580)($at)"])
        self.assertTrue(out is None or "%lo(D_007A5580)" in out)

    def test_non_address_symbol_is_left_alone(self):
        words = [0x3C010000, 0xAC220000]
        out = self.run_case(words, [(0, HI, "some_table"), (4, LO, "some_table")],
                            [0x3C01007A, 0xAC225580],
                            ["op1 $at, %hi(D_007A5580)", "op2 $v0, %lo(D_007A5580)($at)"])
        self.assertIsNone(out)


if __name__ == "__main__":
    unittest.main()
