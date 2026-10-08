"""Native ADPCM editing proofs, with original decoder as the read-back oracle."""
from __future__ import annotations

import hashlib
import json
import math
import os
from pathlib import Path
import struct
import tempfile
import unittest
import wave

from tools import audio_export, decode_sound
from tools.repack import archive, audio, iso

ROOT = Path(__file__).resolve().parents[2]


def raw_clip(frames=8):
    out = bytearray()
    for frame in range(frames):
        flags = 6 if frame == 0 else 3 if frame == frames - 1 else 2
        out.extend((4, flags))
        out.extend(((i + frame) % 16) | ((i + frame + 1) % 16) << 4 for i in range(14))
    return bytes(out)


def bank_fixture():
    bodies = [raw_clip() + bytes([12, 7]) + bytes(14), raw_clip(4)]
    out = bytearray(b"\0" * 0x100)
    size = 0x100 + sum(map(len, bodies))
    struct.pack_into("<8I", out, 0, size, 0x40, 0, 2, 0x100, size - 0x100, 0x100, 0)
    for number, body in enumerate(bodies):
        header = 0x40 + number * 0x40
        struct.pack_into("<4I", out, 0x20 + number * 16, len(body), header, number + 1, 0)
        struct.pack_into("<3I4s", out, header, 0x40, len(body), 0, b"SShd")
        out[header + 24:header + 32] = b"UNKNOWN!"
    return bytes(out) + b"".join(bodies) + b"preserved trailing bytes"


def native_archive_leaves(image: Path, names: set[str] | None = None) -> list[tuple[str, bytes]]:
    """Read selected corrected leaves, or discover all native sound-bank leaves."""
    disc = iso.inventory(image)
    entries = {entry["path"]: entry for entry in disc["files"]}
    index, data = entries["DATA/INDEX.IDX"], entries["DATA/DATA.DAT"]
    found = []
    with image.open("rb") as source:
        source.seek(index["offset"])
        regions, _ = archive.parse_index(source.read(index["size"]), data["size"])
        for region in regions:
            for leaf in region["files"]:
                if names is not None and leaf["path"] not in names:
                    continue
                offset = data["offset"] + region["offset"] + leaf["offset"]
                if names is None:
                    if leaf["size"] < 0x60:
                        continue
                    source.seek(offset)
                    header_offset = struct.unpack("<2I", source.read(8))[1]
                    if not 0x20 <= header_offset <= min(0x400, leaf["size"] - 16):
                        continue
                    source.seek(offset + header_offset + 12)
                    if source.read(4) != b"SShd":
                        continue
                source.seek(offset)
                found.append((leaf["path"], source.read(leaf["size"])))
    if names is not None and {name for name, _ in found} != names:
        raise ValueError("requested native archive leaf is absent from the supplied disc")
    return found


class AudioTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="audio-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)

    def unpack(self, raw, kind="auto"):
        source = self.base / "source.bin"
        source.write_bytes(raw)
        tree = self.base / "tree"
        manifest = audio.unpack_audio(source, tree, kind=kind, rate=32000)
        return tree, manifest

    def test_bank_noop_preserves_headers_terminators_and_padding(self):
        raw = bank_fixture()
        tree, manifest = self.unpack(raw)
        self.assertEqual(manifest["sample_count"], 2)
        output = self.base / "output.bin"
        result = audio.pack_audio(tree, output)
        self.assertTrue(result["byte_identical"])
        self.assertEqual(output.read_bytes(), raw)

    def test_edited_wav_native_decode_flags_and_other_bytes(self):
        raw = bank_fixture()
        tree, manifest = self.unpack(raw)
        sample = manifest["layout"]["samples"][0]
        values = [int(6000 * math.sin(n * .13)) for n in range(sample["pcm_samples"])]
        pcm = struct.pack(f"<{len(values)}h", *values)
        audio_export.write_wav(tree / sample["path"], pcm, 32000, 1)
        output = self.base / "output.bin"
        result = audio.pack_audio(tree, output)
        self.assertEqual(result["changed_samples"], [sample["path"]])
        rebuilt = output.read_bytes()
        start, end = sample["offset"], sample["offset"] + sample["size"]
        self.assertEqual(rebuilt[:start], raw[:start])
        self.assertEqual(rebuilt[end:], raw[end:])
        self.assertEqual(rebuilt[start + 1:end:16], raw[start + 1:end:16])
        decoded, anomalies = decode_sound.decode_vag(rebuilt[start:end], 0, sample["frames"])
        self.assertEqual(anomalies, 0)
        self.assertEqual(decoded, audio_export.decode_adpcm(rebuilt[start:end]))
        restored = struct.unpack(f"<{len(values)}h", decoded)
        rms = math.sqrt(sum((a - b) ** 2 for a, b in zip(values, restored)) / len(values))
        self.assertLess(rms, 150)

    def test_raw_and_vag_header_roundtrip(self):
        raw = raw_clip()
        header = bytearray(48)
        header[:4] = b"VAGp"
        struct.pack_into(">II", header, 12, len(raw), 44100)
        source = self.base / "vag.bin"
        source.write_bytes(bytes(header) + raw + b"tail")
        tree = self.base / "vag"
        manifest = audio.unpack_audio(source, tree)
        self.assertEqual(manifest["layout"]["rate"], 44100)
        output = self.base / "vag-output.bin"
        audio.pack_audio(tree, output)
        self.assertEqual(output.read_bytes(), source.read_bytes())
        tree, _ = self.unpack(raw, "raw")
        audio.pack_audio(tree, self.base / "raw-output.bin")
        self.assertEqual((self.base / "raw-output.bin").read_bytes(), raw)

    def test_changed_duration_rate_channels_and_template_are_rejected(self):
        tree, manifest = self.unpack(raw_clip(), "raw")
        pcm = audio_export.decode_adpcm(raw_clip())
        sample = tree / manifest["layout"]["samples"][0]["path"]
        for content, rate, channels in [(pcm[:-2], 32000, 1), (pcm, 44100, 1), (pcm * 2, 32000, 2)]:
            audio_export.write_wav(sample, content, rate, channels)
            with self.assertRaises(ValueError):
                audio.pack_audio(tree, self.base / "output.bin")
        (tree / "original.bin").write_bytes(bytes(len(raw_clip())))
        with self.assertRaises(ValueError):
            audio.pack_audio(tree, self.base / "output.bin")

    def test_input_alias_and_malformed_frame_are_rejected(self):
        tree, _ = self.unpack(raw_clip(), "raw")
        alias = self.base / "alias.bin"
        os.link(self.base / "source.bin", alias)
        with self.assertRaises(ValueError):
            audio.pack_audio(tree, alias)
        for raw in [b"short", bytes([0xF0]) + bytes(15)]:
            with self.assertRaises(ValueError):
                audio.encode_adpcm(bytes(56), raw)


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1", "set EM_TEST_FULL=1 for real sound banks")
class FullAudioTests(unittest.TestCase):
    def test_real_edited_sample_decodes_and_preserves_other_bytes(self):
        root = ROOT / "build/repack"
        root.mkdir(parents=True, exist_ok=True)
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        self.assertTrue(image.is_file(), "provide the user's disc through EM_TEST_ISO")
        name = "chunk00/f05_id05.bin"
        native = dict(native_archive_leaves(image, {name}))[name]
        with tempfile.TemporaryDirectory(prefix="sound-edit-proof-", dir=root) as directory:
            base = Path(directory)
            source = base / "source.bin"
            source.write_bytes(native)
            manifest = audio.unpack_audio(source, base / "tree", kind="sshd")
            sample = min(manifest["layout"]["samples"], key=lambda item: item["size"])
            wav = base / "tree" / sample["path"]
            silence = bytes(sample["pcm_samples"] * 2)
            audio_export.write_wav(wav, silence, 48000, 1)
            output = base / "edited.bin"
            result = audio.pack_audio(base / "tree", output)
            self.assertEqual(result["changed_samples"], [sample["path"]])
            raw, rebuilt = source.read_bytes(), output.read_bytes()
            start, end = sample["offset"], sample["offset"] + sample["size"]
            self.assertEqual(raw[:start], rebuilt[:start])
            self.assertEqual(raw[end:], rebuilt[end:])
            self.assertEqual(raw[start + 1:end:16], rebuilt[start + 1:end:16])
            pcm, anomalies = decode_sound.decode_vag(rebuilt[start:end], 0, sample["frames"])
            self.assertEqual(anomalies, 0)
            self.assertEqual(pcm, silence)
            redecoded = audio.unpack_audio(output, base / "decoded-again", kind="sshd")
            self.assertEqual(redecoded["layout"], manifest["layout"])
            with wave.open(str(base / "decoded-again" / sample["path"]), "rb") as check:
                self.assertEqual(check.readframes(check.getnframes()), silence)
            (root / "audio-edit-test-receipt.json").write_text(json.dumps({
                "source_image": str(image.resolve()), "source": name, "sample": sample["path"],
                "native_offset": start, "native_size": sample["size"],
                "flags_preserved": True, "all_other_bytes_unchanged": True,
                "existing_decoder_anomalies": anomalies, "edited_bank_reexported": True,
                "edited_pcm_sha256": hashlib.sha256(pcm).hexdigest()}, indent=2) + "\n")

    def test_all_real_sound_containers_roundtrip(self):
        root = ROOT / "build/repack"
        root.mkdir(parents=True, exist_ok=True)
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        self.assertTrue(image.is_file(), "provide the user's disc through EM_TEST_ISO")
        sources = native_archive_leaves(image)
        self.assertEqual(len(sources), 41)
        self.assertIn("chunk01/f00_id06.bin", {name for name, _ in sources})
        records, banks = [], 0
        for name, raw in sources:
            with tempfile.TemporaryDirectory(prefix="sound-proof-", dir=root) as directory:
                base = Path(directory)
                source = base / "source.bin"
                source.write_bytes(raw)
                manifest = audio.unpack_audio(source, base / "tree", kind="sshd")
                banks += len(manifest["layout"]["banks"])
                output = base / "packed.bin"
                result = audio.pack_audio(base / "tree", output)
                self.assertEqual(output.read_bytes(), raw, name)
                records.append({"source": name, "sha256": result["sha256"],
                                "samples": manifest["sample_count"], "byte_identical": True})
        self.assertEqual(banks, 116)
        self.assertEqual(sum(record["samples"] for record in records), 2318)
        (root / "audio-test-receipt.json").write_text(json.dumps(
            {"source_image": str(image.resolve()), "mapping": "resident-correct DATA/INDEX leaves",
             "containers": len(records), "banks": banks, "samples": 2318,
             "records": records}, indent=2) + "\n")


if __name__ == "__main__":
    unittest.main()
