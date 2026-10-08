"""Synthetic delta and whole-original provenance gate regression tests."""
from __future__ import annotations

import hashlib
from pathlib import Path
import random
import struct
import tempfile
import unittest

from tools.repack import delta
from tools.repack.original_scan import _payload_batches, scan_original_runs

ROOT = Path(__file__).resolve().parents[2]
ARTIFACTS = ROOT / 'build/repack/delta-tests'


def synthetic(seed, size):
    return random.Random(seed).randbytes(size)


def instruction(code, offset, length, payload=b''):
    return bytes((code,)) + (delta.LENGTH.pack(length) if code == delta.INSERT
                            else delta.REFERENCE.pack(offset, length)) + payload


def authored_delta(base, edited, operations):
    return (delta.HEADER.pack(delta.MAGIC, len(base), len(edited), hashlib.sha256(base).digest(),
                              hashlib.sha256(edited).digest(), len(operations)) + b''.join(operations))


class DeltaTests(unittest.TestCase):
    def roundtrip(self, base, edited, **kwargs):
        encoded = delta.encode(base, edited, **kwargs)
        self.assertEqual(encoded, delta.encode(base, edited, **kwargs))
        self.assertEqual(delta.apply(base, encoded), edited)
        self.assertEqual(delta.validate(encoded)['edited_size'], len(edited))
        return encoded

    def test_empty_noop_growth_shrink(self):
        for base, edited in ((b'', b''), (b'', b'authored edit'), (b'original fixture', b''),
                             (b'abc', b'abc'), (synthetic(1, 150000), synthetic(1, 150000))):
            with self.subTest(original=len(base), edited=len(edited)):
                encoded = self.roundtrip(base, edited)
                if base == edited:
                    self.assertEqual(delta.distributable_payloads(encoded), ())
                    self.assertEqual(len(encoded), delta.HEADER.size + (17 if base else 0))

    def test_single_byte_is_only_xor_mask(self):
        base = synthetic(2, 170000)
        changed = bytearray(base)
        changed[77000] ^= 0x59
        encoded = self.roundtrip(base, bytes(changed))
        self.assertEqual([bytes(p) for p in delta.distributable_payloads(encoded)], [b'Y'])
        self.assertEqual([encoded[start:end] for start, end in delta.distributable_payload_ranges(encoded)], [b'Y'])
        self.assertEqual(delta.validate(encoded)['operation_count'], 3)

    def test_insert_delete_and_relocate_large_runs(self):
        base = synthetic(3, 18000)
        edited = b'inserted prefix!' + base[8191:10003] + b'new material' + base[101:8120] + base[10009:]
        encoded = self.roundtrip(base, edited)
        self.assertLess(len(encoded), 350)
        inserted = self.roundtrip(base, base[:3500] + b'new middle' + base[3500:])
        self.assertLess(len(inserted), 180)
        deleted = self.roundtrip(base, base[:3500] + base[4100:])
        self.assertLess(len(deleted), 180)

    def test_unaligned_shared_65_byte_runs_are_references(self):
        base = synthetic(4, 500)
        for offset in range(32):
            edited = b'!' + base[96 + offset:161 + offset] + b'?'
            encoded = self.roundtrip(base, edited, prefer_xor=False)
            self.assertLess(sum(map(len, delta.distributable_payloads(encoded))), 34)

    def test_deterministic_random_edits(self):
        rng = random.Random(9)
        for case in range(120):
            base = rng.randbytes(rng.randrange(0, 3000))
            edited = base
            for _ in range(6):
                start = rng.randrange(len(edited) + 1)
                end = rng.randrange(start, len(edited) + 1)
                replacement = rng.randbytes(rng.randrange(100)) if rng.randrange(2) else base[:rng.randrange(len(base) + 1)]
                edited = edited[:start] + replacement + edited[end:]
            with self.subTest(case=case):
                self.roundtrip(base, edited)

    def test_authentication_and_output_limit(self):
        base, edited = b'a' * 100, b'b' * 100
        encoded = self.roundtrip(base, edited)
        with self.assertRaisesRegex(delta.DeltaError, 'original size/hash'):
            delta.apply(b'c' * 100, encoded)
        corrupt = encoded[:-1] + bytes((encoded[-1] ^ 1,))
        with self.assertRaisesRegex(delta.DeltaError, 'output hash'):
            delta.apply(base, corrupt)
        with self.assertRaisesRegex(delta.DeltaError, 'allowed limit'):
            delta.apply(base, encoded, max_output_size=99)
        self.assertEqual(delta.apply(base, encoded, max_output_size=100), edited)
        for limit in (-1, True, 1.5, '100', 1 << 32):
            with self.assertRaises(delta.DeltaError):
                delta.validate(encoded, max_output_size=limit)

    def test_strict_binary_grammar(self):
        base, edited = b'abcdefgh', b'12345678'
        cases = [authored_delta(base, edited, [instruction(99, 0, 8)]),
                 authored_delta(base, edited, [instruction(delta.COPY, 1, 8)]),
                 authored_delta(base, edited, [instruction(delta.COPY, 0, 9)]),
                 authored_delta(base, edited, [instruction(delta.COPY, 0, 0)]),
                 authored_delta(base, edited, [instruction(delta.XOR, 0, 8, bytes(8))]),
                 authored_delta(base, edited, [instruction(delta.INSERT, 0, 4, edited[:4]),
                                              instruction(delta.INSERT, 0, 4, edited[4:])]),
                 authored_delta(base, base, [instruction(delta.COPY, 0, 4), instruction(delta.COPY, 4, 4)]),
                 authored_delta(base, edited, [instruction(delta.XOR, 0, 4, b'xxxx'),
                                              instruction(delta.XOR, 4, 4, b'yyyy')])]
        valid = delta.encode(base, edited)
        cases.extend(valid[:end] for end in range(len(valid)))
        cases.extend((b'x' + valid[1:], valid + b'comments are forbidden'))
        too_many = bytearray(valid)
        struct.pack_into('<I', too_many, delta.HEADER.size - 4, 0xFFFFFFFF)
        cases.append(bytes(too_many))
        for number, raw in enumerate(cases):
            with self.subTest(case=number):
                with self.assertRaises(delta.DeltaError):
                    delta.validate(raw)
                with self.assertRaises(delta.DeltaError):
                    delta.distributable_payloads(raw)
                with self.assertRaises(delta.DeltaError):
                    delta.distributable_payload_ranges(raw)


