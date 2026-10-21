#!/usr/bin/env python3
"""Host-only tests for tu_static.py on synthetic objects (no game data, no compiler).

The synthetic objects mimic mwcc's layout: every function in its own .text
section with a .mwcats catalog section linked to it, relocations in .rel
sections, local symbols before globals.
"""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import tu_static  # noqa: E402

SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_REL = 1, 2, 3, 9
MWCATS = 0x804A2A82  # SHT_LOUSER + 0x4a2a82, as mwcc writes .mwcats
R_MIPS_32, R_MIPS_26 = 2, 4
HELPER = bytes(range(8))           # 2 synthetic words
CALLER = bytes(range(16, 32))      # 4 synthetic words; reloc at offset 4


def elf(sections, symbols, strtab):
    """sections: [(name, type, flags, link, info, align, entsize, data)], index 0 = null.
    symbols: [(name_off, value, size, info, shndx)] including the null symbol."""
    shstr = b"\0"
    names = []
    for s in sections[1:]:
        names.append(len(shstr))
        shstr += s[0].encode() + b"\0"
    symdata = b"".join(struct.pack("<IIIBBH", n, v, sz, i, 0, x) for n, v, sz, i, x in symbols)
    out = bytearray(52)
    out[:7] = b"\x7fELF\x01\x01\x01"
    hdrs = [[0] * 10]
    for k, s in enumerate(sections[1:], 1):
        name, typ, flags, link, info, align, ent, data = s
        if typ == SHT_SYMTAB:
            data = symdata
        elif name == ".strtab":
            data = strtab
        elif name == ".shstrtab":
            data = shstr
        while len(out) % max(align, 1):
            out.append(0)
        hdrs.append([names[k - 1], typ, flags, 0, len(out), len(data), link, info, align, ent])
        out += data
    while len(out) % 4:
        out.append(0)
    shoff = len(out)
    for h in hdrs:
        out += struct.pack("<10I", *h)
    struct.pack_into("<I", out, 0x20, shoff)
    struct.pack_into("<HHH", out, 0x2E, 40, len(hdrs), 3)
    return bytes(out)


def strtab_of(*names):
    blob, offs = b"\0", {}
    for n in names:
        offs[n] = len(blob)
        blob += n.encode() + b"\0"
    return blob, offs


def compiled_tu():
    st, o = strtab_of("helper", "caller", "ext", ".mwcats_helper", ".mwcats_caller")
    rel_text = struct.pack("<II", 4, (1 << 8) | R_MIPS_26) + struct.pack("<II", 8, (5 << 8) | R_MIPS_26)
    sections = [
        None,
        (".symtab", SHT_SYMTAB, 0, 2, 4, 4, 16, b""),
        (".strtab", SHT_STRTAB, 0, 0, 0, 1, 0, b""),
        (".shstrtab", SHT_STRTAB, 0, 0, 0, 1, 0, b""),
        (".text", SHT_PROGBITS, 6, 0, 0, 16, 0, HELPER),                        # 4: helper
        (".mwcats", MWCATS, 0, 4, 0, 4, 0, b"\0" * 8),                         # 5
        (".rel.mwcats", SHT_REL, 0, 1, 5, 4, 8, struct.pack("<II", 4, (1 << 8) | R_MIPS_32)),
        (".text", SHT_PROGBITS, 6, 0, 0, 16, 0, CALLER),                        # 7: caller
        (".rel.text", SHT_REL, 0, 1, 7, 4, 8, rel_text),                         # 8
        (".mwcats", MWCATS, 0, 7, 0, 4, 0, b"\0" * 8),                         # 9
        (".rel.mwcats", SHT_REL, 0, 1, 9, 4, 8, struct.pack("<II", 4, (4 << 8) | R_MIPS_32)),
    ]
    symbols = [
        (0, 0, 0, 0, 0),
        (o["helper"], 0, len(HELPER), 0x02, 4),        # LOCAL FUNC
        (o[".mwcats_helper"], 0, 8, 0x03, 5),         # LOCAL SECTION
        (o[".mwcats_caller"], 0, 8, 0x03, 9),
        (o["caller"], 0, len(CALLER), 0x12, 7),        # GLOBAL FUNC
        (o["ext"], 0, 0, 0x10, 0),                     # GLOBAL undefined
    ]
    return elf(sections, symbols, st)


