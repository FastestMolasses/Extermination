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
LOOP_TOP = 0x001AAF28
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
    def __init__(self):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.s.settimeout(5)
        self.s.connect(str(Path(os.environ["TMPDIR"]) / "pcsx2.sock"))

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
                 ready_timeout: float = 45.0, visible: bool = False):
        self.state = Path(state).resolve()
        if not self.state.exists():
            raise FileNotFoundError(self.state)
        self.emulator, self.iso = Path(emulator), Path(iso)
        self.log_dir = Path(log_dir) if log_dir else ROOT / "build/pcsx2_session"
        self.ready_timeout = ready_timeout
        self.proc: subprocess.Popen | None = None
        self.pid: int | None = None
        self.visible = visible
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
        args = ["-portable", "-fastboot", "-statefile", str(self.state), "-elf", str(ELF),
                "-logfile", str(self.log_dir / "emulator.log"), str(self.iso)]
        if self.visible:
            self.proc = subprocess.Popen([str(self.emulator), *args],
                                         stdout=self._log, stderr=subprocess.STDOUT)
            self.pid = self.proc.pid
        else:
            bundle = self.emulator.parents[2]          # .../PCSX2.app
            before = set(self._emulator_pids())
            subprocess.run(["open", "-g", "-j", "-n", "-a", str(bundle), "--args", *args],
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

    def close(self) -> None:
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
        if hashlib.sha256(self.state.read_bytes()).hexdigest() != self._digest:
            raise RuntimeError(f"source save state changed: {self.state}")

    # -- process helpers -------------------------------------------------------
    def _emulator_pids(self) -> list[int]:
        out = subprocess.run(["pgrep", "-f", str(self.emulator)], capture_output=True,
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
            used = {int(p.name.split(".")[-2]) for p in SSTATES.glob(f"{serial}.*.p2s")}
            slot = next(s for s in range(16, 64) if s not in used)
        target = SSTATES / f"{serial}.{slot:02d}.p2s"
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


if __name__ == "__main__":
    import argparse
    ap = argparse.ArgumentParser(description="Step the original game and snapshot it.")
    ap.add_argument("state", help="source .p2s (never modified)")
    ap.add_argument("--frames", type=int, default=1)
    ap.add_argument("--buttons", default="", help="comma list, e.g. CROSS,R1")
    ap.add_argument("--lx", type=lambda v: int(v, 0), default=0x7F)
    ap.add_argument("--ly", type=lambda v: int(v, 0), default=0x7F)
    ap.add_argument("--snapshot", help="output directory for a final snapshot")
    ap.add_argument("--visible", action="store_true", help="show the emulator window")
    a = ap.parse_args()
    names = [b for b in a.buttons.split(",") if b]
    with OriginalSession(a.state, visible=a.visible) as s:
        counters = s.step(a.frames, buttons=names, lx=a.lx, ly=a.ly)
        print(json.dumps({"counters": [counters[0], counters[-1]] if counters else []}))
        if a.snapshot:
            print(json.dumps(s.snapshot(a.snapshot), indent=2))
