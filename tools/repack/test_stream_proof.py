"""Playback evidence must show live progress and the exact transferred bytes."""
from pathlib import Path
import contextlib
import io
import json
import random
import struct
import tempfile
import unittest

from tools.repack.stream_proof import verify_playback, _matching_half, capture_inputs, main
from tools.repack.test_streams import ARTIFACTS, fixture


class PlaybackTests(unittest.TestCase):
    def setUp(self):
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix='playback-', dir=ARTIFACTS)
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        fixture(self.base)
        irx = bytearray(0x900)
        irx[:6] = b'\x7fELF\x01\x01'
        struct.pack_into('<I', irx, 32, 0x800)
        struct.pack_into('<3H', irx, 46, 40, 3, 2)
        names = b'\0.text\0.shstrtab\0'
        irx[0x780:0x780 + len(names)] = names
        struct.pack_into('<10I', irx, 0x828, 1, 1, 0, 0, 128, 0x700, 0, 0, 4, 0)
        struct.pack_into('<10I', irx, 0x850, 7, 3, 0, 0, 0x780, len(names), 0, 0, 1, 0)
        signature = b'authored signature bytes'
        irx[128 + 0x634:128 + 0x64C] = signature
        self.irx = self.base / 'driver.irx'
        self.irx.write_bytes(irx)
        music = self.base / 'MUSIC.DAT'
        raw = bytearray(music.read_bytes())
        rng = random.Random(3)
        for frame in range(32 * 2048 // 16):
            raw[frame * 16:frame * 16 + 16] = bytes((12, 0)) + rng.randbytes(14)
        music.write_bytes(raw)
        native = raw[:32 * 2048]
        channels = [b''.join(native[i + 1024 * c:i + 1024 * (c + 1)]
                             for i in range(0, len(native), 2048)) for c in (0, 1)]
        spu_offset = 96
        spu = bytearray(spu_offset + 0x200000)
        for voice in (0, 1):
            for half in (0, 1):
                data = bytearray(channels[1 - voice][half * 8192:(half + 1) * 8192])
                data[1], data[-15] = (6, 2) if half == 0 else (2, 3)
                start = spu_offset + 0x5010 + voice * 0x4000 + half * 0x2000
                spu[start:start + 8192] = data
        self.captures = []
        for number in (0, 1):
            iop = bytearray(0x9000)
            iop[0x100 + 0x634:0x100 + 0x64C] = signature
            for voice in range(4):
                start = 0x5010 + voice * 0x4000
                struct.pack_into('<13I', iop, 0x100 + 0x76C0 + voice * 0x34,
                                 int(voice < 2), 0x20000, start, 0x4000, 0, 0x10000,
                                 0x3000, 0x3000, 48000, start + 0x2020 + number * 16, 0, 0, 0)
            staging = spu[spu_offset + 0x7010:spu_offset + 0x9010]
            iop[0x100 + 0x46B0:0x100 + 0x66B0] = staging
            iop_path, spu_path = self.base / f'iop{number}', self.base / f'spu{number}'
            iop_path.write_bytes(iop)
            spu_path.write_bytes(spu)
            self.captures.append(dict(label=str(number), iop_memory=iop_path, spu_state=spu_path,
                                     stream_lanes=[dict(lane=0, cue=1, active=2, voice=0)]))

    def prove(self):
        return verify_playback(self.base / 'MUSIC.DAT', self.base / 'boot.elf', self.irx,
                               self.captures, kind='music', cues=[1])

    def test_active_advancing_exact_playback(self):
        self.assertEqual(len(self.prove()['advancing']), 2)

    def test_shared_bytes_do_not_override_ee_cue_identity(self):
        for capture in self.captures:
            capture['stream_lanes'][0]['cue'] = 2
        with self.assertRaisesRegex(ValueError, 'advancing'):
            self.prove()
        for capture in self.captures:
            del capture['stream_lanes']
        with self.assertRaisesRegex(ValueError, 'EE stream_lanes'):
            self.prove()

    def test_static_cursors_do_not_prove_playback(self):
        self.captures[1]['iop_memory'].write_bytes(self.captures[0]['iop_memory'].read_bytes())
        with self.assertRaisesRegex(ValueError, 'advancing'):
            self.prove()

    def test_stopped_or_one_working_stereo_channel_is_insufficient(self):
        for capture in self.captures:
            raw = bytearray(capture['iop_memory'].read_bytes())
            struct.pack_into('<I', raw, 0x100 + 0x76C0 + 0x34, 0)
            capture['iop_memory'].write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'advancing'):
            self.prove()
        for capture in self.captures:
            raw = bytearray(capture['iop_memory'].read_bytes())
            for voice in range(2):
                struct.pack_into('<I', raw, 0x100 + 0x76C0 + voice * 0x34, 0)
            capture['iop_memory'].write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'advancing'):
            self.prove()

    def test_native_byte_mismatch_cannot_prove_playback(self):
        path = self.base / 'MUSIC.DAT'
        raw = bytearray(path.read_bytes())
        for sector in range(32):
            raw[sector * 2048 + 42] ^= 1
            raw[sector * 2048 + 1024 + 42] ^= 1
        path.write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'advancing'):
            self.prove()

    def test_short_cue_repeats_inside_spu_half(self):
        channel = random.Random(123).randbytes(1024)
        actual = bytearray(channel * 8)
        actual[1], actual[-15] = 6, 2
        self.assertEqual(_matching_half(channel, actual, 0), 0)
        actual[4000] ^= 1
        self.assertIsNone(_matching_half(channel, actual, 0))

    def test_receipt_selection_cli_and_alias_guard(self):
        proof_dir = self.base / 'proof'
        proof_dir.mkdir()
        snapshots = []
        for capture in self.captures:
            directory = proof_dir / capture['label']
            directory.mkdir()
            for old, new in (('iop_memory', 'iopMemory.bin'), ('spu_state', 'SPU2.bin')):
                (directory / new).write_bytes(capture[old].read_bytes())
            snapshots.append(dict(screenshot=str(directory / 'original.png'),
                                  stream_lanes=capture['stream_lanes']))
        receipt = proof_dir / 'proof.json'
        receipt.write_text(json.dumps(dict(snapshots=snapshots)))
        voice_dir = proof_dir / 'voice-route'
        voice_dir.mkdir()
        voice_receipt = voice_dir / 'proof.json'
        voice_receipt.write_text(json.dumps(dict(snapshots=[])))
        self.assertEqual(capture_inputs(proof_dir, 'voice')[0], voice_receipt)
        self.assertEqual(len(capture_inputs(proof_dir, 'music')[1]), 2)
        args = ['--stream', str(self.base / 'MUSIC.DAT'), '--elf', str(self.base / 'boot.elf'),
                '--driver', str(self.irx), '--proof', str(proof_dir), '--kind', 'music', '--cue', '1']
        output = self.base / 'result.json'
        with contextlib.redirect_stdout(io.StringIO()) as stdout:
            main(args + ['--out', str(output)])
        self.assertTrue(json.loads(stdout.getvalue())['verified'])
        self.assertTrue(json.loads(output.read_text())['cue_identity_verified'])
        with self.assertRaisesRegex(ValueError, 'aliases'):
            main(args + ['--out', str(receipt)])


if __name__ == '__main__':
    unittest.main()