class OriginalRunTests(unittest.TestCase):
    def setUp(self):
        ARTIFACTS.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix='scan-', dir=ARTIFACTS)
        self.addCleanup(self.temp.cleanup)
        self.original = Path(self.temp.name) / 'original.fixture'

    def scan(self, original, payloads, **kwargs):
        self.original.write_bytes(original)
        return scan_original_runs(self.original, payloads, **kwargs)

    def test_threshold_and_complete_original_scope(self):
        original = synthetic(20, 1100)
        self.assertIsNone(self.scan(original, [original[400:464]]))
        match = self.scan(original, [b'authored prefix' + original[801:866]])
        self.assertEqual(match, dict(original_offset=801, payload_offset=15, length=65))
        # The edited file's original is a different region of the same disc.
        base = original[:200]
        edited = original[801:866]
        malformed_distribution = authored_delta(base, edited, [instruction(delta.INSERT, 0, 65, edited)])
        self.assertEqual(delta.apply(base, malformed_distribution), edited)
        self.assertIsNotNone(self.scan(original, delta.distributable_payloads(malformed_distribution)))

    def test_operation_and_member_boundaries(self):
        original = synthetic(21, 1000)
        copied = original[377:442]
        encoded = authored_delta(b'x', copied[:31] + b'x' + copied[31:],
                                 [instruction(delta.INSERT, 0, 31, copied[:31]),
                                  instruction(delta.COPY, 0, 1),
                                  instruction(delta.INSERT, 0, 34, copied[31:])])
        self.assertIsNotNone(self.scan(original, delta.distributable_payloads(encoded)))
        self.assertIsNotNone(self.scan(original, [copied[:10], b'', copied[10:30], copied[30:]]))
        # FULL member followed by a different member's XOR payload.
        xor_delta = authored_delta(bytes(34), copied[31:],
                                   [instruction(delta.XOR, 0, 34, copied[31:])])
        self.assertIsNotNone(self.scan(original, [copied[:31], *delta.distributable_payloads(xor_delta)]))

    def test_source_and_payload_batch_boundaries(self):
        original = synthetic(22, 1400)
        for source_at in (1, 64, 65, 66, 110, 128, 130, 1335):
            for payload_at in (0, 1, 32, 64, 65, 66, 129):
                payload = synthetic(23, payload_at) + original[source_at:source_at + 65] + b'z'
                match = self.scan(original, [payload], batch_size=65, read_size=65)
                self.assertIsNotNone(match)
                self.assertEqual(match['original_offset'], source_at)
                self.assertEqual(match['payload_offset'], payload_at)

    def test_against_bruteforce_oracle_and_random_member_splits(self):
        rng = random.Random(24)
        for case in range(70):
            original = rng.randbytes(300)
            payload = rng.randbytes(240)
            if case % 2:
                src, dst = rng.randrange(235), rng.randrange(175)
                payload = payload[:dst] + original[src:src + 65] + payload[dst + 65:]
            expected = any(payload[p:p + 65] in original for p in range(len(payload) - 64))
            parts = [payload[:21], payload[21:102], payload[102:]]
            self.assertEqual(self.scan(original, parts, batch_size=100, read_size=100) is not None, expected)

    def test_batch_windows_cover_concatenated_payload_exactly(self):
        payload = synthetic(25, 401)
        starts = set()
        for start, batch in _payload_batches([payload[:25], payload[25:83], payload[83:]], 65):
            self.assertEqual(batch, payload[start:start + len(batch)])
            starts.update(range(start, start + len(batch) - 64))
        self.assertEqual(starts, set(range(len(payload) - 64)))

    def test_invalid_scan_parameters(self):
        self.original.write_bytes(b'fixture')
        for kwargs in ({'batch_size': 64}, {'batch_size': 4 * 1024 * 1024 + 1},
                       {'read_size': 64}, {'read_size': True}):
            with self.assertRaises(ValueError):
                scan_original_runs(self.original, [b'authored'], **kwargs)
        with self.assertRaises(TypeError):
            scan_original_runs(self.original, ['not bytes'])


if __name__ == '__main__':
    unittest.main()
