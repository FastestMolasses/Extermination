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
ARTIFACTS = ROOT / "build/repack/audio-growth"


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
        struct.pack_into("<I", out, header + 16, 0xFFFFFFFF)
        struct.pack_into("<I", out, header + 36, 0xFFFFFFFF)
    return bytes(out) + b"".join(bodies) + b"preserved trailing bytes"


def tone_bank_fixture():
    bodies = [raw_clip(4) + bytes((12, 7)) + bytes(14) + raw_clip(5), raw_clip(3)]
    out = bytearray(0x180)
    total = len(out) + sum(map(len, bodies))
    struct.pack_into("<8I", out, 0, total, 0x40, 0, 2, len(out), total - len(out), len(out), 0)
    for i, (header, body) in enumerate(zip((0x40, 0xE0), bodies)):
        struct.pack_into("<4I", out, 32 + i * 16, len(body), header, 1, 0)
        struct.pack_into("<3I4s", out, header, 0xA0, len(body), 0, b"SShd")
        struct.pack_into("<6I", out, header + 16, 0x30, 0xFFFFFFFF, 0xFFFFFFFF,
                         0xFFFFFFFF, 0xFFFFFFFF, 0x30)
        region = header + 0x30
        struct.pack_into("<HH", out, region, 0, 4)
        out[region + 4:region + 12] = bytes((0xFF, 100, 64, 0, 12, 0, 0, 1 if i == 0 else 0))
        struct.pack_into("<H", out, region + 12 + 4, 0)
        if i == 0:
            struct.pack_into("<H", out, region + 28 + 4, 80 >> 3)
    return bytes(out) + b"".join(bodies) + b"unchanged trailer"


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
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="audio-test-", dir=ARTIFACTS)
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

    def test_changed_rate_channels_empty_and_template_are_rejected(self):
        tree, manifest = self.unpack(raw_clip(), "raw")
        pcm = audio_export.decode_adpcm(raw_clip())
        sample = tree / manifest["layout"]["samples"][0]["path"]
        for content, rate, channels in [(b"", 32000, 1), (pcm, 44100, 1), (pcm * 2, 32000, 2)]:
            audio_export.write_wav(sample, content, rate, channels)
            with self.assertRaises(ValueError):
                audio.pack_audio(tree, self.base / "output.bin")
        (tree / "original.bin").write_bytes(bytes(len(raw_clip())))
        with self.assertRaises(ValueError):
            audio.pack_audio(tree, self.base / "output.bin")

    def test_bank_growth_and_shrink_relocate_tones_sizes_and_following_bank(self):
        for frames in (7, 2):
            with self.subTest(frames=frames):
                native = tone_bank_fixture()
                source = self.base / f"source-{frames}.bin"
                source.write_bytes(native)
                tree = self.base / f"tree-{frames}"
                manifest = audio.unpack_audio(source, tree, rate=32000)
                sample = manifest["layout"]["samples"][0]
                audio_export.write_wav(tree / sample["path"], bytes(frames * 28 * 2), 32000, 1)
                output = self.base / f"output-{frames}.bin"
                receipt = audio.pack_audio(tree, output)
                rebuilt = output.read_bytes()
                delta = (frames - 4) * 16
                self.assertEqual(len(rebuilt), len(native) + delta)
                self.assertEqual(struct.unpack_from("<I", rebuilt, 0)[0], struct.unpack_from("<I", native, 0)[0] + delta)
                self.assertEqual(struct.unpack_from("<I", rebuilt, 20)[0], struct.unpack_from("<I", native, 20)[0] + delta)
                self.assertEqual(struct.unpack_from("<I", rebuilt, 32)[0], 160 + delta)
                self.assertEqual(struct.unpack_from("<I", rebuilt, 0x44)[0], 160 + delta)
                self.assertEqual(struct.unpack_from("<H", rebuilt, 0x90)[0] << 3, 80 + delta)
                self.assertEqual(rebuilt[0x180 + frames * 16:0x180 + frames * 16 + 16], native[0x1C0:0x1D0])
                self.assertEqual(rebuilt[0x180 + 160 + delta:], native[0x180 + 160:])
                self.assertEqual(receipt["tone_updates"][0]["relocated"], 80 + delta)
                decoded = audio.unpack_audio(output, self.base / f"decoded-{frames}", rate=32000)
                self.assertEqual(decoded["sample_count"], 3)
                self.assertEqual(decoded["layout"]["samples"][0]["frames"], frames)

    def test_equal_total_size_still_relocates_tone_references(self):
        tree, manifest = self.unpack(tone_bank_fixture())
        for sample, frames in zip(manifest["layout"]["samples"][:2], (6, 3)):
            audio_export.write_wav(tree / sample["path"], bytes(frames * 56), 32000, 1)
        output = self.base / "balanced.bin"
        result = audio.pack_audio(tree, output)
        self.assertEqual(result["size"], len(tone_bank_fixture()))
        self.assertEqual(struct.unpack_from("<H", output.read_bytes(), 0x90)[0] << 3, 112)

    def test_duration_rounding_and_loop_start_validation(self):
        tree, manifest = self.unpack(raw_clip(), "raw")
        sample = manifest["layout"]["samples"][0]
        audio_export.write_wav(tree / sample["path"], bytes(29 * 2), 32000, 1)
        loops = {sample["path"]: 3}
        (tree / "loops.json").write_text(json.dumps(loops))
        with self.assertRaisesRegex(ValueError, "loop start"):
            audio.pack_audio(tree, self.base / "bad-loop.bin")
        loops[sample["path"]] = 0
        (tree / "loops.json").write_text(json.dumps(loops))
        output = self.base / "rounded.bin"
        result = audio.pack_audio(tree, output)
        self.assertEqual(result["size"], 32)
        self.assertEqual(result["sample_changes"][0]["padded_pcm_samples"], 27)
        self.assertEqual(output.read_bytes()[1::16], bytes((6, 3)))

    def test_spu_partition_and_single_bank_bounds(self):
        with self.assertRaisesRegex(ValueError, "capacity"):
            audio._check_bank_capacity([dict(type=3, size=16)], [0x10010])
        with self.assertRaisesRegex(ValueError, "capacity"):
            audio._check_bank_capacity([dict(type=1, size=16)], [0x80010])

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
        root = ARTIFACTS
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
        root = ARTIFACTS
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

    def test_real_bank_sample_growth_shrink_and_original_extractor(self):
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        name = "chunk00/f05_id05.bin"
        native = dict(native_archive_leaves(image, {name}))[name]
        results = []
        for delta in (16, -16):
            with tempfile.TemporaryDirectory(prefix="real-resize-", dir=ARTIFACTS) as directory:
                base = Path(directory)
                source = base / "source.bin"
                source.write_bytes(native)
                tree = base / "tree"
                manifest = audio.unpack_audio(source, tree, kind="sshd")
                sample = next(row for row in manifest["layout"]["samples"] if row["frames"] >= 3)
                new_frames = sample["frames"] + delta // 16
                loops = json.loads((tree / "loops.json").read_text())
                loops[sample["path"]] = 0
                (tree / "loops.json").write_text(json.dumps(loops))
                audio_export.write_wav(tree / sample["path"], bytes(new_frames * 56), 48000, 1)
                output = base / "edited.bin"
                result = audio.pack_audio(tree, output)
                rebuilt = output.read_bytes()
                self.assertEqual(len(rebuilt), len(native) + delta)
                before, after = audio_export.parse_container(native), audio_export.parse_container(rebuilt)
                index = sample["bank"]
                start = sample["offset"] - before["banks"][index]["body_base"]
                for i, (old_bank, new_bank) in enumerate(zip(before["banks"], after["banks"])):
                    self.assertEqual(new_bank["body_size"], old_bank["body_size"] + (delta if i == index else 0))
                    old_programs = audio_export.parse_programs(native, old_bank["hd"])
                    new_programs = audio_export.parse_programs(rebuilt, new_bank["hd"])
                    self.assertEqual(len(old_programs), len(new_programs))
                    for old_program, new_program in zip(old_programs, new_programs):
                        for old_tone, new_tone in zip(old_program["tones"], new_program["tones"]):
                            expected = old_tone["samp_off"]
                            if i == index and expected >= start + sample["size"]:
                                expected += delta
                            self.assertEqual(new_tone["samp_off"], expected)
                redecoded = audio.unpack_audio(output, base / "readback", kind="sshd")
                self.assertEqual(redecoded["sample_count"], manifest["sample_count"])
                new_sample = next(row for row in redecoded["layout"]["samples"] if row["path"] == sample["path"])
                pcm, anomalies = decode_sound.decode_vag(rebuilt, new_sample["offset"] // 16, new_frames)
                self.assertEqual(anomalies, 0)
                self.assertEqual(pcm, bytes(new_frames * 56))
                results.append(dict(delta=delta, source=name, sample=sample["path"], new_frames=new_frames,
                                    changed_tone_fields=len(result["tone_updates"]), extractor_readback=True,
                                    output_sha256=result["sha256"], spu_allocations=result["spu_allocations"]))
        (ARTIFACTS / "duration-proof.json").write_text(json.dumps({"source_image": str(image), "cases": results}, indent=2) + "\n")


if __name__ == "__main__":
    unittest.main()
