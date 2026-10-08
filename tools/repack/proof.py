"""Hidden, cold-disc title captures through the existing PCSX2 session driver.

A private emulator copy and private configuration keep cards and reference save
slots inaccessible. The screenshot is taken from a new private save slot 16.
"""
from __future__ import annotations

import configparser
from contextlib import contextmanager
import json
import os
from pathlib import Path
import re
import shutil
import signal
import struct
import subprocess
import time
import zipfile

from .archive import ROOT, safe_output, sha256_file
from tools import pcsx2_session as session

REFERENCE = ROOT.parent / "Extermination" / "build" / "startup-reference"
LOCK = ROOT.parent / "Extermination" / "build" / ".pcsx2.lock"


@contextmanager
def emulator_lock():
    while True:
        try:
            LOCK.mkdir()
            break
        except FileExistsError:
            time.sleep(30)
    try:
        yield
    finally:
        LOCK.rmdir()


def prepare(out: Path, reference: Path) -> tuple[Path, Path]:
    app = out / "PCSX2.app"
    shutil.copytree(reference / "PCSX2.app", app, symlinks=True)
    config = configparser.ConfigParser(interpolation=None, strict=False)
    config.optionxform = str
    config.read(reference / "inis" / "PCSX2.ini")
    if "MemoryCards" not in config or "Folders" not in config:
        raise ValueError("reference emulator configuration is incomplete")
    bios = Path(config["Folders"]["Bios"])
    for name in config["Folders"]:
        path = out / "data" / name
        path.mkdir(parents=True, exist_ok=True)
        config["Folders"][name] = str(path)
    shutil.copytree(bios, Path(config["Folders"]["Bios"]), dirs_exist_ok=True)
    for key in config["MemoryCards"]:
        if key.endswith("_Enable"):
            config["MemoryCards"][key] = "false"
        elif key.endswith("_Filename"):
            config["MemoryCards"][key] = ""
    for key in ("SaveStateOnShutdown", "McdFolderAutoManage", "EnableCheats", "EnablePatches"):
        config["EmuCore"][key] = "false"
    config["EmuCore"]["EnablePINE"] = "true"
    config["UI"]["ConfirmShutdown"] = "false"
    config["UI"]["StartPaused"] = "false"
    config["UI"]["PauseOnFocusLoss"] = "false"
    config["UI"]["SetupWizardIncomplete"] = "false"
    (out / "inis").mkdir()
    with (out / "inis" / "PCSX2.ini").open("w") as stream:
        config.write(stream)
    return app / "Contents" / "MacOS" / "PCSX2", Path(config["Folders"]["Savestates"])


