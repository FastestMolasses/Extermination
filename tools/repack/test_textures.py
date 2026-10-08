"""Native upload/PNG inverse proofs using the established forward decoders."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import struct
import tempfile
import unittest
import zlib

from tools.repack import png, textures
from tools.repack.archive import ROOT, parse_index
from extract_textures import deswizzle, psmct32_word, psmt8_byte, psmt4_nibble
from export_startup import palette_address, upload_module, texture


def synthetic(path: Path, psm=0x13):
    width, height = 64, 32
    raw = bytearray(b"\xa7" * (128 + width * height * 4 + 97))
    for at, value, reg in ((48, 1 << 48, 0x50), (64, 0, 0x51),
                           (80, width | height << 32, 0x52), (96, 0, 0x53)):
        struct.pack_into("<QQ", raw, at, value, reg)
    raw[112:128] = b"\0" * 16
    struct.pack_into("<H", raw, 112, width * height // 4)
    raw[119] = 8
    for i in range(width * height * 4):
        raw[128 + i] = (i * 13 + 5) & 255
    mapping = {psmct32_word(x, y, 1) * 4: 128 + (y * width + x) * 4
               for y in range(height) for x in range(width)}
    for i in range(256 if psm == 0x13 else 16):
        at = mapping[palette_address(8, i, psm)]
        raw[at:at + 4] = bytes((i, i ^ 90, 255 - i, 128 if i else 200))
    for y in range(16):
        for x in range(16):
            if psm == 0x13:
                at = psmt8_byte(x, y, 1)
                raw[mapping[at & ~3] + (at & 3)] = (x + y) % 16
            else:
                nibble = psmt4_nibble(x, y, 1)
                address, shift = nibble // 2, (nibble & 1) * 4
                at = mapping[address & ~3] + (address & 3)
                raw[at] = (raw[at] & ~(15 << shift)) | ((x + y) % 16) << shift
    path.write_bytes(raw)
    return (2 << 14) | (psm << 20) | (4 << 26) | (4 << 30) | (8 << 37)


def edit_pixel(path: Path, index: int, color: bytes):
    width, height, pixels = png.read(path)
    data = bytearray(pixels)
    data[index * 4:index * 4 + 4] = color
    png.write(path, width, height, data)


class TextureTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="texture-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.source = self.base / "source.bin"
        self.tree = self.base / "tree"
        self.output = self.base / "output.bin"

    def test_indices_noop_and_changed_pixel_decode_through_existing_extractor(self):
        synthetic(self.source)
        textures.unpack(self.source, self.tree)
        report = textures.pack(self.tree, self.output)
        self.assertTrue(report["unchanged"])
        self.assertEqual(self.output.read_bytes(), self.source.read_bytes())
        path = self.tree / "upload000.indices.png"
        width, height, rgba = png.read(path)
        expected = bytearray(rgba[::4])
        expected[137] ^= 1
        png.write(path, width, height, expected, mode="L")
        report = textures.pack(self.tree, self.output)
        self.assertEqual(report["changed_native_bytes"], 1)
        rebuilt = self.output.read_bytes()
        self.assertEqual(deswizzle(rebuilt[128:128 + 8192], 64, 32)[2], expected)
        self.assertEqual(rebuilt[:128], self.source.read_bytes()[:128])
        self.assertEqual(rebuilt[-97:], self.source.read_bytes()[-97:])

    def test_rgba_psmt8_and_psmt4_changes_decode_exactly(self):
        for psm in (0x13, 0x14):
            with self.subTest(psm=psm):
                source = self.base / f"source-{psm}.bin"
                token = synthetic(source, psm)
                tree = self.base / f"tree-{psm}"
                textures.unpack(source, tree, tex0=[token])
                textures.pack(tree, self.output)
                self.assertEqual(self.output.read_bytes(), source.read_bytes())
                path = tree / "texture000.png"
                _, _, before = png.read(path)
                target = before[4:8]
                edit_pixel(path, 0, target)
                expected = png.read(path)[2]
                result = textures.pack(tree, self.output)
                self.assertEqual(result["changed_native_bytes"], 1)
                self.assertEqual(texture(*upload_module(self.output), token)[2], expected)

    def test_palette_edit_preserves_saturated_native_alpha(self):
        token = synthetic(self.source)
        textures.unpack(self.source, self.tree, tex0=[token])
        palette = self.tree / "palette000.png"
        _, _, before = png.read(palette)
        edit_pixel(palette, 0, bytes((255, 1, 2, before[3])))
        result = textures.pack(self.tree, self.output)
        self.assertEqual(result["changed_native_bytes"], 3)
        decoded = texture(*upload_module(self.output), token)[2]
        self.assertEqual(decoded[:4], bytes((255, 1, 2, 255)))
        differences = [i for i, (a, b) in enumerate(zip(self.source.read_bytes(), self.output.read_bytes())) if a != b]
        self.assertEqual(differences, list(range(differences[0], differences[0] + 3)))

    def test_unrepresentable_color_requires_explicit_quantization(self):
        token = synthetic(self.source, 0x14)
        textures.unpack(self.source, self.tree, tex0=[token])
        edit_pixel(self.tree / "texture000.png", 0, bytes((255, 0, 0, 255)))
        with self.assertRaisesRegex(ValueError, "absent from its palette"):
            textures.pack(self.tree, self.output)
        result = textures.pack(self.tree, self.output, quantize="nearest")
        self.assertEqual(result["quantized_pixels"], 1)
        self.assertGreater(result["maximum_channel_error"], 0)

    def test_palette_alpha_quantization_is_explicit(self):
        token = synthetic(self.source)
        textures.unpack(self.source, self.tree, tex0=[token])
        _, _, colors = png.read(self.tree / "palette000.png")
        edit_pixel(self.tree / "palette000.png", 0, colors[:3] + b"\x7f")
        with self.assertRaisesRegex(ValueError, "palette alpha"):
            textures.pack(self.tree, self.output)
        result = textures.pack(self.tree, self.output, quantize="nearest")
        self.assertEqual(result["quantized_pixels"], 1)
        self.assertEqual(texture(*upload_module(self.output), token)[2][3], 128)

    def test_conflicting_overlapping_rgba_views_rejected(self):
        token = synthetic(self.source)
        textures.unpack(self.source, self.tree, tex0=[token, token | (1 << 61)])
        _, _, palette = png.read(self.tree / "palette000.png")
        edit_pixel(self.tree / "texture000.png", 0, palette[4:8])
        edit_pixel(self.tree / "texture001.png", 0, palette[8:12])
        with self.assertRaisesRegex(ValueError, "conflicting"):
            textures.pack(self.tree, self.output)

    def test_rejects_dimension_change_colored_indices_and_input_alias(self):
        synthetic(self.source)
        textures.unpack(self.source, self.tree)
        with self.assertRaisesRegex(ValueError, "aliases"):
            textures.pack(self.tree, self.source)
        path = self.tree / "upload000.indices.png"
        edit_pixel(path, 0, b"\xff\0\0\xff")
        with self.assertRaisesRegex(ValueError, "gray index"):
            textures.pack(self.tree, self.output)
        png.write(path, 1, 1, b"\0\0\0\xff")
        with self.assertRaisesRegex(ValueError, "dimensions"):
            textures.pack(self.tree, self.output)

    def test_truncated_native_upload_rejected(self):
        synthetic(self.source)
        self.source.write_bytes(self.source.read_bytes()[:4000])
        with self.assertRaisesRegex(ValueError, "truncated"):
            textures.unpack(self.source, self.tree)

    def test_png_all_filters_and_crc(self):
        width, height = 4, 5
        original = bytes((i * 73 + 19) & 255 for i in range(width * height * 4))
        rows = []
        for y in range(height):
            row, above = original[y * 16:y * 16 + 16], original[(y - 1) * 16:y * 16] if y else bytes(16)
            encoded = bytearray()
            for x, value in enumerate(row):
                a, b, c = row[x - 4] if x >= 4 else 0, above[x], above[x - 4] if x >= 4 else 0
                if y == 4:
                    prediction = a + b - c
                    candidates = (a, b, c)
                    predictor = min(enumerate(candidates), key=lambda item: (abs(prediction - item[1]), item[0]))[1]
                else:
                    predictor = (0, a, b, (a + b) // 2)[y]
                encoded.append((value - predictor) & 255)
            rows.append(bytes((y,)) + encoded)
        blob = (png.SIGNATURE + png._chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)) +
                png._chunk(b"IDAT", zlib.compress(b"".join(rows))) + png._chunk(b"IEND", b""))
        path = self.base / "filters.png"
        path.write_bytes(blob)
        self.assertEqual(png.read(path), (width, height, original))
        damaged = bytearray(blob)
        damaged[-1] ^= 1
        path.write_bytes(damaged)
        with self.assertRaisesRegex(ValueError, "checksum"):
            png.read(path)

    def test_indexed_png_palette_transparency_and_rgb_are_channel_exact(self):
        path = self.base / "indexed.png"
        header = struct.pack(">IIBBBBB", 2, 1, 8, 3, 0, 0, 0)
        path.write_bytes(png.SIGNATURE + png._chunk(b"IHDR", header) +
                         png._chunk(b"PLTE", bytes((12, 34, 56, 78, 90, 123))) +
                         png._chunk(b"tRNS", bytes((37, 255))) +
                         png._chunk(b"IDAT", zlib.compress(b"\0\0\1")) + png._chunk(b"IEND", b""))
        self.assertEqual(png.read(path), (2, 1, bytes((12, 34, 56, 37, 78, 90, 123, 255))))
        header = struct.pack(">IIBBBBB", 1, 1, 8, 2, 0, 0, 0)
        path.write_bytes(png.SIGNATURE + png._chunk(b"IHDR", header) +
                         png._chunk(b"IDAT", zlib.compress(bytes((0, 21, 43, 65)))) + png._chunk(b"IEND", b""))
        self.assertEqual(png.read(path), (1, 1, bytes((21, 43, 65, 255))))


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1", "disc-wide texture proof requires EM_TEST_FULL=1")
class FullTextureTests(unittest.TestCase):
    def test_all_native_transfer_leaves_roundtrip(self):
        from tools.repack.iso import inventory
        source = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        info = inventory(source)
        data = next(f for f in info["files"] if f["path"] == "DATA/DATA.DAT")
        index = next(f for f in info["files"] if f["path"] == "DATA/INDEX.IDX")
        results = []
        with source.open("rb") as original:
            original.seek(index["offset"])
            regions, _ = parse_index(original.read(index["size"]), data["size"])
            with tempfile.TemporaryDirectory(prefix="texture-full-", dir=ROOT / "build/repack") as temp:
                base = Path(temp)
                for region in regions:
                    for entry in region["files"]:
                        original.seek(data["offset"] + region["offset"] + entry["offset"])
                        raw = original.read(entry["size"])
                        uploads = textures._uploads(raw)
                        if not uploads:
                            continue
                        with tempfile.TemporaryDirectory(dir=base) as local:
                            work = Path(local)
                            native = work / "source.bin"
                            native.write_bytes(raw)
                            textures.unpack(native, work / "tree")
                            result = textures.pack(work / "tree", work / "output.bin")
                            self.assertTrue(result["unchanged"], entry["path"])
                            self.assertEqual((work / "output.bin").read_bytes(), raw)
                            results.append(dict(path=entry["path"], size=len(raw), transfers=len(uploads),
                                                sha256=hashlib.sha256(raw).hexdigest(), equal=True))
        self.assertEqual(len(results), 63)
        self.assertEqual(sum(entry["transfers"] for entry in results), 113)
        (ROOT / "build/repack/texture-full.json").write_text(json.dumps(dict(
            source_image_sha256=info["image_sha256"], leaf_count=len(results), transfer_count=113,
            files=results), indent=2) + "\n")


if __name__ == "__main__":
    unittest.main()
