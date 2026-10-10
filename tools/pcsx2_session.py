#!/usr/bin/env python3
"""pcsx2_session.py — drive the ORIGINAL game in the MCP-enabled PCSX2 frame by frame.

Lockstep evidence for the native port: load one of the user's own save states,
advance exactly one main-loop frame at a time with a chosen pad state, read EE
memory between frames, and snapshot the full machine (eeMemory, GS, scratchpad
and the save state's embedded screenshot) at chosen frames.

Mechanism (no emulator rebuild needed):
  * frame boundary = a breakpoint at the main-loop top 0x001AAF28 (FINDINGS
    "ENGINE FRAME ANATOMY", step A). One resume -> the next hit = one frame;
    the main-loop counter 0x70003B64 advances by exactly one.
  * input = DebugServer pad_set (libpad mask order, applied at vsync above
    SIO2). Measured latency: a pad state set before step N first appears in
    the game's processed pad block (0x810E70) after step N+2. Save state 03
    still has player movement locked; state 04 accepts stick movement.
  * memory = Pine IPC reads; snapshots = Pine save to a FREE slot, then the
    zstd entries are extracted and the slot file is moved into the output
    directory so the user's existing save-state slots are never overwritten.

The emulator runs HIDDEN by default (launched with `open -g -j`, then kept
hidden through System Events while it boots) so automated captures never put
a window in front of the user; pass visible=True (or --visible) to watch.

Everything read or written here is derived from the user's own disc and stays
in gitignored build/ output. The source save state is hashed before and after
to prove it was not modified.

Example:
    from pcsx2_session import OriginalSession, PAD
    with OriginalSession('build/startup-reference/portable-data/sstates/'
                         'SCUS-97112 (0AE679AF).03.p2s') as s:
        s.step(30, ly=0x00)                      # hold left stick up 30 frames
        pos = s.read_f32(0x810350, 3)
        s.snapshot('build/s87/walk30')
"""
from __future__ import annotations

import hashlib
import json
import os
import shutil
import signal
import socket
import struct
import subprocess
import sys
import time
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from parse_pcsx2_state import extract_zstd_entry  # noqa: E402

REFERENCE = ROOT / "build/startup-reference"
DEFAULT_EMULATOR = REFERENCE / "PCSX2.app/Contents/MacOS/PCSX2"
DEFAULT_ISO = ROOT / "Extermination-rebuilt.iso"
ELF = ROOT / "config/SCUS_971.12"
SSTATES = REFERENCE / "portable-data/sstates"
SERIAL = "SCUS-97112 (0AE679AF)"
LOOP_TOP = 0x001AAF28

# The agent-debug fork (docs/PCSX2_FORK.md) and the states it saved
# (build/startup-reference/fork-states/, described by its manifest.json).
FORK_REPO = Path(os.environ.get("PCSX2_FORK_REPO", ROOT.parent / "pcsx2-fork"))
FORK_APP = Path(os.environ.get("PCSX2_FORK_APP", FORK_REPO / "build-x64/pcsx2-qt/PCSX2.app"))
FORK_PY = FORK_REPO / "extermination/python"
FORK_STATES = REFERENCE / "fork-states"
FORK_MANIFEST = FORK_STATES / "manifest.json"
RUN_LOCK = ROOT / "build/.pcsx2.lock"


def fork_manifest() -> dict:
    if not FORK_MANIFEST.exists():
        raise FileNotFoundError(f"no fork-state manifest at {FORK_MANIFEST} "
                                "(regenerate with tools/fork_states.py)")
    return json.loads(FORK_MANIFEST.read_text())


def fork_state(name: str | Path) -> Path:
    """The fork-saved state that replaces a v2.6.3 one.  `name` is a manifest
    key ("slot04_first_control", "route/14_roger_encounter"), an alias
    ("04", "slot04", "14_roger_encounter") or the old state's path (a user slot
    file or a build/s87/... beat snapshot).  An existing path that is not a
    v2.6.3 state the manifest knows is returned unchanged."""
    m = fork_manifest()
    states, aliases = m["states"], m.get("aliases", {})
    key = str(name)
    p = Path(key)
    if p.suffix == ".p2s":
        rp = p.resolve()
        for k, v in states.items():
            old = v.get("old")
            if old and (ROOT / old).resolve() == rp:
                key = k
                break
        else:
            if rp.exists():
                return rp
            raise KeyError(f"{name}: not a state the fork manifest replaces")
    key = aliases.get(key, key)
    if key not in states:
        raise KeyError(f"{name}: not in {FORK_MANIFEST} (keys: {', '.join(sorted(states))})")
    path = FORK_STATES / states[key]["file"]
    if not path.exists():
        raise FileNotFoundError(path)
    return path
