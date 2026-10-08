"""Synthetic shutdown tests: no emulator, IPC connection, or real signal."""
from __future__ import annotations

import io
import json
from pathlib import Path
import re
import signal
import struct
import subprocess
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch
import zipfile

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


class ColdRouteTests(unittest.TestCase):
    def setUp(self):
        root = proof.ROOT / "build/repack"
        root.mkdir(parents=True, exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="proof-route-", dir=root)
        self.addCleanup(temporary.cleanup)
        self.base = Path(temporary.name)
        self.states = self.base / "states"
        self.states.mkdir()
        self.target = self.states / "SCUS-97112 (0AE679AF).16.p2s"

    def snapshot_game(self):
        game = SimpleNamespace(frames_stepped=42, pine=SimpleNamespace(save=Mock()))

        def save(slot):
            self.assertEqual(slot, 16)
            with zipfile.ZipFile(self.target, "w") as state:
                state.writestr("Screenshot.png", b"synthetic screenshot")
                state.writestr("iopMemory.bin", b"synthetic iop")
                state.writestr("SPU2.bin", b"synthetic spu")

        def read(address, size):
            if address == 0x282154:
                return bytes((2, 1, 0, 2))[:size]
            self.assertIn(address, [0x281FD0 + lane * 0x60 for lane in range(3)])
            self.assertEqual(size, 0x60)
            data = bytearray(size)
            struct.pack_into("<I", data, 4, (address - 0x281FD0) // 0x60)
            return bytes(data)

        game.pine.save.side_effect = save
        game.read = Mock(side_effect=read)
        game.u32 = lambda address: 1000 if address == proof.session.FRAME_COUNTER else 143
        return game

    def test_snapshot_has_three_lanes_and_removes_only_its_private_slot(self):
        game, clock = self.snapshot_game(), Clock()
        sibling = self.states / "SCUS-97112 (0AE679AF).15.p2s"
        sibling.write_bytes(b"protected sibling")
        with patch.object(proof.time, "monotonic", side_effect=lambda: clock.now), \
                patch.object(proof.time, "sleep", side_effect=clock.sleep), \
                patch.object(proof.session, "extract_zstd_entry", side_effect=lambda path, name: name.encode()):
            report = proof.private_snapshot(game, self.states, self.base / "capture")
        self.assertEqual([row["lane"] for row in report["stream_lanes"]], [0, 1, 2])
        self.assertEqual([row["active"] for row in report["stream_lanes"]], [2, 1, 0])
        self.assertEqual(set(report["files"]), {"original.png", "iopMemory.bin", "SPU2.bin"})
        self.assertFalse(self.target.exists())
        self.assertEqual(sibling.read_bytes(), b"protected sibling")

    def test_snapshot_extraction_failure_cleans_slot_and_occupied_slot_is_preserved(self):
        game, clock = self.snapshot_game(), Clock()
        with patch.object(proof.time, "monotonic", side_effect=lambda: clock.now), \
                patch.object(proof.time, "sleep", side_effect=clock.sleep), \
                patch.object(proof.session, "extract_zstd_entry", side_effect=ValueError("bad state")):
            with self.assertRaisesRegex(ValueError, "bad state"):
                proof.private_snapshot(game, self.states, self.base / "bad-capture")
        self.assertFalse(self.target.exists())
        self.target.write_bytes(b"pre-existing private slot")
        game.pine.save.reset_mock()
        with self.assertRaisesRegex(ValueError, "already occupied"):
            proof.private_snapshot(game, self.states, self.base / "occupied-capture")
        game.pine.save.assert_not_called()
        self.assertEqual(self.target.read_bytes(), b"pre-existing private slot")

    def route_game(self):
        game = SimpleNamespace(frames_stepped=0, pad=Mock(), write=Mock())

        def step(frames=1, **pad):
            first = game.frames_stepped
            game.frames_stepped += frames
            return list(range(first + 1, game.frames_stepped + 1))

        game.step = step
        game.read = lambda address, size: bytes((0, 2, 0, 0))[:size]
        game.u32 = lambda address: 143 if address == 0x28217C else 0
        return game

    def fake_route(self, game):
        r = SimpleNamespace(inputs=[{"f": 0, "buttons": 0}], teleports=[], begun=False)
        r.s = game

        def begin():
            r.begun = True

        def until(*args):
            self.assertTrue(r.begun, "Route.begin must populate state before until")

        r.begin, r.until = Mock(side_effect=begin), Mock(side_effect=until)
        return r

    def test_voice_route_restores_methods_and_pad_after_observation(self):
        from tools import route_capture as route
        game = self.route_game()
        original_step, original_write = game.step, game.write
        r = self.fake_route(game)

        def beat(_r):
            game.step(150)

        with patch.object(route, "Route", return_value=r), patch.object(route, "settle"), \
                patch.object(route, "beat_battery", side_effect=beat), \
                patch.object(proof, "private_snapshot", side_effect=lambda _g, _s, p: {"name": p.name}):
            result = proof.capture_voice_route(game, self.states, self.base / "route")
        self.assertEqual([item["name"] for item in result["snapshots"]], ["voice-060", "voice-120"])
        self.assertEqual(result["first_active_frame"], 1)
        self.assertEqual(result["frames"], 121)
        self.assertEqual(result["teleports"], 0)
        self.assertIs(game.step, original_step)
        self.assertIs(game.write, original_write)
        original_write.assert_not_called()
        game.pad.assert_called_once_with(0)
        self.assertEqual(json.loads((self.base / "route/inputs.json").read_text()), r.inputs)

    def test_voice_route_refuses_ram_writes_and_restores_methods_on_failure(self):
        from tools import route_capture as route
        game = self.route_game()
        original_step, original_write = game.step, game.write
        r = self.fake_route(game)
        with patch.object(route, "Route", return_value=r), patch.object(route, "settle"), \
                patch.object(route, "beat_battery", side_effect=lambda _r: game.write(0x810350, b"no")):
            with self.assertRaisesRegex(RuntimeError, "forbids RAM writes"):
                proof.capture_voice_route(game, self.states, self.base / "refused-route")
        self.assertIs(game.step, original_step)
        self.assertIs(game.write, original_write)
        original_write.assert_not_called()
        game.pad.assert_called_once_with(0)


if __name__ == "__main__":
    unittest.main()