class ColdDiscSession(session.OriginalSession):
    """Reuse hiding and frame stepping, with bounded, checked shutdown."""
    SHUTDOWN_TIMEOUT = 10.0
    TERMINATE_GRACE = 1.0
    LATE_LAUNCH_GRACE = 2.0
    SHUTDOWN_POLL = 0.05

    def _emulator_pids(self):
        # pgrep uses an extended regular expression. Escape the private path
        # and require an argument boundary so similarly named apps are excluded.
        pattern = "^" + re.escape(str(self.emulator)) + "([[:space:]]|$)"
        result = subprocess.run(["pgrep", "-f", pattern], capture_output=True, text=True)
        if result.returncode not in (0, 1):
            raise RuntimeError("could not verify private emulator process ownership")
        return [int(value) for value in result.stdout.split()]

    def _resume_to_boundary(self, timeout=45.0):
        return super()._resume_to_boundary(timeout)

    def _start(self):
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self._log = (self.log_dir / "launch.log").open("w")
        # No ELF override and no loaded state: the boot executable and texture
        # uploads are read from the freshly built disc itself.
        args = ["-portable", "-fastboot", "-logfile", str(self.log_dir / "emulator.log"), str(self.iso)]
        before = set(self._emulator_pids())
        self._launch_pending = True
        subprocess.run(["open", "-g", "-j", "-n", "-a", str(self.emulator.parents[2]), "--args", *args],
                       check=True, stdout=self._log, stderr=subprocess.STDOUT)
        deadline = time.monotonic() + 20
        while time.monotonic() < deadline and self.pid is None:
            fresh = set(self._emulator_pids()) - before
            self.pid = max(fresh) if fresh else None
            time.sleep(0.05)
        if self.pid is None:
            raise RuntimeError("hidden emulator launch: process not found")
        self._launch_pending = False
        deadline = time.monotonic() + self.ready_timeout
        while time.monotonic() < deadline:
            self._hide()
            if not self._alive():
                raise RuntimeError("emulator exited during cold boot")
            try:
                status = self.debug.call({"cmd": "status"})
                if status.get("data", status).get("alive"):
                    self.debug.call({"cmd": "pause"})
                    break
            except (OSError, EOFError, RuntimeError):
                pass
            time.sleep(0.1)
        else:
            raise RuntimeError("emulator did not become ready")
        self.debug.call({"cmd": "pad_set", "clear": True})
        self.debug.call({"cmd": "set_breakpoint", "address": session.LOOP_TOP,
                         "description": "repack cold-disc frame boundary"})
        self._resume_to_boundary()
        self.pine = session.Pine()
        return self

    def _reap_private_processes(self):
        """Confirm exit after signals, including a delayed LaunchServices start."""
        started = time.monotonic()
        deadline = started + self.SHUTDOWN_TIMEOUT
        quiet_period = self.LATE_LAUNCH_GRACE if getattr(self, "_launch_pending", False) else 0.0
        quiet_since = started
        terminated, killed = {}, set()
        while True:
            now = time.monotonic()
            private = self._emulator_pids()
            if not private:
                if now - quiet_since >= quiet_period:
                    self.pid = self.proc = None
                    self._launch_pending = False
                    return
            else:
                quiet_since = now
                for pid in private:
                    sig = None
                    if pid not in terminated:
                        terminated[pid] = now
                        sig = signal.SIGTERM
                    elif pid not in killed and now - terminated[pid] >= self.TERMINATE_GRACE:
                        killed.add(pid)
                        sig = signal.SIGKILL
                    if sig is not None:
                        try:
                            os.kill(pid, sig)
                        except ProcessLookupError:
                            pass
            # A successful signal is not proof that the process has exited.
            if now >= deadline:
                raise RuntimeError(f"private emulator shutdown was not confirmed; remaining PIDs: {private}")
            time.sleep(min(self.SHUTDOWN_POLL, deadline - now))

    def close(self):
        if getattr(self, "_shutdown_complete", False):
            return
        try:
            try:
                super().close()
            finally:
                # The inherited terminator may return immediately after KILL.
                # Keep the lock until an exact private-path scan confirms exit.
                self._reap_private_processes()
                self._shutdown_complete = True
        finally:
            try:
                if self.pine is not None:
                    self.pine.s.close()
                    self.pine = None
            finally:
                if hasattr(self, "_log"):
                    self._log.close()


def reach_title(game, out: Path) -> tuple[bytes, bool]:
    stable = 0
    title_ready = False
    for frame in range(3000):
        task = game.read(0x28A750, 32)
        if frame % 100 == 0:
            (out / "progress.json").write_text(json.dumps(dict(frame=frame, function=hex(int.from_bytes(task[4:8], "little")),
                task_state=list(task[8:16]), fade=game.u32(0x28A9A0))) + "\n")
        on_title = int.from_bytes(task[4:8], "little") == 0x001AC070 and task[8] == 2
        at_menu = on_title and task[9] == 2
        if at_menu:
            game.pad(0)
            stable = stable + 1 if game.u32(0x28A9A0) == 0 else 0
            if stable >= 30:
                title_ready = True
                break
        else:
            stable = 0
            if int.from_bytes(task[4:8], "little") == 0x001AB7E0:
                # With both cards disabled, acknowledge the original
                # no-card prompt using ordinary controller input.
                game.pad(["CROSS"] if frame % 60 < 4 else 0)
            else:
                game.pad(0 if on_title else ["START"])
        game.step()
    return task, title_ready