FRAME_COUNTER = 0x70003B64
VSYNC_COUNTER = 0x00810E90

# libpad mask bits (DebugServer.cpp)
PAD = {
    "SELECT": 0x0001, "L3": 0x0002, "R3": 0x0004, "START": 0x0008,
    "UP": 0x0010, "RIGHT": 0x0020, "DOWN": 0x0040, "LEFT": 0x0080,
    "L2": 0x0100, "R2": 0x0200, "L1": 0x0400, "R1": 0x0800,
    "TRIANGLE": 0x1000, "CIRCLE": 0x2000, "CROSS": 0x4000, "SQUARE": 0x8000,
}


class DebugServer:
    def __init__(self, port: int = 21512):
        self.port = port

    def call(self, cmd: dict) -> dict:
        with socket.create_connection(("127.0.0.1", self.port), timeout=5) as s:
            s.sendall((json.dumps(cmd) + "\n").encode())
            data = b""
            while b"\n" not in data:
                chunk = s.recv(65536)
                if not chunk:
                    raise EOFError("DebugServer closed the connection")
                data += chunk
        response = json.loads(data.split(b"\n")[0])
        if not response.get("ok"):
            raise RuntimeError(response)
        return response


class Pine:
    def __init__(self, path: str | Path | None = None):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.s.settimeout(5)
        self.s.connect(str(path or Path(os.environ["TMPDIR"]) / "pcsx2.sock"))

    def _recv(self, n: int) -> bytes:
        b = b""
        while len(b) < n:
            v = self.s.recv(n - len(b))
            if not v:
                raise EOFError("Pine closed")
            b += v
        return b

    def request(self, body: bytes) -> bytes:
        self.s.sendall(struct.pack("<I", len(body) + 4) + body)
        n = struct.unpack("<I", self._recv(4))[0]
        data = self._recv(n - 4)
        if data[0]:
            raise RuntimeError(f"Pine rejected {body[:5].hex()}")
        return data[1:]

    def read(self, address: int, size: int) -> bytes:
        if size % 4 or address % 4:
            raise ValueError("Pine reads are 32-bit aligned")
        out = bytearray()
        for base in range(0, size, 0x4000):  # keep individual requests small
            n = min(0x4000, size - base)
            out += self.request(b"".join(struct.pack("<BI", 2, address + base + i)
                                         for i in range(0, n, 4)))
        return bytes(out)

    def save(self, slot: int) -> None:
        self.request(bytes((9, slot)))


