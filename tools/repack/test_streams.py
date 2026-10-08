"""Sector/cue relocation and independent ADPCM read-back proofs."""
from __future__ import annotations

import json
import math
import os
from pathlib import Path
import struct
import tempfile
import unittest
import wave

from tools import audio_export
from tools.repack import streams, iso, archive

ROOT = Path(__file__).resolve().parents[2]
ARTIFACTS = ROOT / 'build/repack/streams'


def fixture(base):
    elf = bytearray(0x10080)
    elf[:6] = b'\x7fELF\x01\x01'
    struct.pack_into('<I', elf, 28, 52)
    struct.pack_into('<HH', elf, 42, 32, 1)
    struct.pack_into('<8I', elf, 52, 1, 128, 0x250000, 0x250000, 0x10000, 0x10000, 7, 16)
    for kind, profile in streams.PROFILES.items():
        raw, cursor = bytearray(), 0
        at = 128 + profile['vram'] - 0x250000
        for cue in range(1, profile['count']):
            size = (32 if cue == 1 else 1) * 2048
            struct.pack_into('<4I', elf, at + cue * 16, cursor // 2048, cursor, size,
                             int(kind == 'music' and cue == 1))
            raw.extend((bytes((12, 0)) + bytes(14)) * (size // 16))
            cursor += size
        (base / profile['filename']).write_bytes(raw)
    (base / 'boot.elf').write_bytes(elf)


def sine_wav(path, channels, samples=3000):
    values = [int(7000 * math.sin(n * (.051 if c == 0 else .083)))
              for n in range(samples) for c in range(channels)]
    pcm = struct.pack(f'<{len(values)}h', *values)
    audio_export.write_wav(path, pcm, 48000, channels)
    return pcm


class StreamTests(unittest.TestCase):
    def setUp(self):
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix='test-', dir=ARTIFACTS)
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        fixture(self.base)

    def unpack(self, kind):
        tree = self.base / (kind + '-tree')
        streams.unpack_stream(self.base / streams.PROFILES[kind]['filename'], self.base / 'boot.elf',
                              tree, kind=kind, cues=[1])
        return tree

    def test_noop_stereo_mono_and_cue_patch_are_identical(self):
        bundles = []
        for kind, profile in streams.PROFILES.items():
            tree = self.unpack(kind)
            bundle = self.base / (kind + '-bundle')
            result = streams.pack_stream(tree, bundle)
            self.assertTrue(result['byte_identical'])
            self.assertEqual((bundle / profile['filename']).read_bytes(),
                             (self.base / profile['filename']).read_bytes())
            bundles.append(bundle)
        result = streams.apply_cue_patches(self.base / 'boot.elf', bundles, self.base / 'new.elf')
        self.assertTrue(result['byte_identical'])

    def test_resized_stereo_mono_cues_and_independent_readback(self):
        bundles = []
        for kind, profile in streams.PROFILES.items():
            tree = self.unpack(kind)
            pcm = sine_wav(tree / 'cue_001.wav', profile['channels'])
            bundle = self.base / (kind + '-bundle')
            result = streams.pack_stream(tree, bundle)
            rows = result['rows']
            self.assertLess(rows[1][2], 32 * 2048)
            self.assertEqual(rows[2][:2], [rows[1][2] // 2048, rows[1][2]])
            native = (bundle / profile['filename']).read_bytes()[:rows[1][2]]
            if profile['channels'] == 2:
                left = b''.join(native[i:i + 1024] for i in range(0, len(native), 2048))
                right = b''.join(native[i + 1024:i + 2048] for i in range(0, len(native), 2048))
                self.assertEqual(left[0] >> 4, 0)
                self.assertEqual(right[0] >> 4, 0)
                decoded = audio_export.interleave_pcm(audio_export.decode_adpcm(left), audio_export.decode_adpcm(right))
            else:
                decoded = audio_export.decode_adpcm(native)
                self.assertEqual(native[0] >> 4, 0)
            values = struct.unpack(f'<{len(pcm) // 2}h', pcm)
            actual = struct.unpack(f'<{len(pcm) // 2}h', decoded[:len(pcm)])
            rms = math.sqrt(sum((a - b) ** 2 for a, b in zip(values, actual)) / len(values))
            self.assertLess(rms, 150)
            bundles.append(bundle)
        out = self.base / 'new.elf'
        result = streams.apply_cue_patches(self.base / 'boot.elf', bundles, out)
        old, new = (self.base / 'boot.elf').read_bytes(), out.read_bytes()
        allowed = set()
        for kind, profile in streams.PROFILES.items():
            at = streams._table_offset(old, kind)
            for cue in range(1, profile['count']):
                allowed.update(range(at + cue * 16, at + cue * 16 + 12))
        self.assertTrue(result['changed_rows'])
        self.assertTrue(all(i in allowed for i, (a, b) in enumerate(zip(old, new)) if a != b))

    def test_invalid_metadata_input_and_output_alias_rejected(self):
        tree = self.unpack('music')
        with self.assertRaises(ValueError):
            streams.pack_stream(tree, tree / 'bundle')
        bundle = self.base / 'bundle'
        streams.pack_stream(tree, bundle)
        metadata = bundle / 'cue-edits.json'
        patch = json.loads(metadata.read_text())
        patch['rows'][1][3] ^= 1
        metadata.write_text(json.dumps(patch))
        with self.assertRaisesRegex(ValueError, 'loop flags'):
            streams.inspect_bundle(bundle, self.base / 'boot.elf')
        patch['rows'][1][3] ^= 1
        patch['rows'][2][1] += 2048
        metadata.write_text(json.dumps(patch))
        with self.assertRaisesRegex(ValueError, 'tile'):
            streams.inspect_bundle(bundle, self.base / 'boot.elf')
        patch['rows'][2][1] -= 2048
        patch['rows'][1][2] = 0x80000000
        with self.assertRaisesRegex(ValueError, 'tile'):
            streams._validate_rows(patch['rows'], 'music', 0x80000000)

    def test_duration_grows_and_rate_channels_remain_fixed(self):
        tree = self.unpack('voice')
        sine_wav(tree / 'cue_001.wav', 1, 32 * 3584 + 1)
        result = streams.pack_stream(tree, self.base / 'grown')
        request = 32 * 3584 + 1
        required = ((request + 799) // 800 + 31) * 800
        self.assertEqual(result['rows'][1][2], ((required + 3583) // 3584) * 2048)
        self.assertGreaterEqual(result['changed_cues'][0]['padded_pcm_samples'], 31 * 800)
        with wave.open(str(tree / 'cue_001.wav'), 'wb') as wav:
            wav.setnchannels(2)
            wav.setsampwidth(2)
            wav.setframerate(48000)
            wav.writeframes(bytes(400))
        with self.assertRaisesRegex(ValueError, 'channel count'):
            streams.pack_stream(tree, self.base / 'bad')

    def test_one_shot_tail_accounts_for_original_early_stop_timer(self):
        for kind in ('music', 'voice'):
            channels = streams.PROFILES[kind]['channels']
            pcm = bytes(800 * channels * 2)
            native, padding = streams._encode(pcm, kind)
            self.assertGreaterEqual(padding, 31 * 800)
            sectors = len(native) // 2048
            f32 = lambda value: struct.unpack('<f', struct.pack('<f', value))[0]
            seconds = f32(f32(.074666664) * sectors)
            if kind == 'music':
                seconds = f32(seconds / 2)
            timer = int(f32(f32(60 * seconds) - 30))
            self.assertGreaterEqual(timer, 1)
            if kind == 'music':
                looped, loop_padding = streams._encode(pcm, kind, loop=True)
                self.assertLess(len(looped), len(native))
                self.assertEqual(len(looped), 2048)


@unittest.skipUnless(os.environ.get('EM_TEST_FULL') == '1', 'set EM_TEST_FULL=1 for original disc stream proof')
class FullStreamTests(unittest.TestCase):
    def test_original_streams_roundtrip_and_edited_extractor_cue_tables(self):
        self.assertTrue(os.environ.get('EM_TEST_ISO'), 'provide the original user disc through EM_TEST_ISO')
        image = Path(os.environ['EM_TEST_ISO'])
        entries = {entry['path']: entry for entry in iso.inventory(image)['files']}
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='real-', dir=ARTIFACTS) as temp:
            base = Path(temp)
            with image.open('rb') as disc:
                for name in ('SCUS_971.12', 'STREAM/MUSIC.DAT', 'STREAM/VOICE.DAT'):
                    entry = entries[name]
                    disc.seek(entry['offset'])
                    with (base / Path(name).name).open('wb') as out:
                        remaining = entry['size']
                        while remaining:
                            block = disc.read(min(remaining, 1024 * 1024))
                            self.assertTrue(block)
                            out.write(block)
                            remaining -= len(block)
            elf = base / 'SCUS_971.12'
            bundles, report = [], {}
            for kind, cue in (('music', 63), ('voice', 143)):
                profile = streams.PROFILES[kind]
                native = base / profile['filename']
                tree = base / (kind + '-tree')
                streams.unpack_stream(native, elf, tree, kind=kind, cues=[cue])
                noop = base / (kind + '-noop')
                exact = streams.pack_stream(tree, noop)
                self.assertEqual(exact['stream_sha256'], archive.sha256_file(native))
                (noop / profile['filename']).unlink()
                pcm = sine_wav(tree / f'cue_{cue:03d}.wav', profile['channels'])
                bundle = base / (kind + '-edited')
                result = streams.pack_stream(tree, bundle)
                self.assertEqual([entry['cue'] for entry in result['changed_cues']], [cue])
                self.assertLess(result['rows'][cue][2], exact['rows'][cue][2])
                streams.inspect_bundle(bundle, elf)
                with native.open('rb') as original, Path(result['stream_path']).open('rb') as edited:
                    for i, (old, new) in enumerate(zip(exact['rows'], result['rows'])):
                        original.seek(old[1])
                        edited.seek(new[1])
                        if i != cue:
                            self.assertEqual(original.read(old[2]), edited.read(new[2]))
                        else:
                            decoded = streams._decode(edited.read(new[2]), profile['channels'])
                            expected = struct.unpack(f'<{len(pcm) // 2}h', pcm)
                            actual = struct.unpack(f'<{len(pcm) // 2}h', decoded[:len(pcm)])
                            rms = math.sqrt(sum((a - b) ** 2 for a, b in zip(expected, actual)) / len(actual))
                            self.assertLess(rms, 150)
                bundles.append(bundle)
                report[kind] = dict(original_sha256=exact['stream_sha256'], edited_sha256=result['stream_sha256'],
                                    changed_cues=result['changed_cues'], decoder_rms=rms, rows=result['rows'])
            patched = base / 'edited.elf'
            patch = streams.apply_cue_patches(elf, bundles, patched)
            for kind in report:
                self.assertEqual([list(row) for row in audio_export.read_cue_table(patched, kind == 'music')],
                                 report[kind].pop('rows'))
            report['patch'] = dict(original_elf_sha256=patch['original_elf_sha256'], changed_rows=patch['changed_rows'])
            (ARTIFACTS / 'test-receipt.json').write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    unittest.main()
