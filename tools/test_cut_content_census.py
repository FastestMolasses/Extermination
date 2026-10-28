#!/usr/bin/env python3
"""Synthetic call-site fixtures; no game data, filesystem output or emulator."""
import json
import struct
import unittest

from cut_content_census import LANE_START, constant_step, jal_targets, lane_calls


def immediate(op, rt, value, rs=0):
    return (op << 26) | (rs << 21) | (rt << 16) | (value & 65535)


def packed_copy(rd, rs=0):
    return (28 << 26) | (rs << 21) | (rd << 11) | (24 << 6) | 40


def call(target=0x1FA790):
    return (3 << 26) | (target >> 2)


def fixture(*words, image="fixture"):
    return lane_calls(struct.pack("<" + "I" * len(words), *words), 0x100000, image)


class LaneCallsTest(unittest.TestCase):
    def test_lane_constants_and_delay_slot(self):
        self.assertEqual(fixture(immediate(9, 4, 1), call(), 0)[0]["lane_argument"], 1)
        self.assertEqual(fixture(immediate(9, 4, 1), call(), immediate(9, 4, 2))[0]["lane_argument"], 2)
        self.assertEqual(fixture(packed_copy(4), call(), 0)[0]["lane_argument"], 0)

    def test_copies_and_wrapper_enable_are_json_values(self):
        rows = fixture(immediate(9, 8, 0), packed_copy(4, 8), call(0x1FABF0), immediate(9, 7, 1))
        row = json.loads(json.dumps(rows))[0]
        self.assertEqual((row["lane_argument"], row["enable_argument"]), (0, 1))
        forwarded = fixture(call(0x1FABF0), packed_copy(7))[0]
        self.assertEqual((forwarded["lane_argument"], forwarded["enable_argument"]), ("unknown", 0))

    def test_loads_and_unsupported_operations_do_not_invent_constants(self):
        for word in (immediate(35, 4, 0, 8), (28 << 26) | 63):
            self.assertEqual(fixture(immediate(9, 4, 1), word, call(), 0)[0]["lane_argument"], "unknown")

    def test_call_clobbers_and_register_zero(self):
        row = fixture(immediate(9, 4, 2), call(0x123450), 0, call(), 0)[0]
        self.assertEqual(row["lane_argument"], "unknown")
        self.assertEqual(fixture(immediate(9, 0, 17), packed_copy(4), call(), 0)[0]["lane_argument"], 0)

    def test_branch_target_does_not_inherit_a_fallthrough_constant(self):
        # Unknown branch can skip the constant assignment and enter at the call.
        for op in (4, 20):  # ordinary and likely branches
            rows = fixture(immediate(op, 9, 2, 8), 0, immediate(9, 4, 2), call(), 0)
            self.assertEqual(rows[0]["lane_argument"], "unknown")

    def test_truncated_and_nested_calls_are_unknown_and_all_are_counted(self):
        words = [call(), call(0x1FABF0), 0, call()]
        rows = fixture(*words)
        blob = struct.pack("<4I", *words)
        self.assertEqual(len(rows), len(jal_targets(blob, 0x100000, LANE_START)))
        self.assertTrue(all(r["lane_argument"] == "unknown" for r in rows))

    def test_overlay_identity_and_unrelated_targets(self):
        a = fixture(call(), packed_copy(4), image="AREA_A")[0]
        b = fixture(call(), packed_copy(4), image="AREA_B")[0]
        self.assertEqual(a["address"], b["address"])
        self.assertNotEqual(a["image"], b["image"])
        self.assertEqual(fixture(call(0x1FD4C0), 0), [])

    def test_word_sign_extension_and_saturating_byte_add(self):
        registers = [0] + [None] * 31
        constant_step(immediate(9, 4, -1), registers)
        self.assertEqual(registers[4], (1 << 64) - 1)
        registers[8], registers[9] = 250, 10
        constant_step((28 << 26) | (8 << 21) | (9 << 16) | (4 << 11) | (24 << 6) | 40, registers)
        self.assertEqual(registers[4], 255)


if __name__ == "__main__":
    unittest.main()