class OriginalSession:
    def __init__(self, state: str | Path, emulator: Path = DEFAULT_EMULATOR,
                 iso: Path = DEFAULT_ISO, log_dir: Path | None = None,
                 ready_timeout: float = 45.0, visible: bool = False,
                 env: dict[str, str] | None = None,
                 data_dir: str | Path | None = None, renderer: int = 13):
        """env: extra environment variables for the emulator process ONLY
        (opt-in; empty = the unchanged launch). video_compare/ps2.py passes
        DYLD_FALLBACK_LIBRARY_PATH to a folder of FFmpeg libraries for
        PCSX2's Media Capture; they must be built for the emulator's
        architecture (this PCSX2 is x86_64). Nothing in the emulator install
        changes.

        emulator + data_dir (opt-in; data_dir None = the unchanged -portable
        launch of the legacy app, which reads and writes its own portable
        data): run another PCSX2 build, e.g. the local agent-debug fork
        (../pcsx2-fork/build-x64/pcsx2-qt/PCSX2.app/Contents/MacOS/PCSX2), on
        a SCRATCH data folder.  data_dir must lie outside ~/Documents (each
        rebuilt, ad-hoc-signed app needs macOS permission for Documents files
        and, hidden, blocks on that prompt).  The session fills it from
        read-only copies of portable-data/bios and inis/PCSX2.ini (every
        [Folders] entry pointed inside it, PINE on slot 28011, no hotkeys,
        [EmuCore/GS] Renderer = `renderer`, 13 = software: the fork build has
        no Metal renderer), copies the state and the ELF into it, APFS-clones
        the ISO into it (removed again on close) and launches with -datapath.
        Snapshots then go through <data_dir>/PCSX2/sstates; the emulator's
        log is <data_dir>/emulator.log (launch.log stays in log_dir).  The folder keeps
        BIOS copies: delete it after the session.  v2.6.3 states (version
        0x9A55) do not load in the fork (0x9A59); give it states it saved."""
        self.state = Path(state).resolve()
        if not self.state.exists():
            raise FileNotFoundError(self.state)
        self.data_dir = Path(data_dir).resolve() if data_dir else None
        if self.data_dir is not None:
            documents = (Path.home() / "Documents").resolve()
            if self.data_dir == documents or documents in self.data_dir.parents:
                raise ValueError(f"data_dir must be outside ~/Documents: {self.data_dir}")
            if self.data_dir == REFERENCE.resolve() or REFERENCE.resolve() in self.data_dir.parents:
                raise ValueError(f"data_dir must not be inside {REFERENCE} (user data)")
        self.renderer = renderer
        self.sstates = self.data_dir / "PCSX2/sstates" if self.data_dir else SSTATES
        self._iso_clone: Path | None = None
        self.emulator, self.iso = Path(emulator), Path(iso)
        self.log_dir = Path(log_dir) if log_dir else ROOT / "build/pcsx2_session"
        self.ready_timeout = ready_timeout
        self.proc: subprocess.Popen | None = None
        self.pid: int | None = None
        self.visible = visible
        self.env = dict(env or {})
        self.pine: Pine | None = None
        self.debug = DebugServer()
        self.frames_stepped = 0
        self._digest = hashlib.sha256(self.state.read_bytes()).hexdigest()

    # -- lifecycle -------------------------------------------------------
    def __enter__(self) -> "OriginalSession":
        try:
            return self._start()
        except BaseException:
            self.close()
            raise

    def _start(self) -> "OriginalSession":
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self._log = open(self.log_dir / "launch.log", "w")
        if self.data_dir is None:
            args = ["-portable", "-fastboot", "-statefile", str(self.state), "-elf", str(ELF),
                    "-logfile", str(self.log_dir / "emulator.log"), str(self.iso)]
        else:
            state, elf, iso = self._prepare_data_dir()
            # the emulator's own log goes into data_dir too (a Documents path would block it)
            args = ["-datapath", str(self.data_dir), "-fastboot", "-statefile", str(state),
                    "-elf", str(elf), "-logfile", str(self.data_dir / "emulator.log"), str(iso)]
        if self.env:
            self._log.write(f"emulator environment additions: {self.env}\n")
            self._log.flush()
        if self.visible:
            self.proc = subprocess.Popen([str(self.emulator), *args],
                                         stdout=self._log, stderr=subprocess.STDOUT,
                                         env={**os.environ, **self.env} if self.env else None)
            self.pid = self.proc.pid
        else:
            bundle = self.emulator.parents[2]          # .../PCSX2.app
            before = set(self._emulator_pids())
            envargs = [a for k, v in self.env.items() for a in ("--env", f"{k}={v}")]
            subprocess.run(["open", "-g", "-j", "-n", *envargs, "-a", str(bundle), "--args", *args],
                           check=True, stdout=self._log, stderr=subprocess.STDOUT)
            deadline = time.monotonic() + 20
            while time.monotonic() < deadline and self.pid is None:
                fresh = [p for p in self._emulator_pids() if p not in before]
                self.pid = max(fresh) if fresh else None
                time.sleep(0.05)
            if self.pid is None:
                raise RuntimeError("hidden emulator launch: process not found")
        deadline = time.monotonic() + self.ready_timeout
        while time.monotonic() < deadline:
            if not self._alive():
                raise RuntimeError("emulator exited early")
            if not self.visible:
                self._hide()
            try:
                self.pine = Pine()
                if self.u32(FRAME_COUNTER) > 0:
                    break
            except (OSError, EOFError, RuntimeError):
                self.pine = None
            time.sleep(0.1)
        else:
            raise RuntimeError("original state did not become ready")
        if not self.visible:
            self._hide()
        self.debug.call({"cmd": "pause"})
        self.debug.call({"cmd": "pad_set", "clear": True})
        self.debug.call({"cmd": "set_breakpoint", "address": LOOP_TOP,
                         "description": "pcsx2_session frame boundary"})
        # Align to a frame boundary so every later step() is exactly one frame.
        self._resume_to_boundary()
        return self

    def __exit__(self, *exc) -> None:
        self.close()

    def _prepare_data_dir(self) -> tuple[Path, Path, Path]:
        """Scratch -datapath folder (opt-in, see __init__): read-only copies in,
        nothing written back.  Returns the local (state, elf, iso)."""
        root = self.data_dir / "PCSX2"
        folders = {"Bios": "bios", "Snapshots": "snaps", "Savestates": "sstates",
                   "MemoryCards": "memcards", "Logs": "logs", "Cheats": "cheats",
                   "Patches": "patches", "UserResources": "resources", "Cache": "cache",
                   "Textures": "textures", "InputProfiles": "inputprofiles", "Videos": "videos",
                   "DebuggerLayouts": "debuggerlayouts", "DebuggerSettings": "debuggersettings"}
        for sub in ("inis", *folders.values()):
            (root / sub).mkdir(parents=True, exist_ok=True)
        for f in sorted((SSTATES.parent / "bios").iterdir()):
            if f.is_file():          # copies: the BIOS writes its .nvm/.mec next to the image
                shutil.copyfile(f, root / "bios" / f.name)
        sets = {"Folders": {k: str(root / v) for k, v in folders.items()},
                "EmuCore": {"EnablePINE": "true", "PINESlot": "28011", "SaveStateOnShutdown": "false",
                            "EnableDiscordPresence": "false"},
                "EmuCore/GS": {"Renderer": str(self.renderer)},
                "UI": {"StartPaused": "false", "ConfirmShutdown": "false",
                       "SetupWizardIncomplete": "false", "PauseOnFocusLoss": "false"},
                "AutoUpdater": {"CheckAtStartup": "false"}}
        out, section, seen = [], "", set()
        for line in (REFERENCE / "inis/PCSX2.ini").read_text().splitlines():
            if line.startswith("[") and line.rstrip().endswith("]"):
                for k, v in sets.get(section, {}).items():
                    if (section, k) not in seen:
                        out.append(f"{k} = {v}")
                section = line.strip()[1:-1]
                out.append(line)
                continue
            key = line.split("=", 1)[0].strip()
            if section == "Hotkeys" and "=" in line:
                continue                 # no host hotkeys in an automated session
            if "=" in line and key in sets.get(section, {}):
                out.append(f"{key} = {sets[section][key]}")
                seen.add((section, key))
                continue
            out.append(line)
        for k, v in sets.get(section, {}).items():
            if (section, k) not in seen:
                out.append(f"{k} = {v}")
        (root / "inis/PCSX2.ini").write_text("\n".join(out) + "\n")
        state = self.data_dir / self.state.name
        shutil.copyfile(self.state, state)
        elf = self.data_dir / ELF.name
        shutil.copyfile(ELF, elf)
        iso = self.data_dir / self.iso.name
        if not iso.exists():
            subprocess.run(["cp", "-c", str(self.iso), str(iso)], check=True)   # APFS clone
            self._iso_clone = iso
        return state, elf, iso

    def close(self) -> None:
        if self._iso_clone is not None and self.pid is None:
            self._remove_iso_clone()
        if self.pid is None:
            return
        try:
            self.debug.call({"cmd": "remove_breakpoint", "address": LOOP_TOP})
            self.debug.call({"cmd": "pad_set", "clear": True})
            self.debug.call({"cmd": "pause"})
        except Exception:
            pass
        self._terminate()
        self.proc = None
        self.pid = None
        self._remove_iso_clone()
        if hashlib.sha256(self.state.read_bytes()).hexdigest() != self._digest:
            raise RuntimeError(f"source save state changed: {self.state}")

    def _remove_iso_clone(self) -> None:
        clone, self._iso_clone = self._iso_clone, None
        if clone is not None and clone.is_file() and not clone.is_symlink():
            clone.unlink()

    # -- process helpers -------------------------------------------------------
    def _emulator_pids(self) -> list[int]:
        # anchored: only processes whose command starts with the emulator binary
        out = subprocess.run(["pgrep", "-f", "^" + str(self.emulator)], capture_output=True,
                             text=True).stdout.split()
        return [int(x) for x in out]

    def _alive(self) -> bool:
        if self.proc is not None:
            return self.proc.poll() is None
        try:
            os.kill(self.pid, 0)
            return True
        except OSError:
            return False

    def _hide(self) -> None:
        subprocess.run(["osascript", "-e", 'tell application "System Events" to set visible of '
                        f"(first process whose unix id is {self.pid}) to false"],
                       capture_output=True)

    def _terminate(self) -> None:
        if self.proc is not None:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
            return
        try:
            os.kill(self.pid, signal.SIGTERM)
        except OSError:
            return
        for _ in range(50):
            if not self._alive():
                return
            time.sleep(0.1)
        try:
            os.kill(self.pid, signal.SIGKILL)
        except OSError:
            pass

    # -- stepping ----------------------------------------------------------
    def _paused(self) -> bool:
        status = self.debug.call({"cmd": "status"})
        return bool(status.get("data", status).get("paused"))

    def _resume_to_boundary(self, timeout: float = 5.0) -> None:
        self.debug.call({"cmd": "resume"})
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if self._paused():
                return
            time.sleep(0.003)   # tighter polling exhausts local TCP ports
        raise TimeoutError("frame boundary breakpoint did not hit")

    def pad(self, buttons: int | list[str] = 0, lx: int = 0x7F, ly: int = 0x7F,
            rx: int = 0x7F, ry: int = 0x7F) -> None:
        """Hold a full pad state until changed. Sticks: 0x00 = up/left, 0xFF = down/right."""
        if isinstance(buttons, (list, tuple)):
            buttons = sum(PAD[name] for name in buttons)
        self.debug.call({"cmd": "pad_set", "buttons": buttons, "lx": lx, "ly": ly,
                         "rx": rx, "ry": ry})

    def step(self, frames: int = 1, **pad) -> list[int]:
        """Advance whole main-loop frames; optional pad(...) keywords apply first.
        Returns the main-loop counter after each frame."""
        if pad:
            self.pad(**pad)
        counters = []
        for _ in range(frames):
            before = self.u32(FRAME_COUNTER)
            self._resume_to_boundary()
            after = self.u32(FRAME_COUNTER)
            if after != before + 1:
                raise RuntimeError(f"frame step skipped: {before} -> {after}")
            counters.append(after)
            self.frames_stepped += 1
        return counters

    # -- memory ------------------------------------------------------------
    def read(self, address: int, size: int) -> bytes:
        assert self.pine
        return self.pine.read(address, size)

    def u32(self, address: int) -> int:
        return struct.unpack("<I", self.read(address, 4))[0]

    def read_f32(self, address: int, count: int = 1) -> tuple[float, ...]:
        return struct.unpack(f"<{count}f", self.read(address, 4 * count))

    def write(self, address: int, data: bytes) -> None:
        self.debug.call({"cmd": "write_memory", "cpu": "ee", "address": hex(address),
                         "data": data.hex()})

    # -- snapshots -----------------------------------------------------------
    def snapshot(self, out_dir: str | Path, slot: int | None = None) -> dict:
        """Save the paused machine to a free slot, extract EE/GS/scratchpad and the
        embedded screenshot into out_dir, and move the slot file there too."""
        out = Path(out_dir)
        out.mkdir(parents=True, exist_ok=True)
        serial = self.state.name.split(".")[0]
        if slot is None:
            used = {int(p.name.split(".")[-2]) for p in self.sstates.glob(f"{serial}.*.p2s")}
            slot = next(s for s in range(16, 64) if s not in used)
        target = self.sstates / f"{serial}.{slot:02d}.p2s"
        if target.exists():
            raise FileExistsError(target)
        assert self.pine
        self.pine.save(slot)
        deadline = time.monotonic() + 10
        size = -1
        while time.monotonic() < deadline:  # wait for the file to be complete
            if target.exists() and target.stat().st_size == size and size > 0:
                break
            size = target.stat().st_size if target.exists() else -1
            time.sleep(0.2)
        else:
            raise RuntimeError("save state was not written")
        for name, dst in (("eeMemory.bin", "eeMemory.bin"), ("GS.bin", "gs.bin"),
                          ("Scratchpad.bin", "scratchpad.bin")):
            (out / dst).write_bytes(extract_zstd_entry(target, name))
        with zipfile.ZipFile(target) as z:
            if "Screenshot.png" in z.namelist():
                (out / "original.png").write_bytes(z.read("Screenshot.png"))
        shutil.move(str(target), out / "state.p2s")
        info = {"source_state": str(self.state), "source_sha256": self._digest,
                "frames_stepped": self.frames_stepped,
                "main_loop_counter": self.u32(FRAME_COUNTER),
                "vsync_counter": self.u32(VSYNC_COUNTER),
                "ee_sha256": hashlib.sha256((out / "eeMemory.bin").read_bytes()).hexdigest()}
        (out / "snapshot.json").write_text(json.dumps(info, indent=2) + "\n")
        return info


