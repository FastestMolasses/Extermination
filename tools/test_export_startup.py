"""Asset-free checks of startup texture addressing and input validation."""
import tempfile
import unittest
from pathlib import Path

from export_startup import palette_address, texture, upload_module, sprite


class StartupExportTest(unittest.TestCase):
    def test_csm1_palette_addresses(self):
        # GS CSM1's first 32 PSMCT32 word addresses. In particular index2
        # maps to word4, not word2: a linear CLUT read scrambles gradients.
        expected = [0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15,
                    64, 65, 68, 69, 72, 73, 76, 77, 66, 67, 70, 71, 74, 75, 78, 79]
        self.assertEqual([palette_address(0, i, 0x13) // 4 for i in range(32)], expected)
        self.assertEqual([palette_address(0, i, 0x14) // 4 for i in range(16)], expected[:16])
        self.assertEqual({palette_address(0, i, 0x13) for i in range(256)},
                         set(range(0, 1024, 4)))

    def test_texture_requires_real_upload_coverage(self):
        with self.assertRaisesRegex(ValueError, 'texture reads outside'):
            texture(bytes(4194304), set(), (2 << 14) | (0x13 << 20))

    def test_psmt4_uses_low_and_high_nibbles(self):
        local = bytearray(4096)
        local[0] = 2
        local[4] = 3
        # TBP0=0, TBW=2, PSMT4, TW=1, TH=0, CBP=8; two texels.
        tex0 = (2 << 14) | (0x14 << 20) | (1 << 26) | (8 << 37)
        local[2048 + 16:2048 + 20] = bytes((9, 10, 11, 128))
        local[2048 + 20:2048 + 24] = bytes((90, 100, 110, 64))
        w, h, pixels = texture(local, {0, 8}, tex0)
        self.assertEqual((w, h), (2, 1))
        self.assertEqual(pixels, bytes((9, 10, 11, 255, 90, 100, 110, 128)))

    def test_truncated_module_is_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'screen.bin'
            path.write_bytes(b'\0' * 32)
            with self.assertRaisesRegex(ValueError, 'truncated'):
                upload_module(path)

    def test_sprite_flips_vertical_only(self):
        canvas = bytearray(b'\0\0\0\xff' * (512 * 448))
        sprite(canvas, (2, 2, bytes((1, 2, 3, 255, 4, 5, 6, 255,
                                    7, 8, 9, 255, 10, 11, 12, 255))), 0, 0, 2, 2)
        self.assertEqual(canvas[:8], bytes((7, 8, 9, 255, 10, 11, 12, 255)))
        self.assertEqual(canvas[2048:2056], bytes((1, 2, 3, 255, 4, 5, 6, 255)))


if __name__ == '__main__':
    unittest.main()