def capture_title(iso: Path, out_dir: Path, *, reference: Path = REFERENCE) -> dict:
    image, out = Path(iso).resolve(), safe_output(out_dir)
    if not image.is_file() or (out.exists() and any(out.iterdir())):
        raise ValueError("provide an existing image and a new empty proof directory")
    out.mkdir(parents=True, exist_ok=True)
    with emulator_lock():
        # Do not contend with an emulator started outside the lock convention.
        if subprocess.run(["pgrep", "-f", r"^.*/PCSX2.app/Contents/MacOS/PCSX2"], capture_output=True).returncode == 0:
            raise ValueError("another PCSX2 instance is running; retry once it exits")
        emulator, states = prepare(out, reference)
        protected = {str(path): sha256_file(path) for path in (reference / "portable-data" / "sstates").glob("*.p2s")
                     if 1 <= int(path.name.split(".")[-2]) <= 15}
        try:
            expected_image_hash = sha256_file(image)
            identity = out / "proof-input.json"
            identity.write_text(json.dumps(dict(iso=str(image), sha256=expected_image_hash)) + "\n")
            # OriginalSession protects this small immutable identity receipt;
            # stream the ISO hash separately instead of loading 2 GiB into RAM.
            with ColdDiscSession(identity, emulator=emulator, iso=image, log_dir=out / "logs", ready_timeout=60) as game:
                task, title_ready = reach_title(game, out)
                target = states / "SCUS-97112 (0AE679AF).16.p2s"
                if target.exists():
                    raise ValueError("private screenshot slot is already occupied")
                game.pine.save(16)
                deadline, previous = time.monotonic() + 15, -1
                while time.monotonic() < deadline:
                    size = target.stat().st_size if target.exists() else 0
                    if size > 0 and size == previous:
                        break
                    previous = size
                    time.sleep(0.25)
                else:
                    raise RuntimeError("private screenshot state was not written")
                with zipfile.ZipFile(target) as state:
                    (out / "title.png").write_bytes(state.read("Screenshot.png"))
                result = dict(iso_path=str(image), iso_sha256=sha256_file(image),
                              screenshot=str(out / "title.png"), screenshot_sha256=sha256_file(out / "title.png"),
                              frames=game.frames_stepped, counter=game.u32(session.FRAME_COUNTER),
                              task_state=list(task[8:16]), hidden=True, cold_disc_boot=True,
                              memory_cards_enabled=False, private_slot=16, title_ready=title_ready)
                target.unlink()
                if result["iso_sha256"] != expected_image_hash:
                    raise RuntimeError("proof input image changed during capture")
        finally:
            if any(sha256_file(Path(path)) != digest for path, digest in protected.items()):
                raise RuntimeError("protected reference save state changed")
            if any((out / "data" / "MemoryCards").iterdir()):
                raise RuntimeError("private memory-card directory is no longer empty")
        result["protected_save_states_unchanged"] = len(protected)
        (out / "proof.json").write_text(json.dumps(result, indent=2) + "\n")
        if not title_ready:
            raise RuntimeError(f"cold boot did not reach the interactive title menu; inspect {out / 'title.png'}")
    return result


def private_snapshot(game, states: Path, out: Path) -> dict:
    """Keep a screenshot and sound state from private slot 16, then remove it."""
    out.mkdir(parents=True, exist_ok=False)
    target = states / "SCUS-97112 (0AE679AF).16.p2s"
    if target.exists():
        raise ValueError("private screenshot slot is already occupied")
    try:
        game.pine.save(16)
        deadline, previous = time.monotonic() + 15, -1
        while time.monotonic() < deadline:
            size = target.stat().st_size if target.exists() else 0
            if size > 0 and size == previous:
                break
            previous = size
            time.sleep(0.25)
        else:
            raise RuntimeError("private snapshot was not written")
        with zipfile.ZipFile(target) as state:
            (out / "original.png").write_bytes(state.read("Screenshot.png"))
            names = state.namelist()
        for name in ("iopMemory.bin", "SPU2.bin"):
            if name in names:
                (out / name).write_bytes(session.extract_zstd_entry(target, name))
        lanes = []
        active = game.read(0x282154, 4)
        for lane in range(3):
            raw = game.read(0x281FD0 + lane * 0x60, 0x60)
            word = lambda at: struct.unpack_from("<I", raw, at)[0]
            lanes.append({"lane": lane, "state": raw[0], "half": raw[1], "last_half": raw[2],
                          "voice": word(4), "buffer": word(0x14), "loop": word(0x20),
                          "loop_sector": word(0x24), "end": word(0x28), "read_sector": word(0x30),
                          "volume": word(0x58), "active": active[lane], "cue": game.u32(0x282178 + lane * 4)})
        return {"frame": game.frames_stepped, "counter": game.u32(session.FRAME_COUNTER),
                "stream_lanes": lanes,
                "screenshot": str(out / "original.png"),
                "files": {p.name: sha256_file(p) for p in out.iterdir()}}
    finally:
        if target.exists():
            target.unlink()