class ForkSession(OriginalSession):
    """OriginalSession on the agent-debug fork (opt-in; docs/PCSX2_FORK.md).

    Same API (step, pad, read, write, snapshot, frames_stepped), launched
    through the fork's own pcsx2dbg launcher: hidden, on a fresh scratch
    folder outside ~/Documents (BIOS and ini copies, the ISO APFS-cloned, the
    write root inside it), software renderer, no frame limiter.  A frame step
    is one fork `run {until: {ticks: 1}}` with the tick PC at the main-loop top
    (the same boundary as the legacy breakpoint, one request instead of a
    resume-and-poll); memory reads stay on PINE (this instance's socket).
    snapshot() saves with the fork's exact `state_save` into the write root
    and takes original.png from `gs_field` (the displayed field).

    The state must be one the fork saved (0x9A59); fork_state() resolves the
    v2.6.3 names through build/startup-reference/fork-states/manifest.json.
    The session takes the run lock build/.pcsx2.lock itself (waiting up to
    `lock_wait` seconds for another run to finish) and removes it, the scratch
    folder and the ISO clone on close; no emulator is left running."""

    boundary_timeout = 30.0

    def __init__(self, state: str | Path | None, log_dir: Path | None = None, *,
                 app: Path = FORK_APP, iso: Path = DEFAULT_ISO, renderer: int = 13,
                 scratch_base: Path | None = None, lock: Path | None = RUN_LOCK,
                 lock_wait: float = 3600.0, ready_timeout: float = 120.0,
                 lease_s: int = 1800, rtc: str = "2026-01-01 00:00:00", align: bool = True,
                 boundary_timeout: float | None = None, **_ignored):
        """state None = a cold boot of the disc (fixed RTC `rtc`, so boots repeat).
        A state load starts paused ([UI] StartPaused), so nothing runs between
        the load and the first request.  align False = stay exactly on the loaded
        state (no first step); True = one `run {ticks: 1}` to the main-loop top
        (a state saved at the loop top stays where it is: the fork counts the
        tick at the loaded PC without executing).  Either way step(n) advances
        exactly n frames (see _resume_to_boundary)."""
        self.align = align
        if boundary_timeout:
            self.boundary_timeout = boundary_timeout
        self.state = Path(state).resolve() if state is not None else None
        if self.state is not None and not self.state.exists():
            raise FileNotFoundError(self.state)
        self.rtc = rtc
        self.app, self.iso, self.renderer = Path(app), Path(iso), renderer
        self.emulator = self.app / "Contents/MacOS/PCSX2"
        self.scratch_base = Path(scratch_base or Path(os.environ.get("TMPDIR", "/tmp")) / "pcsx2-fork-session")
        documents = (Path.home() / "Documents").resolve()
        if documents in self.scratch_base.resolve().parents:
            raise ValueError(f"scratch must be outside ~/Documents: {self.scratch_base}")
        self.lock, self.lock_wait = (Path(lock) if lock else None), lock_wait
        self.log_dir = Path(log_dir) if log_dir else ROOT / "build/pcsx2_session"
        self.ready_timeout, self.lease_s = ready_timeout, lease_s
        self.data_dir = None
        self.sstates = None
        self.visible = False
        self.env = {}
        self.proc = None
        self.pid: int | None = None
        self.pine: Pine | None = None
        self.debug = DebugServer()
        self.frames_stepped = 0
        self.hello: dict = {}
        self._fs = None
        self._inst = None
        self._client = None
        self._locked = False
        self._snaps = 0
        self._iso_clone = None
        self._digest = hashlib.sha256(self.state.read_bytes()).hexdigest() if self.state else None
        self._ee_cycle = None   # EE cycle of the last stop (a tick stop without execution is not a frame)

    def _take_lock(self) -> None:
        if self.lock is None:
            return
        deadline = time.monotonic() + self.lock_wait
        while True:
            try:
                self.lock.mkdir()
                self._locked = True
                return
            except FileExistsError:
                if time.monotonic() > deadline:
                    raise RuntimeError(f"run lock {self.lock} still held after {self.lock_wait:.0f} s")
                time.sleep(2)

    def _wait_no_emulator(self) -> None:
        """Runs that predate the lock (legacy route_capture) share PINE's socket
        and port 21512: never start while any PCSX2 emulator process exists."""
        deadline = time.monotonic() + self.lock_wait
        while subprocess.run(["pgrep", "-f", "^[^ ]*PCSX2.app/Contents/MacOS/PCSX2"],
                             capture_output=True).returncode == 0:
            if time.monotonic() > deadline:
                raise RuntimeError("another PCSX2 emulator is still running")
            time.sleep(2)

    def _start(self) -> "ForkSession":
        if str(FORK_PY) not in sys.path:
            sys.path.insert(0, str(FORK_PY))
        from pcsx2dbg.launcher import Session, LaunchConfig   # the fork's MIT launcher
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self._take_lock()
        self._wait_no_emulator()
        self.scratch_base.mkdir(parents=True, exist_ok=True)
        self._fs = Session(scratch_base=self.scratch_base, lock=None)
        self._fs.open()
        # A cold boot runs the disc's own boot ELF (an -elf override on a cold boot
        # leaves the EE in the kernel: "Failed to read ELF"); a state load gets
        # -elf and starts paused, so nothing runs before the first request.
        cfg = LaunchConfig(app=self.app, iso=self.iso, elf=ELF if self.state else None,
                           statefile=self.state, renderer=self.renderer, unlimited=True,
                           lease_s=self.lease_s, rtc=None if self.state else self.rtc,
                           ini_overrides={"UI": {"StartPaused": "true"}} if self.state else {})
        self._inst = self._fs.launch("orig", cfg)
        self.pid = self._inst.pid
        self._client = c = self._inst.client()
        c.wait_vm(self.ready_timeout)
        self.hello = c.call("hello").get("emulator", {})
        self.debug = DebugServer(self._inst.port)
        self.load_state_info = c.call("state")
        c.call("halt")
        self.debug.call({"cmd": "pad_set", "clear": True})
        c.call("set_tick_pc", pc=LOOP_TOP)
        deadline = time.monotonic() + self.ready_timeout
        while True:
            try:
                self.pine = Pine(self._inst.pine_socket)
                break
            except OSError:
                if time.monotonic() > deadline:
                    raise
                time.sleep(0.1)
        (self.log_dir / "launch.log").write_text(json.dumps(
            {"instance": self._inst.info(), "hello": self.hello, "state": str(self.state)}, indent=1) + "\n")
        self._ee_cycle = c.call("state").get("ee_cycle")
        if self.align:
            self._run_tick()
        return self

    def _run_tick(self, timeout: float | None = None) -> dict:
        timeout = timeout or self.boundary_timeout
        r = self._client.call("run", until={"ticks": 1}, timeout_s=timeout, timeout=timeout + 60)
        stop = r.get("stop", {})
        if stop.get("reason") != "tick":
            raise TimeoutError(f"frame boundary not reached: {stop}")
        executed = stop.get("ee_cycle") != self._ee_cycle
        self._ee_cycle = stop.get("ee_cycle")
        stop["executed"] = executed
        return stop

    def _resume_to_boundary(self, timeout: float | None = None) -> None:
        """One whole frame: run to the next main-loop top.  A state saved at the
        loop top loads with the PC on the tick PC, and the fork counts that tick
        at once: the first run {ticks: 1} after such a load stops where it
        started, with no EE cycle executed.  That stop is not a frame, so the
        run is repeated (once: the next one always executes)."""
        if not self._run_tick(timeout)["executed"]:
            if not self._run_tick(timeout)["executed"]:
                raise RuntimeError("frame step: two tick stops without executing")

    def _paused(self) -> bool:
        return bool(self._client.call("state").get("paused"))

    def write(self, address: int, data: bytes) -> None:
        self.debug.call({"cmd": "write_memory", "cpu": "ee", "address": hex(address),
                         "data": data.hex()})

    def snapshot(self, out_dir: str | Path, slot: int | None = None) -> dict:
        """Exact fork save (state_save) of the paused machine into out_dir/state.p2s,
        plus eeMemory.bin, gs.bin, scratchpad.bin and original.png (gs_field)."""
        out = Path(out_dir)
        out.mkdir(parents=True, exist_ok=True)
        self._snaps += 1
        rel = f"snap/{SERIAL}.snap{self._snaps:03d}.p2s"
        sv = self._client.call("state_save", path=rel, timeout=300)
        src = self._inst.root / rel
        for name, dst in (("eeMemory.bin", "eeMemory.bin"), ("GS.bin", "gs.bin"),
                          ("Scratchpad.bin", "scratchpad.bin")):
            (out / dst).write_bytes(extract_zstd_entry(src, name))
        field = self._client.call("gs_field", path=f"snap/field{self._snaps:03d}.png")
        shutil.copyfile(self._inst.root / f"snap/field{self._snaps:03d}.png", out / "original.png")
        shutil.move(str(src), out / "state.p2s")
        info = {"emulator": "fork", "fork_rev": self.hello.get("rev"), "fork_hash": self.hello.get("hash"),
                "save_version": sv.get("save_version"), "source_state": str(self.state),
                "source_sha256": self._digest, "frames_stepped": self.frames_stepped,
                "main_loop_counter": self.u32(FRAME_COUNTER), "vsync_counter": self.u32(VSYNC_COUNTER),
                "fork_vsync": sv.get("vsync"), "fork_tick": sv.get("tick"),
                "field": {k: field.get(k) for k in ("width", "height", "psm", "rgba_xxh3", "renderer")},
                "ee_sha256": hashlib.sha256((out / "eeMemory.bin").read_bytes()).hexdigest()}
        (out / "snapshot.json").write_text(json.dumps(info, indent=2) + "\n")
        return info

    def close(self) -> None:
        if self._fs is None and not self._locked:
            return
        try:
            if self._client is not None:
                try:
                    self.debug.call({"cmd": "pad_set", "clear": True})
                except Exception:
                    pass
            if self.pine is not None:
                try:
                    self.pine.s.close()
                except OSError:
                    pass
                self.pine = None
            if self._fs is not None:
                self._fs.close()             # shuts the instance down, removes the ISO clone
                d = self._fs.dir.resolve()
                if d.is_dir() and not d.is_symlink() and self.scratch_base.resolve() in d.parents:
                    shutil.rmtree(d)         # BIOS copies; absolute path inside our scratch base
        finally:
            self._fs = self._inst = self._client = None
            self.pid = None
            if self._locked and self.lock is not None:
                try:
                    self.lock.rmdir()
                except OSError:
                    pass
                self._locked = False
        if self.state and hashlib.sha256(self.state.read_bytes()).hexdigest() != self._digest:
            raise RuntimeError(f"source save state changed: {self.state}")