def expected_helper(code=HELPER):
    st, o = strtab_of("helper")
    sections = [
        None,
        (".symtab", SHT_SYMTAB, 0, 2, 1, 4, 16, b""),
        (".strtab", SHT_STRTAB, 0, 0, 0, 1, 0, b""),
        (".shstrtab", SHT_STRTAB, 0, 0, 0, 1, 0, b""),
        (".text", SHT_PROGBITS, 6, 0, 0, 16, 0, code),
    ]
    return elf(sections, [(0, 0, 0, 0, 0), (o["helper"], 0, len(code), 0x12, 4)], st)


class TuStaticTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.old = tu_static.EXPECTED
        tu_static.EXPECTED = self.dir
        (self.dir / "helper.o").write_bytes(expected_helper())

    def tearDown(self):
        tu_static.EXPECTED = self.old
        self.tmp.cleanup()

    def test_strip_leaves_one_function_with_external_call(self):
        out = tu_static.Obj(tu_static.strip(compiled_tu(), "caller", ["helper"]))
        texts = [i for i in range(len(out.sh)) if out.secname(i) == ".text"]
        self.assertEqual(len(texts), 1)
        self.assertEqual(out.body[texts[0]], CALLER)
        mwcats = [i for i in range(len(out.sh)) if out.secname(i) == ".mwcats"]
        self.assertEqual([out.sh[i][6] for i in mwcats], texts)
        names = {out.name(s[0]): s for s in out.syms[1:]}
        self.assertEqual(names["caller"][5], texts[0])
        self.assertEqual((names["helper"][3] >> 4, names["helper"][5]), (1, 0))  # GLOBAL, undefined
        self.assertNotIn(".mwcats_helper", names)
        locals_ = sum(1 for s in out.syms if (s[3] >> 4) == 0)
        self.assertEqual(out.sh[out.symtab][7], locals_)
        rel = next(i for i, h in enumerate(out.sh) if h[1] == SHT_REL and h[7] == texts[0])
        targets = [(r[0], out.name(out.syms[r[1] >> 8][0]), r[1] & 0xFF) for r in out.relocs(rel)]
        self.assertEqual(targets, [(4, "helper", R_MIPS_26), (8, "ext", R_MIPS_26)])

    def test_rerun_is_a_no_op(self):
        once = tu_static.strip(compiled_tu(), "caller", ["helper"])
        self.assertEqual(tu_static.strip(once, "caller", ["helper"]), once)

    def test_guard_rejects_a_copy_that_differs_from_the_original(self):
        (self.dir / "helper.o").write_bytes(expected_helper(bytes(8)))
        with self.assertRaisesRegex(ValueError, "differs from the original"):
            tu_static.strip(compiled_tu(), "caller", ["helper"])

    def test_guard_requires_the_expected_object(self):
        (self.dir / "helper.o").unlink()
        with self.assertRaisesRegex(ValueError, "missing"):
            tu_static.strip(compiled_tu(), "caller", ["helper"])

    def test_failure_deletes_the_object(self):
        (self.dir / "helper.o").write_bytes(expected_helper(bytes(8)))
        obj = self.dir / "caller.o"
        obj.write_bytes(compiled_tu())
        self.assertEqual(tu_static.main([str(obj), "caller", "helper"]), 1)
        self.assertFalse(obj.exists())

    def test_directive_parsing(self):
        src = "// COMPILER: mwcc233\n// TU_STATIC: func_00179680 func_0021B900\n// TU_STATIC:x\nint a;\n"
        self.assertEqual(tu_static.tu_static_names(src), ["func_00179680", "func_0021B900", "x"])


if __name__ == "__main__":
    unittest.main()
