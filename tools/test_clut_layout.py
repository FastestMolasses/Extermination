#!/usr/bin/env python3
"""Synthetic GS CLUT addressing regressions; no game data required."""
import struct
import unittest

from clut_pair import read_clut16_rgba, read_clut_rgba
from gs_vram import csm1_unswizzle_clut


class ClutLayoutTest(unittest.TestCase):
    def test_all_256_index_addresses(self):
        # Route index bits through the GS manual's CSM1 coordinates and
        # PSMCT32 word/column/block layout. This fixture deliberately does
        # not use the production address table or its permutation helper.
        # Physical word bits 0..7 receive index bits 0,3,1,2,5,6,4,7.
        memory = bytearray(1024)
        expected = bytearray()
        for index in range(256):
            word = sum(((index >> source) & 1) << bit
                       for bit, source in enumerate((0, 3, 1, 2, 5, 6, 4, 7)))
            color = bytes((index, index ^ 0x5a, 255-index, index % 129))
            memory[word * 4:word * 4 + 4] = color
            expected += color
        self.assertEqual(csm1_unswizzle_clut(memory), expected)
        # Include a nonzero base pointer and the consumer's alpha expansion.
        vram = bytes(256 * 37) + memory
        expanded = bytearray(expected)
        expanded[3::4] = bytes(min(255, a * 2) for a in expected[3::4])
        self.assertEqual(read_clut_rgba(vram, 37), expanded)

    def test_psmt4_agrees_with_first_16_psmt8_entries(self):
        # CSM1's first 16 entries occupy the same 8x2 rectangle for both
        # index formats. The pre-existing four-bit reader already decoded
        # the physical word order and catches the old eight-bit bug.
        memory = b''.join(struct.pack('<I', i * 0x01010101) for i in range(256))
        self.assertEqual(read_clut_rgba(memory, 0)[:64],
                         read_clut16_rgba(memory, 0))

    def test_rejects_incomplete_palette(self):
        for size in (0, 64, 1023, 1025):
            with self.assertRaises(ValueError):
                csm1_unswizzle_clut(bytes(size))


if __name__ == '__main__':
    unittest.main()