if __name__ == "__main__":
    import argparse
    ap = argparse.ArgumentParser(description="Step the original game and snapshot it.")
    ap.add_argument("state", help="source .p2s (never modified); with --emulator fork also a "
                                  "fork-states manifest name or alias (04, slot01, 14_roger_encounter)")
    ap.add_argument("--frames", type=int, default=1)
    ap.add_argument("--buttons", default="", help="comma list, e.g. CROSS,R1")
    ap.add_argument("--lx", type=lambda v: int(v, 0), default=0x7F)
    ap.add_argument("--ly", type=lambda v: int(v, 0), default=0x7F)
    ap.add_argument("--snapshot", help="output directory for a final snapshot")
    ap.add_argument("--visible", action="store_true", help="show the emulator window")
    ap.add_argument("--emulator", default=str(DEFAULT_EMULATOR),
                    help="emulator binary (default: the legacy app in build/startup-reference); "
                         "'fork' = the agent-debug fork through ForkSession (the state is resolved "
                         "through build/startup-reference/fork-states/manifest.json, the run lock "
                         "is taken, scratch is automatic)")
    ap.add_argument("--data-dir", type=Path,
                    help="opt-in scratch -datapath folder outside ~/Documents (another build, "
                         "e.g. the agent-debug fork); default: the legacy -portable launch")
    ap.add_argument("--renderer", type=int, default=13, help="with --data-dir: [EmuCore/GS] Renderer")
    a = ap.parse_args()
    names = [b for b in a.buttons.split(",") if b]
    if a.emulator == "fork":
        session = ForkSession(fork_state(a.state), renderer=a.renderer)
    else:
        session = OriginalSession(a.state, emulator=Path(a.emulator), visible=a.visible,
                                  data_dir=a.data_dir, renderer=a.renderer)
    with session as s:
        counters = s.step(a.frames, buttons=names, lx=a.lx, ly=a.ly)
        print(json.dumps({"counters": [counters[0], counters[-1]] if counters else []}))
        if a.snapshot:
            print(json.dumps(s.snapshot(a.snapshot), indent=2))