def capture_gameplay(iso: Path, out_dir: Path, *, model: Path | None = None,
                     frames: int = 400, voice_route: bool = False, reference: Path = REFERENCE) -> dict:
    """Cold New Game proof using only controller input and read-only observations.

    An optional player model is checked at three native vertex ranges through
    the game's resource-slot pointer, not a historical assumed heap address.
    Sound snapshots allow independent verification of the streaming driver.
    """
    image, out = Path(iso).resolve(), safe_output(out_dir)
    if not image.is_file() or (out.exists() and any(out.iterdir())) or not 100 <= frames <= 3000:
        raise ValueError("provide an image, an empty proof directory and 100..3000 gameplay frames")
    expected_model = Path(model).read_bytes() if model else None
    probes = (73264, 127344, 181424)
    if expected_model is not None and len(expected_model) <= max(probes) + 64:
        raise ValueError("model proof expects the player resource chunk28/f00_id3b.bin")
    out.mkdir(parents=True, exist_ok=True)
    with emulator_lock():
        scan = subprocess.run(["pgrep", "-f", r"^.*/PCSX2.app/Contents/MacOS/PCSX2"], capture_output=True)
        if scan.returncode != 1:
            raise ValueError("another emulator is running or process ownership cannot be checked")
        emulator, states = prepare(out, reference)
        protected = {str(p): sha256_file(p) for p in (reference / "portable-data" / "sstates").glob("*.p2s")
                     if 1 <= int(p.name.split(".")[-2]) <= 15}
        digest = sha256_file(image)
        identity = out / "proof-input.json"
        identity.write_text(json.dumps({"iso": str(image), "sha256": digest}) + "\n")
        result = {"iso": str(image), "iso_sha256": digest, "hidden": True,
                  "cold_disc_boot": True, "memory_cards_enabled": False, "private_slot": 16,
                  "snapshots": [], "model_probes": []}
        try:
            with ColdDiscSession(identity, emulator=emulator, iso=image, log_dir=out / "logs", ready_timeout=60) as game:
                _, ready = reach_title(game, out)
                if not ready:
                    raise RuntimeError("cold boot did not reach the interactive title")
                result["title"] = private_snapshot(game, states, out / "title")
                entered = False
                for n in range(4500):
                    task = game.read(0x28A750, 16)
                    area = game.u32(0x810700) & 255
                    if area == 11 and task[11] == 1:
                        entered = True
                        break
                    at_title = int.from_bytes(task[4:8], "little") == 0x1AC070 and task[8:10] == b"\x02\x02"
                    game.pad(["CROSS"] if at_title and n % 120 < 4 else ([] if at_title else ["START"]))
                    game.step()
                    if n % 100 == 0:
                        (out / "progress.json").write_text(json.dumps({"phase": "new-game", "frames": n, "area": area}) + "\n")
                game.pad(0)
                if not entered:
                    raise RuntimeError("New Game did not enter AREA11")
                result["area11_counter"] = game.u32(session.FRAME_COUNTER)
                for n in range(frames + 1):
                    if n in (100, frames // 2, frames):
                        pointer = game.u32(0x28A57C)
                        matches = []
                        if expected_model is not None:
                            if pointer < 0x100000 or pointer + len(expected_model) > 0x2000000:
                                raise RuntimeError("player resource pointer is outside EE asset memory")
                            matches = [game.read(pointer + at, 64) == expected_model[at:at + 64] for at in probes]
                            if not all(matches):
                                raise RuntimeError("loaded player vertex probes differ from the edited disc asset")
                        result["model_probes"].append({"frame": n, "resource_pointer": pointer, "matches": matches})
                        result["snapshots"].append(private_snapshot(game, states, out / f"frame-{n:04d}"))
                        (out / "proof.json").write_text(json.dumps(result, indent=2) + "\n")
                    if n < frames:
                        game.step()
                if voice_route:
                    result["voice_route"] = capture_voice_route(game, states, out / "voice-route")
                result["frames"] = game.frames_stepped
            if sha256_file(image) != digest:
                raise RuntimeError("proof input disc changed")
        finally:
            if any(sha256_file(Path(p)) != h for p, h in protected.items()):
                raise RuntimeError("protected reference save state changed")
            if any((out / "data" / "MemoryCards").iterdir()):
                raise RuntimeError("private memory-card directory is no longer empty")
        result["protected_save_states_unchanged"] = len(protected)
        result["emulator_exit_confirmed"] = True
        (out / "proof.json").write_text(json.dumps(result, indent=2) + "\n")
    return result


def capture_voice_route(game, states: Path, out: Path) -> dict:
    """Walk the established first-level route until dialogue cue 143 plays.

    Reuses only controller-driven route functions, never their checkpoint
    loader/saver. No reference slot, RAM write or teleport participates.
    """
    from tools import route_capture as route
    out.mkdir(parents=True, exist_ok=False)
    result = {"controller_only": True, "beats": [], "snapshots": []}
    plain_step, plain_write, first_active = game.step, game.write, None

    class Observed(Exception):
        pass

    def observed_step(frames=1, **pad):
        nonlocal first_active
        counters = []
        for _ in range(frames):
            counters += plain_step(1, **pad)
            active = game.read(0x282154, 4)
            cues = [game.u32(0x282178 + 4 * lane) for lane in (1, 2)]
            playing = any(active[lane] and cues[lane - 1] == 143 for lane in (1, 2))
            if playing and first_active is None:
                first_active = game.frames_stepped
            if first_active is not None and game.frames_stepped - first_active in (60, 120):
                elapsed = game.frames_stepped - first_active
                result["snapshots"].append(private_snapshot(game, states, out / f"voice-{elapsed:03d}"))
                if elapsed == 120:
                    raise Observed()
            if game.frames_stepped % 200 == 0:
                (out / "progress.json").write_text(json.dumps({"frames": game.frames_stepped,
                    "beat": result["beats"][-1]["name"] if result["beats"] else "opening",
                    "voice_cues": cues, "voice_active": list(active[1:3])}) + "\n")
        return counters

    game.step = observed_step
    def refuse_write(*_args, **_kwargs):
        raise RuntimeError("the cold controller proof forbids RAM writes")
    game.write = refuse_write
    try:
        r = route.Route(game)
        r.begin()
        r.until(route.in_control, 2200)
        route.settle(r)
        for name in ("battery", "elevator_refusal", "panel_power", "elevator_ride", "boxes",
                     "hill_slide", "truck_preview", "truck_crossing", "cage_roof_roger"):
            row = {"name": name, "start_frame": game.frames_stepped}
            result["beats"].append(row)
            getattr(route, "beat_" + name)(r)
            row["end_frame"] = game.frames_stepped
        raise RuntimeError("controller route ended without dialogue cue143")
    except Observed:
        result["first_active_frame"] = first_active
        result["frames"] = game.frames_stepped
        result["teleports"] = len(r.teleports)
        if r.teleports:
            raise RuntimeError("voice proof route unexpectedly teleported")
        (out / "inputs.json").write_text(json.dumps(r.inputs) + "\n")
        (out / "proof.json").write_text(json.dumps(result, indent=2) + "\n")
        return result
    finally:
        game.step = plain_step
        game.write = plain_write
        game.pad(0)
