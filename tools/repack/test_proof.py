"""Synthetic shutdown tests: no emulator, IPC connection, or real signal."""
from __future__ import annotations

import io
from pathlib import Path
import re
import signal
import subprocess
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch

from tools.repack import proof


class Clock:
    def __init__(self):
        self.now = 0.0

    def sleep(self, seconds):
        self.now += seconds


class ProofLifecycleTests(unittest.TestCase):
    def game(self, *, pending=False):
        # Skip OriginalSession.__init__, which normally constructs IPC helpers.
        game = object.__new__(proof.ColdDiscSession)
        game.pid = None
        game.proc = None
        game.emulator = Path("/private/synthetic proof.[1]/PCSX2.app/Contents/MacOS/PCSX2")
        game.pine = SimpleNamespace(s=Mock())
        game._log = io.StringIO()
        game._launch_pending = pending
        return game

    def test_waits_for_exit_after_inherited_sigkill(self):
        clock, game = Clock(), self.game()
        game.pid = 101
        socket = game.pine.s
        signals = []

        def inherited_close(instance):
            proof.os.kill(instance.pid, signal.SIGKILL)
            instance.pid = None

        with patch.object(proof.session.OriginalSession, "close", inherited_close), \
                patch.object(game, "_emulator_pids", side_effect=lambda: [101] if clock.now < .3 else []), \
                patch.object(proof.time, "monotonic", side_effect=lambda: clock.now), \
                patch.object(proof.time, "sleep", side_effect=clock.sleep), \
                patch.object(proof.os, "kill", side_effect=lambda pid, sig: signals.append((pid, sig))):
            game.close()
        self.assertGreaterEqual(clock.now, .3)
        self.assertEqual(signals[0], (101, signal.SIGKILL))
        self.assertTrue(game._shutdown_complete)
        socket.close.assert_called_once()
        self.assertTrue(game._log.closed)

    def test_late_private_launch_is_killed_and_observed_exiting(self):
        clock, game = Clock(), self.game(pending=True)
        signals, killed_at = [], []

        def private_pids():
            if clock.now < .2 or killed_at and clock.now >= killed_at[0] + .2:
                return []
            return [202]

        def kill(pid, sig):
            signals.append((pid, sig))
            if sig == signal.SIGKILL:
                killed_at.append(clock.now)

        with patch.object(proof.session.OriginalSession, "close"), \
                patch.object(game, "_emulator_pids", side_effect=private_pids), \
                patch.object(proof.time, "monotonic", side_effect=lambda: clock.now), \
                patch.object(proof.time, "sleep", side_effect=clock.sleep), \
                patch.object(proof.os, "kill", side_effect=kill):
            game.close()
        self.assertEqual(signals, [(202, signal.SIGTERM), (202, signal.SIGKILL)])
        self.assertGreaterEqual(clock.now, killed_at[0] + .2)
        self.assertFalse(game._launch_pending)
        self.assertTrue(game._shutdown_complete)

    def test_persistent_process_raises_and_releases_lock_and_handles(self):
        clock, game = Clock(), self.game()
        socket = game.pine.s
        output = proof.ROOT / "build/repack"
        output.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="proof-lifecycle-", dir=output) as directory:
            lock = Path(directory) / "emulator.lock"
            with patch.object(proof, "LOCK", lock), \
                    patch.object(proof.session.OriginalSession, "close"), \
                    patch.object(game, "_emulator_pids", return_value=[303]), \
                    patch.object(proof.time, "monotonic", side_effect=lambda: clock.now), \
                    patch.object(proof.time, "sleep", side_effect=clock.sleep), \
                    patch.object(proof.os, "kill") as kill:
                with self.assertRaisesRegex(RuntimeError, "shutdown was not confirmed"):
                    with proof.emulator_lock():
                        game.close()
                self.assertFalse(lock.exists())
                self.assertIn(unittest.mock.call(303, signal.SIGKILL), kill.call_args_list)
        self.assertFalse(getattr(game, "_shutdown_complete", False))
        socket.close.assert_called_once()
        self.assertTrue(game._log.closed)

    def test_discovery_escapes_path_and_requires_executable_boundary(self):
        game = self.game()
        with patch.object(proof.subprocess, "run", return_value=subprocess.CompletedProcess([], 0, "404\n")) as run:
            self.assertEqual(game._emulator_pids(), [404])
        pattern = run.call_args.args[0][2]
        self.assertEqual(pattern, "^" + re.escape(str(game.emulator)) + "([[:space:]]|$)")
        # Python's regex engine uses a different spelling for the POSIX class.
        expression = re.compile(pattern.replace("[[:space:]]", r"\s"))
        self.assertIsNotNone(expression.search(str(game.emulator) + " -portable"))
        self.assertIsNotNone(expression.search(str(game.emulator)))
        self.assertIsNone(expression.search(str(game.emulator) + "-other"))
        self.assertIsNone(expression.search(str(game.emulator).replace(".[1]", "x1")))
        with patch.object(proof.subprocess, "run", return_value=subprocess.CompletedProcess([], 2, "")):
            with self.assertRaisesRegex(RuntimeError, "process ownership"):
                game._emulator_pids()


if __name__ == "__main__":
    unittest.main()
