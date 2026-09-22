#!/usr/bin/env python3
"""Disc-independent regressions for recovered opening animation semantics."""
import math
import unittest

from export_opening_actors import Channel, OpeningClip, quaternion_matrix


class OpeningAnimationTests(unittest.TestCase):
    def test_original_quaternion_blend_does_not_normalize(self):
        q = Channel([(0, (0, 0, 0, 1), False),
                     (2, (0, 1, 0, 0), False),
                     (65535, (0, 1, 0, 0), False)], True)
        q.advance()
        q.advance()
        self.assertEqual(q.quaternion_value(), (0, 0.5, 0, 0.5))
        self.assertAlmostEqual(math.dist(q.quaternion_value(), (0, 0, 0, 0)),
                               math.sqrt(0.5))
        # The unnormalized pair scales these basis vectors. A normalization
        # step would change both diagonal values from 0.5 to zero.
        matrix = quaternion_matrix(q.quaternion_value(), (1, 2, 3), (1, 1, 1))
        self.assertEqual(matrix[0], (0.5, 0, 0.5, 0))
        self.assertEqual(matrix[2], (-0.5, 0, 0.5, 0))
        self.assertEqual(matrix[3], (1, 2, 3, 1))

    def test_negative_hemisphere_negates_earlier_sample(self):
        q = Channel([(0, (0, 0, 0, 1), False),
                     (2, (0, 0, 0, -1), False),
                     (65535, (0, 0, 0, -1), False)], True)
        q.advance()
        self.assertEqual(q.quaternion_value(), (0, 0, 0, -1))

    def test_scale_hold_stops_other_channels_until_their_next_key(self):
        clip = OpeningClip.__new__(OpeningClip)
        clip.rotation = [Channel([(0, (0, 0, 0, 1), False),
                                  (4, (0, 1, 0, 0), False),
                                  (8, (1, 0, 0, 0), False)], True)]
        clip.translation = [Channel([(0, (0, 0, 0), False),
                                     (4, (8, 0, 0), False),
                                     (8, (16, 0, 0), False)])]
        clip.scale = [Channel([(0, (1, 1, 1), False),
                               (1, (1, 1, 1), True),
                               (2, (1, 1, 1), False),
                               (8, (1, 1, 1), False)])]
        for _ in range(2):
            clip.advance()
        self.assertEqual(clip.translation[0].value, (2, 0, 0))
        self.assertEqual(clip.rotation[0].blend, 0.25)
        for _ in range(5):
            clip.advance()
        # Clearing the scale flag at2 does not restore the other velocities.
        self.assertEqual(clip.translation[0].value, (2, 0, 0))
        self.assertEqual(clip.rotation[0].blend, 0.25)
        clip.advance()  # time4: original next-key branch resets the channels
        self.assertEqual(clip.translation[0].value, (8, 0, 0))
        self.assertEqual(clip.rotation[0].blend, 0)
        clip.advance()
        self.assertEqual(clip.translation[0].value, (9, 0, 0))
        self.assertEqual(clip.rotation[0].blend, 0.125)


if __name__ == "__main__":
    unittest.main()
