"""ps2.py - play an EMREC recording on the ORIGINAL game in the MCP-enabled
PCSX2 and capture its frames (and optionally its audio) for the video
comparison.  docs/VIDEO_COMPARE.md.

Runs with the decomp .venv python (pcsx2_session needs zstandard).

How the original is driven (all through tools/pcsx2_session.py's hidden
emulator, its DebugServer and Pine):
  * Start: the user's title save state (slot 01).  The title is driven the way
    route_census drives it (Cross on NEW GAME until D_00810700 != 0); the
    recording's title inputs are not replayed (the port boots, the PS2 side
    starts from a save state).  A movie inside a main-loop iteration (the main
    loop blocks in it) is skipped with START when the recording skipped it.
  * From the first non-title tick on, every tick: sample the anchors at the
    main-loop top 0x001AAF28, compute the sync phase (emrec.phase_of), take
    the recorded pad from emrec.Scheduler, set it as the raw DualShock state
    (DebugServer pad_set, for raw readers such as the movie skip) and, at a
    breakpoint just after the main loop's input step C returns, write the
    processed pad block the port computes from the same pad (emrec.PadBlock)
    into 0x00810E57, 0x00810E64..67 and 0x00810E70..7B.  So the original's
    logic reads the recorded pad on the same tick the port does, without the
    two-iteration libpad latency of pad_set alone.
  * Frames: the field showing the state after tick t is drawn in iteration
    t + 1 and is the drawing context's FRAME buffer at the main-loop top of
    tick t + 3 (the field on screen then, untouched until that iteration
    draws; docs/CAPTURES_C7.md 5b).  At that loop top a save state goes to a
    free slot >= 40; its GS freeze is decoded (PSMCT32 de-swizzle) into a
    512x224 PNG and the slot file is deleted.  This needs PCSX2's software
    renderer (the hardware renderer keeps no pixels in GS memory); the tool
    switches [EmuCore/GS] Renderer 17 -> 13 for the session and back (user
    decision 2026-09-26: only during capture jobs).  --frames screenshot
    keeps the hardware renderer and uses the save state's 640x480 host
    screenshot instead.
  * Audio (--audio, off by default): PCSX2's SPU2 debug WAV log
    ([SPU2/Debug] Global_Enable and Log_WAVE_Output), switched on for the
    session and back (user decision 2026-10-01: only during comparison
    captures).  In the v2.6.3 build here it writes no file (2026-10-01:
    two sessions, frame-stepped and free-running), so the PS2 side has no
    audio and the videos are silent unless the port's sound is chosen.
  * Media-capture audio (--media-audio, off by default; user decision
    2026-10-09: a visible PCSX2 session may record audio): the session runs
    VISIBLE with PCSX2's Media Capture switched to audio only
    ([EmuCore/GS] EnableVideoCapture = false, EnableAudioCapture = true,
    CaptureContainer = wav, and the hotkey [Hotkeys] ToggleVideoCapture =
    Keyboard/End added; all restored after).  The capture is started from
    PCSX2's UI only (System > Video Capture, or the hotkey: End sent to the
    window), so the tool stops twice with the VM held at a main-loop top and
    waits on marker files in --out: at the
    first recorded tick it writes audio_start.ready and waits for
    audio_start.go (start the capture now), after the last tick it writes
    audio_stop.ready and waits for audio_stop.go (stop it).  The WAV PCSX2
    wrote is moved to audio_media.wav; audio_sync.json compares its sample
    count with the gates' vsync span (48000 / 59.94 samples per field) and
    audio.wav is written only when they agree.  2026-10-09: the hotkey
    reaches PCSX2, but its capture failed to load FFmpeg (docs/VIDEO_COMPARE.md
    "Audio"), so no WAV has been made this way yet.  --emulator-lib-path
    passes a folder of FFmpeg libraries to the emulator process only
    (DYLD_FALLBACK_LIBRARY_PATH); it must match the emulator's architecture:
    this PCSX2 build is x86_64 and Homebrew's FFmpeg 8 is arm64 only, so
    that pair is refused (checked 2026-10-09: the variable reached the
    process and the load still failed).
  * Lock: build/.pcsx2.lock (mkdir) is held for the whole session and always
    removed; the ini is restored before it is released.

Everything written is derived from the user's own disc and stays under
build/video_compare/.  The source save state is hashed before and after.
"""
from __future__ import annotations

import argparse
import concurrent.futures as cf
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

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(HERE))
import emrec  # noqa: E402

REFERENCE = ROOT / "build/startup-reference"
LIVE_INI = REFERENCE / "inis/PCSX2.ini"
SSTATES = REFERENCE / "portable-data/sstates"
SERIAL = "SCUS-97112 (0AE679AF)"
TITLE_STATE = SSTATES / f"{SERIAL}.01.p2s"
LOCK = ROOT / "build/.pcsx2.lock"
ELF = ROOT / "config/SCUS_971.12"
LOOP_TOP = 0x001AAF28
MAIN_LOOP = (0x001AAE40, 0x001AB1DC)      # func_001AAE40 (FINDINGS "ENGINE FRAME ANATOMY")
INPUT_STEP = 0x001B57E0                   # step C
FREE_SLOT_MIN = 40

# (name, address, bytes): one Pine request per tick.
SPANS = [("counter", emrec.ADDR["counter"], 4), ("area", emrec.ADDR["area"], 4),
         ("bd8", emrec.ADDR["bd8"], 4), ("spad", emrec.ADDR["spad"], 8),
         ("task0", emrec.ADDR["task0"], 0x20), ("pos", emrec.ADDR["pos"], 0x10),
         ("yaw", emrec.ADDR["yaw"], 4), ("movie", emrec.ADDR["movie"], 4),
         ("vsync", emrec.ADDR["vsync"], 4), ("fade", 0x0028A9A0, 4),
         ("pad", emrec.ADDR["pad_struct"], 0x40), ("counter2", emrec.ADDR["counter"], 4)]


def log(*a) -> None:
    print("ps2:", *a, flush=True)


# --------------------------------------------------------------- lock + ini

def emulator_running() -> bool:
    """A PCSX2 process (by process name: a full-command-line match would also
    find other lanes' shell commands that merely mention the binary's path)."""
    return subprocess.run(["pgrep", "-x", "PCSX2"], capture_output=True).returncode == 0


class Lock:
    def __init__(self, poll: float = 30.0):
        self.poll, self.held = poll, False

    def __enter__(self):
        waited = 0.0
        while True:
            try:
                os.mkdir(LOCK)
                self.held = True
                break
            except FileExistsError:
                if waited == 0:
                    log(f"waiting for the PCSX2 lock {LOCK} (another capture lane is using PCSX2)")
                time.sleep(self.poll)
                waited += self.poll
        log(f"lock acquired after {waited:.0f} s")
        return self

    def __exit__(self, *exc):
        if self.held:
            try:
                os.rmdir(LOCK)
            except OSError as e:
                log(f"WARNING: could not remove the lock {LOCK}: {e}")
            self.held = False
            log("lock released")


def ini_get(text: str, section: str, key: str) -> str | None:
    cur = None
    for line in text.splitlines():
        s = line.strip()
        if s.startswith("["):
            cur = s[1:-1]
        elif cur == section and s.split("=")[0].strip() == key:
            return s.split("=", 1)[1].strip()
    return None


def ini_set(text: str, section: str, key: str, value: str) -> str:
    out, cur, done = [], None, False
    for line in text.splitlines(keepends=True):
        s = line.strip()
        if s.startswith("["):
            cur = s[1:-1]
        elif cur == section and s.split("=")[0].strip() == key and not done:
            line = f"{key} = {value}\n"
            done = True
        out.append(line)
    if not done:
        raise KeyError(f"[{section}] {key} not in {LIVE_INI}")
    return "".join(out)


def ini_add(text: str, section: str, key: str, value: str) -> str:
    """Insert `key = value` after the last entry of [section] (the key must be absent)."""
    lines = text.splitlines(keepends=True)
    cur, last = None, None
    for i, line in enumerate(lines):
        s = line.strip()
        if s.startswith("["):
            cur = s[1:-1]
            if cur == section:
                last = i
        elif cur == section and s:
            last = i
    if last is None:
        raise KeyError(f"[{section}] not in {LIVE_INI}")
    return "".join(lines[:last + 1] + [f"{key} = {value}\n"] + lines[last + 1:])


def ini_del(text: str, section: str, key: str) -> str:
    out, cur = [], None
    for line in text.splitlines(keepends=True):
        s = line.strip()
        if s.startswith("["):
            cur = s[1:-1]
        elif cur == section and s.split("=")[0].strip() == key:
            continue
        out.append(line)
    return "".join(out)


class IniSwitch:
    """Switch the named ini keys for one PCSX2 session and restore them after
    the emulator has exited (PCSX2 rewrites its ini on exit).  Only these keys
    are touched; the pre-session copy and its sha256 go to `keep`."""

    def __init__(self, changes: dict, keep: Path):
        self.changes, self.keep, self.pre = changes, keep, {}

    def __enter__(self):
        if emulator_running():
            raise RuntimeError("a PCSX2 process is running; the ini can only be edited while it is stopped")
        self.keep.mkdir(parents=True, exist_ok=True)
        text = LIVE_INI.read_text()
        (self.keep / "PCSX2.ini.pre").write_text(text)
        (self.keep / "PCSX2.ini.pre.sha256").write_text(hashlib.sha256(text.encode()).hexdigest() + "\n")
        for (sec, key), val in self.changes.items():
            self.pre[(sec, key)] = ini_get(text, sec, key)
            text = (ini_set(text, sec, key, val) if self.pre[(sec, key)] is not None
                    else ini_add(text, sec, key, val))         # an absent key is added, and removed after
        if self.changes:
            LIVE_INI.write_text(text)
            log("ini switched for the session:", {f"[{s}] {k}": f"{self.pre[(s, k)]} -> {v}"
                                                  for (s, k), v in self.changes.items()})
        return self

    def __exit__(self, *exc):
        if not self.changes:
            return
        for _ in range(100):
            if not emulator_running():
                break
            time.sleep(0.1)
        else:
            log("WARNING: PCSX2 still running; restoring the ini anyway")
        text = LIVE_INI.read_text()
        for (sec, key), val in self.pre.items():
            text = ini_set(text, sec, key, val) if val is not None else ini_del(text, sec, key)
        LIVE_INI.write_text(text)
        pre = (self.keep / "PCSX2.ini.pre").read_text().splitlines()
        now = text.splitlines()
        diff = [f"-{a}\n+{b}" for a, b in zip(pre, now) if a != b]
        if len(pre) != len(now):
            diff.append(f"line count {len(pre)} -> {len(now)}")
        (self.keep / "ini_restore_diff.txt").write_text("\n".join(diff) + "\n")
        log("ini restored:", {f"[{s}] {k}": v for (s, k), v in self.pre.items()},
            f"(remaining differences from the pre copy: {len(diff)} line(s), see ini_restore_diff.txt)")


# --------------------------------------------------------------- debugger

class Debug:
    """DebugServer with one persistent connection (route_census does the same)."""

    def __init__(self, port: int = 21512):
        self.port, self.sock, self.buf = port, None, b""

    def call(self, cmd: dict) -> dict:
        for attempt in range(2):
            try:
                if self.sock is None:
                    self.sock = socket.create_connection(("127.0.0.1", self.port), timeout=30)
                    self.buf = b""
                self.sock.sendall((json.dumps(cmd) + "\n").encode())
                while b"\n" not in self.buf:
                    chunk = self.sock.recv(1 << 16)
                    if not chunk:
                        raise EOFError("DebugServer closed the connection")
                    self.buf += chunk
                line, self.buf = self.buf.split(b"\n", 1)
                resp = json.loads(line)
                break
            except (OSError, EOFError):
                self.close()
                if attempt:
                    raise
        if not resp.get("ok"):
            raise RuntimeError(resp)
        return resp

    def close(self) -> None:
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
        self.sock = None


def post_input_pc() -> int:
    """The main loop's return address after its call to the input step C:
    the call instruction's address + 8, found by scanning func_001AAE40 in
    the user's ELF for the call to 0x001B57E0 (no code is reproduced)."""
    data = ELF.read_bytes()
    phoff, = struct.unpack_from("<I", data, 0x1C)
    off, vaddr = struct.unpack_from("<II", data, phoff + 4)
    want = 0x0C000000 | (INPUT_STEP >> 2)
    for a in range(MAIN_LOOP[0], MAIN_LOOP[1], 4):
        if struct.unpack_from("<I", data, off + a - vaddr)[0] == want:
            return a + 8
    raise RuntimeError("the main loop's input call was not found in the ELF")


# ------------------------------------------------------------- the frames

PAGE32 = [0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23,
          8, 9, 12, 13, 24, 25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31]
COL32 = [0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15]
_MAPS: dict = {}


def word_map(w: int, h: int, fbw_px: int):
    """PSMCT32 local-memory word index of each pixel (public GS layout facts;
    docs/CAPTURES_C7.md 5b "Decode")."""
    import numpy as np
    key = (w, h, fbw_px)
    if key not in _MAPS:
        ppr = fbw_px // 64
        y, x = np.mgrid[0:h, 0:w]
        page = (y // 32) * ppr + x // 64
        blk = np.array(PAGE32)[(y % 32 // 8) * 8 + (x % 64 // 8)]
        px = (y % 8) * 8 + x % 8
        _MAPS[key] = page * 2048 + blk * 64 + (px // 16) * 16 + np.array(COL32)[px % 16]
    return _MAPS[key]


GS_FREEZE_VRAM = 425        # bytes before GS local memory in the GS freeze (tools/gs_vram.py)


def decode_state(p2s: Path, out_png: Path, mode: str) -> dict:
    """One save state taken at the main-loop top of tick t + 3 -> one PNG of
    tick t's field.  mode gs: the drawing context's FRAME buffer (GS freeze,
    context 0; both contexts name the same buffer in the first level).  At a
    loop top that buffer holds the field drawn in iteration t + 1 from the
    list tick t built, the field on screen, untouched until the iteration
    starts (docs/CAPTURES_C7.md 5b; one iteration later than DISPFB2, so the
    frame's late writes such as the letterbox bands have landed).  mode
    screenshot: the save state's 640x480 host screenshot."""
    import numpy as np
    from parse_pcsx2_state import extract_zstd_entry
    if mode == "screenshot":
        with zipfile.ZipFile(p2s) as z:
            out_png.write_bytes(z.read("Screenshot.png"))
        return {"source": "screenshot"}
    gs = extract_zstd_entry(p2s, "GS.bin")
    ctx0 = 4 + 15 * 8                                   # the freeze's context 0 registers (c7cap_partb)
    frame, = struct.unpack_from("<Q", gs, ctx0 + 10 * 8)
    scissor, = struct.unpack_from("<Q", gs, ctx0 + 6 * 8)
    fbp, fbw, psm = frame & 0x1FF, ((frame >> 16) & 0x3F) * 64, (frame >> 24) & 0x3F
    width, height = ((scissor >> 16) & 0x7FF) + 1, ((scissor >> 48) & 0x7FF) + 1
    info = {"fbp": fbp, "fbw": fbw, "psm": psm, "width": width, "height": height}
    if psm != 0 or fbw <= 0 or width > fbw or height > 512 or width < 64 or height < 64:
        with zipfile.ZipFile(p2s) as z:
            out_png.write_bytes(z.read("Screenshot.png"))
        info["source"] = "screenshot (unsupported drawing setup)"
        return info
    vram = np.frombuffer(gs, dtype="<u4", count=0x100000, offset=GS_FREEZE_VRAM)
    px = vram[fbp * 2048 + word_map(width, height, fbw)]
    rgb = px.astype("<u4").view(np.uint8).reshape(height, width, 4)[..., :3]
    emrec.write_png(out_png, np.ascontiguousarray(rgb))
    info["source"] = "gs"
    return info


# ------------------------------------------------------------- the session

class Player:
    def __init__(self, rec: emrec.Recording, out: Path, stride: int, frames: str, audio: bool,
                 state: Path, tail: int, title_limit: int, media: bool = False, media_wait: float = 1800.0,
                 emulator_env: dict[str, str] | None = None):
        self.rec, self.out, self.stride, self.frames, self.audio = rec, out, stride, frames, audio
        self.state, self.tail, self.title_limit = state, tail, title_limit
        self.media, self.media_wait, self.gates = media, media_wait, {}
        self.emulator_env = dict(emulator_env or {})
        self.last_status: dict = {}
        self.rows: list[dict] = []
        self.extra: list[dict] = []
        self.notes: list[str] = []
        self.pending: dict[int, tuple] = {}        # loop-top counter -> (tick row index, cap index)
        self.captures = 0
        self.pool = cf.ThreadPoolExecutor(max_workers=3)
        self.futures: list = []
        self.slot_cycle = FREE_SLOT_MIN
        self.raw_pad = None
        self.post_pc = post_input_pc()
        segs = rec.segments()
        self.first = emrec.first_play_segment(rec)
        title_ticks = sum(len(rows) for _ph, rows in segs[:self.first])
        movies = rec.movies()
        before = [m for m in movies if m[0] <= title_ticks]
        self.title_movie_skip = before[-1][1] if before else True
        self.later_movies = [m[1] for m in movies if m[0] > title_ticks]
        self.movie_count = 0

    # -- low level ---------------------------------------------------------
    def sample(self) -> dict:
        body = b"".join(struct.pack("<BI", 2, a + i) for _n, a, n in SPANS for i in range(0, n, 4))
        for _ in range(5):
            data = self.s.pine.request(body)
            raw, off = {}, 0
            for name, _a, n in SPANS:
                raw[name] = data[off:off + n]
                off += n
            if raw["counter"] == raw["counter2"]:
                break
        else:
            raise RuntimeError("inconsistent Pine sample")
        t0 = raw["task0"]
        x, y, z, _w = struct.unpack("<4f", raw["pos"])
        return {"counter": struct.unpack("<I", raw["counter"])[0], "area": raw["area"][0],
                "sub": raw["area"][1], "entry": raw["area"][2], "bd8": raw["bd8"][0],
                "s8d": raw["spad"][1], "t9": t0[9], "tb": t0[0xB], "x": x, "y": y, "z": z,
                "yaw": struct.unpack("<f", raw["yaw"])[0], "movie": raw["movie"][0],
                "vsync": struct.unpack("<I", raw["vsync"])[0], "fade": raw["fade"],
                "pad": raw["pad"]}

    def pine_write(self, items: list[tuple[int, bytes]]) -> None:
        """Pine writes (8-bit), one request: op 4 = Write8."""
        body = b"".join(struct.pack("<BIB", 4, a + i, b) for a, data in items for i, b in enumerate(data))
        self.s.pine.request(body)

    def set_raw(self, pad) -> None:
        if pad != self.raw_pad:
            btn, lx, ly, rx, ry = pad
            self.dbg.call({"cmd": "pad_set", "buttons": btn, "lx": lx, "ly": ly, "rx": rx, "ry": ry})
            self.raw_pad = pad

    def status(self) -> dict:
        try:
            st = self.dbg.call({"cmd": "status"})
        except OSError as exc:
            if not emulator_running():
                raise RuntimeError("PCSX2 exited during the session (its DebugServer is gone)") from exc
            raise
        return st.get("data", st)

    def resume_until_pause(self, on_slow=None, timeout: float = 120.0) -> int:
        """Resume and wait for the next breakpoint; returns the pc.  `on_slow`
        is called every 0.5 s while the pause has not come (a movie or a
        blocking load inside the iteration)."""
        self.dbg.call({"cmd": "resume"})
        t0 = time.monotonic()
        last_cycles, still = None, t0
        next_slow = t0 + 0.5
        while True:
            d = self.status()
            if d.get("paused"):
                self.last_status = d
                return int(d["pc"], 16)
            now = time.monotonic()
            if on_slow and now > next_slow:
                next_slow = now + 0.5
                on_slow()
            if now - t0 > timeout:
                raise TimeoutError(f"no pause within {timeout} s")
            cyc = d.get("cycles")
            if cyc != last_cycles:
                last_cycles, still = cyc, now
            elif now - still > 3.0:      # a stopped VM that reports running: resume again
                self.dbg.call({"cmd": "resume"})
                still = now
            time.sleep(0.001)

    # -- snapshots ---------------------------------------------------------
    def free_slot(self) -> int:
        for _ in range(64):
            slot = self.slot_cycle
            self.slot_cycle = FREE_SLOT_MIN + (self.slot_cycle - FREE_SLOT_MIN + 1) % (64 - FREE_SLOT_MIN)
            if not (SSTATES / f"{SERIAL}.{slot:02d}.p2s").exists():
                return slot
            time.sleep(0.05)
        raise RuntimeError("no free save-state slot >= 40")

    def capture(self, cap: int, tick_row: int) -> None:
        slot = self.free_slot()
        target = SSTATES / f"{SERIAL}.{slot:02d}.p2s"
        self.s.pine.save(slot)
        deadline = time.monotonic() + 30
        size = -1
        while time.monotonic() < deadline:
            if target.exists():
                try:
                    with zipfile.ZipFile(target) as z:
                        complete = "GS.bin" in z.namelist()
                except (zipfile.BadZipFile, OSError):
                    complete = False
                now = target.stat().st_size
                if complete and now == size:          # a complete archive whose size held for one poll
                    break
                size = now
            time.sleep(0.02)
        else:
            raise RuntimeError("save state was not written")
        tmp = self.out / "tmp" / f"cap{cap:06d}.p2s"
        shutil.move(str(target), tmp)
        png = self.out / "frames" / f"{cap:06d}.png"
        self.futures.append((cap, tick_row, self.pool.submit(self._decode, tmp, png)))

    def _decode(self, tmp: Path, png: Path) -> dict:
        try:
            return decode_state(tmp, png, self.frames)
        finally:
            tmp.unlink(missing_ok=True)

    # -- driving -----------------------------------------------------------
    def run(self) -> None:
        sys.path.insert(0, str(ROOT / "tools"))
        from pcsx2_session import OriginalSession
        (self.out / "frames").mkdir(parents=True, exist_ok=True)
        (self.out / "tmp").mkdir(parents=True, exist_ok=True)
        self.dbg = Debug()
        attempts = 0
        while True:
            attempts += 1
            sess = OriginalSession(self.state, log_dir=self.out / "pcsx2_logs", visible=self.media,
                                   env=self.emulator_env)
            try:
                self.s = sess.__enter__()
                break
            except Exception as exc:          # start-up races: retry like route_capture
                log(f"start attempt {attempts} failed: {exc}")
                if attempts >= 5:
                    raise
                time.sleep(2)
        try:
            self.s.debug = self.dbg
            self.dbg.call({"cmd": "set_breakpoint", "address": self.post_pc,
                           "description": "video_compare pad block"})
            self._drive()
        finally:
            try:
                self.dbg.call({"cmd": "remove_breakpoint", "address": self.post_pc})
            except Exception:
                pass
            self.s.close()
            self.dbg.close()
            for cap, row, fut in self.futures:
                try:
                    info = fut.result()
                    self.rows[row]["cap"] = cap
                    self.extra[row]["frame"] = info
                except Exception as exc:
                    self.notes.append(f"capture {cap} failed: {exc}")
            self.pool.shutdown()
            shutil.rmtree(self.out / "tmp", ignore_errors=True)

    def _wait_loop_top(self, on_slow=None) -> None:
        while True:
            pc = self.resume_until_pause(on_slow=on_slow, timeout=1800.0)
            if pc == LOOP_TOP:
                return
            if pc == self.post_pc:          # the title / movie path: nothing to write
                continue
            raise RuntimeError(f"unexpected pause at {pc:#x}")

    def _movie_hook(self, skip: bool):
        def hook():
            if not self.movie_seen and self.sample()["movie"] == 1:
                self.movie_count += 1
                log(f"movie inside the iteration: {'skipping with START' if skip else 'playing it'}")
                if skip:
                    self.set_raw((emrec.START, 0x80, 0x80, 0x80, 0x80))
                self.movie_seen = True
        return hook

    def _gate(self, name: str, row: dict) -> None:
        """--media-audio: hold the VM at this loop top, write audio_<name>.ready
        and wait for audio_<name>.go (the capture is started / stopped
        meanwhile: the End hotkey sent to the PCSX2 window, or its menu)."""
        if not self.media or (name == "stop" and not self.gates.get("start", {}).get("go")):
            return
        st = self.status()
        info = {"counter": row["counter"], "vsync": row["vsync"], "cycles": st.get("cycles"),
                "pc": st.get("pc"), "paused": st.get("paused"), "written": time.time()}
        go = self.out / f"audio_{name}.go"
        go.unlink(missing_ok=True)
        (self.out / f"audio_{name}.ready").write_text(json.dumps(info) + "\n")
        log(f"audio gate '{name}': VM held at counter {row['counter']}; waiting for {go}")
        t0 = time.monotonic()
        while not go.exists():
            if time.monotonic() - t0 > self.media_wait:
                self.notes.append(f"audio: no {go.name} within {self.media_wait:.0f} s; the PS2 side stays silent")
                info["go"] = False
                self.gates[name] = info
                return
            time.sleep(0.5)
        info["go"], info["waited_s"] = True, round(time.monotonic() - t0, 1)
        self.gates[name] = info
        log(f"audio gate '{name}' released after {info['waited_s']} s")

    def _drive(self) -> None:
        # Title: Cross on NEW GAME (route_census's title driver), the movie skipped.
        n, presses = 0, 0
        self.set_raw(emrec.NEUTRAL)
        self.first_vsync = self.sample()["vsync"]
        while True:
            row = self.sample()
            if emrec.phase_of(row["area"], row["bd8"], row["t9"], row["s8d"]) != "T":
                break
            if n in (5, 125, 245, 365) and presses < 4 and row["fade"] == b"\0\0\0\0":
                self.set_raw((0x4000, 0x80, 0x80, 0x80, 0x80))
                presses += 1
            elif n % 120 == 9:
                self.set_raw(emrec.NEUTRAL)
            self.movie_seen = False
            self._wait_loop_top(on_slow=self._movie_hook(self.title_movie_skip))
            if self.movie_seen:
                self.set_raw(emrec.NEUTRAL)
            n += 1
            if n > self.title_limit:
                raise TimeoutError("the title did not start a New Game")
        log(f"New Game committed after {n} title ticks ({presses} Cross press(es)); counter {row['counter']}")
        self._gate("start", row)
        sched = emrec.Scheduler(self.rec, first=self.first)
        model = emrec.PadBlock.from_memory(row["pad"])
        self.movie_count = 0
        pad = emrec.NEUTRAL
        tail = 0                       # loop tops stepped after the recording was used up
        t = 0
        while True:
            row = self.sample()
            if row["counter"] in self.pending:              # the field of tick t-2 is displayed now
                cap, tick_row = self.pending.pop(row["counter"])
                self.capture(cap, tick_row)
            phase = emrec.phase_of(row["area"], row["bd8"], row["t9"], row["s8d"])
            self.end_vsync = row["vsync"]               # the WAV ends here (the VM stays paused)
            if sched.done:
                if not self.pending or tail >= self.tail:
                    break
                tail += 1
                seg, off = -1, -1                          # keep holding the last pad
            else:
                pad, seg, off = sched.next(phase)
            model.unpack(pad)
            self.set_raw(pad)
            r = {"step": t, "counter": row["counter"], "mv": 0, "ph": phase, "btn": pad[0], "lx": pad[1],
                 "ly": pad[2], "rx": pad[3], "ry": pad[4], "area": row["area"], "sub": row["sub"],
                 "entry": row["entry"], "bd8": row["bd8"], "s8d": row["s8d"], "t9": row["t9"],
                 "tb": row["tb"], "x": row["x"], "y": row["y"], "z": row["z"], "yaw": row["yaw"],
                 "seg": seg, "off": off, "cap": -1, "af": -1}
            self.rows.append(r)
            ex = {"vsync": row["vsync"], "cycles": self.last_status.get("cycles")}
            self.extra.append(ex)
            if seg >= 0 and off % self.stride == 0:
                # the field on screen at loop top t + 3 shows tick t (decode_state)
                self.pending[row["counter"] + 3] = (self.captures, len(self.rows) - 1)
                self.captures += 1
            # step C runs; then the recorded pad block replaces the original's own unpack
            pc = self.resume_until_pause(timeout=120.0)
            if pc != self.post_pc:
                raise RuntimeError(f"expected the post-input breakpoint, paused at {pc:#x}")
            own = self.s.pine.read(0x00810E40, 0x40)       # the original's own unpack of the raw pad
            ex["own_block"] = own[0x30:0x3C].hex()
            ex["own_sticks_gait"] = own[0x24:0x28].hex() + f"{own[0x17]:02x}"
            ex["model_block"] = model.block_bytes().hex()
            ex["model_sticks_gait"] = bytes([model.lx, model.ly, model.rx, model.ry, model.gait]).hex()
            self.pine_write([(0x00810E57, bytes([model.gait])),
                             (0x00810E64, bytes([model.lx, model.ly, model.rx, model.ry])),
                             (0x00810E70, model.block_bytes())])
            self.movie_seen = False
            later_skip = (self.later_movies[self.movie_count] if self.movie_count < len(self.later_movies)
                          else True)
            self._wait_loop_top(on_slow=self._movie_hook(later_skip))
            if self.movie_seen:          # the port's movie steps unpack the movie pad at least twice
                for _ in range(2):
                    model.unpack((emrec.START if later_skip else 0, 0x80, 0x80, 0x80, 0x80))
                ex["movie"] = True
            t += 1
            if t % 200 == 0:
                log(f"tick {t}: counter {row['counter']} phase {phase} segment {seg} offset {off}, "
                    f"{self.captures} captures queued")
            if sched.done and tail == 0 and "used_up" not in self.__dict__:
                self.used_up = t
                log(f"recording used up after {t} ticks (counter {row['counter']})")
        self._gate("stop", row)
        self.notes += sched.notes


def run(args) -> int:
    rec = emrec.read(args.recording)
    out = Path(args.out)
    if out.exists() and not args.resume_dir:
        shutil.rmtree(out)
    out.mkdir(parents=True, exist_ok=True)
    changes = {}
    if args.frames == "gs":
        changes[("EmuCore/GS", "Renderer")] = "13"
    if args.audio:
        changes[("SPU2/Debug", "Global_Enable")] = "true"
        changes[("SPU2/Debug", "Log_WAVE_Output")] = "true"
    if args.media_audio:
        changes[("EmuCore/GS", "EnableVideoCapture")] = "false"
        changes[("EmuCore/GS", "EnableAudioCapture")] = "true"
        changes[("EmuCore/GS", "CaptureContainer")] = "wav"
        # a hotkey for the capture: PCSX2's menu items report disabled while
        # it is in the background (2026-10-09), a key sent to its window may work
        changes[("Hotkeys", "ToggleVideoCapture")] = MEDIA_HOTKEY
    emulator_env = {}
    if args.emulator_lib_path:
        lib = check_ffmpeg_dir(Path(args.emulator_lib_path))
        # the emulator process only; dyld falls back here only where PCSX2's
        # own libraries are not found (the app is ad-hoc signed without the
        # hardened runtime, so dyld honours it); nothing in the install changes
        emulator_env["DYLD_FALLBACK_LIBRARY_PATH"] = str(lib)
        log(f"emulator library fallback (this session's emulator process only): {lib}")
    state = Path(args.state)
    digest = hashlib.sha256(state.read_bytes()).hexdigest()
    player = Player(rec, out, args.stride, args.frames, args.audio, state, args.tail, args.title_limit,
                    media=args.media_audio, media_wait=args.media_wait, emulator_env=emulator_env)
    t0 = time.monotonic()
    marker = audio_marker(out) if (args.audio or args.media_audio) else None

    def on_term(signum, frame):
        raise KeyboardInterrupt(f"signal {signum}")
    old = signal.signal(signal.SIGTERM, on_term)
    try:
        with Lock(args.lock_poll):
            t_lock = time.monotonic()
            with IniSwitch(changes, out / "ini"):
                try:
                    player.run()
                finally:
                    for _ in range(100):
                        if not emulator_running():
                            break
                        time.sleep(0.1)
            if args.audio:
                collect_audio(out, marker, player)
            if args.media_audio:
                collect_media_audio(out, marker, player)
    finally:
        signal.signal(signal.SIGTERM, old)
        left = emulator_running()
        log(f"no emulator process left: {not left}")
    if hashlib.sha256(state.read_bytes()).hexdigest() != digest:
        raise RuntimeError(f"source save state changed: {state}")
    emrec.write(out / "ps2.rec", {"source": "pcsx2", "mode": "playback-log", "state": state.name,
                                  "stride": args.stride, "frames": args.frames}, player.rows)
    (out / "ps2_extra.json").write_text(json.dumps({
        "notes": player.notes, "seconds_total": round(time.monotonic() - t0, 1),
        "seconds_session": round(time.monotonic() - t_lock, 1), "captures": player.captures,
        "first_vsync": getattr(player, "first_vsync", None), "end_vsync": getattr(player, "end_vsync", None),
        "audio_gates": player.gates,
        "rows": player.extra}, indent=0) + "\n")
    log(f"{len(player.rows)} ticks, {player.captures} frames, notes: {player.notes}")
    return 0


# ------------------------------------------------------------------ audio

AUDIO_DIRS = [REFERENCE, Path.home() / "Library/Application Support/PCSX2", Path("/tmp"),
              Path(os.environ.get("TMPDIR", "/tmp"))]


def audio_marker(out: Path) -> Path:
    marker = out / "audio_marker"
    marker.write_text("session start\n")
    time.sleep(1.1)                       # mtime granularity: files written later are strictly newer
    return marker


def collect_audio(out: Path, marker: Path, player: "Player") -> None:
    """Move the WAV file(s) PCSX2's SPU2 debug log wrote during the session
    (found by mtime: its log folder, the portable folder, temp folders)."""
    res = subprocess.run(["find", *[str(d) for d in AUDIO_DIRS if d.exists()], "-type", "f", "-iname", "*.wav",
                          "-newer", str(marker)], capture_output=True, text=True).stdout.split("\n")
    fresh = sorted({p for p in res if p and "/video_compare/" not in p})
    if not fresh:
        player.notes.append("audio: PCSX2 wrote no WAV file; the PS2 side stays silent")
        return
    for i, p in enumerate(fresh):
        dst = out / ("audio.wav" if i == 0 else f"audio_{i}.wav")
        shutil.move(p, dst)
        player.notes.append(f"audio: {p} -> {dst.name} ({dst.stat().st_size} bytes)")


# the FFmpeg majors this PCSX2 build asks for (its "Failed to load FFmpeg"
# message, 2026-10-09); --emulator-lib-path must hold exactly these
FFMPEG_MAJORS = {"avcodec": 62, "avformat": 62, "avutil": 60, "swscale": 9, "swresample": 6}


def check_ffmpeg_dir(lib: Path) -> Path:
    """Fault unless lib holds lib<name>.<major>.dylib for every library
    PCSX2's Media Capture loads (the leaf names it dlopens)."""
    lib = lib.absolute()            # keep a stable opt/ path (not the versioned Cellar one)
    missing = [f"lib{n}.{m}.dylib" for n, m in FFMPEG_MAJORS.items() if not (lib / f"lib{n}.{m}.dylib").is_file()]
    if missing:
        raise FileNotFoundError(f"--emulator-lib-path {lib}: missing {missing}")
    # dyld only loads a library built for the emulator's own architecture:
    # this PCSX2 build is x86_64 (Rosetta), Homebrew's /opt/homebrew FFmpeg is
    # arm64 only (2026-10-09), so that pair can never load; fault up front
    emu = REFERENCE / "PCSX2.app/Contents/MacOS/PCSX2"
    def archs(f: Path) -> set[str]:
        return set(subprocess.run(["lipo", "-archs", str(f)], capture_output=True, text=True,
                                  check=True).stdout.split())
    want = archs(emu)
    wrong = {f"lib{n}.{m}.dylib": sorted(archs(lib / f"lib{n}.{m}.dylib"))
             for n, m in FFMPEG_MAJORS.items() if not want & archs(lib / f"lib{n}.{m}.dylib")}
    if wrong:
        raise RuntimeError(f"--emulator-lib-path {lib}: the emulator is {sorted(want)}, "
                           f"these libraries are not: {wrong}")
    return lib


MEDIA_HOTKEY = "Keyboard/End"     # unbound; a key a background app can be sent
MEDIA_DIRS = [REFERENCE / "portable-data/videos", REFERENCE / "portable-data", Path.home() / "Movies",
              Path.home() / "Desktop", Path.home() / "Documents"]


def collect_media_audio(out: Path, marker: Path, player: "Player") -> None:
    """--media-audio: take the WAV PCSX2's Media Capture wrote (the save path
    typed into its dialog: --out itself, moved; else found by mtime in the
    capture folders, COPIED so nothing outside build/ is ever moved or
    deleted) as audio_media.wav and measure it against the session's span:
    the vsync counter between the two gates (one NTSC field = 48000 / 59.94
    samples).  audio.wav (what compose.py reads) is written only if the sample
    count matches the vsync span within one field: the WAV then starts at the
    start gate and ends at the stop gate (the last logged loop top).  The
    DebugServer's `cycles` span is kept for reference only: it advanced about
    325,600 per field on 2026-10-09, so it is not a plain EE cycle count."""
    import wave
    start, stop = player.gates.get("start", {}), player.gates.get("stop", {})
    if not (start.get("go") and stop.get("go")):
        return
    found = [p for p in out.glob("*.wav") if p.name not in ("audio.wav", "audio_media.wav")]
    if not found:
        res = subprocess.run(["find", *[str(d) for d in MEDIA_DIRS if d.exists()], "-maxdepth", "1", "-type", "f",
                              "-iname", "*.wav", "-newer", str(marker)], capture_output=True, text=True)
        found = [Path(p) for p in res.stdout.split("\n") if p]
    if len(found) != 1:
        player.notes.append(f"audio: expected one capture WAV, found {[str(p) for p in found]}; the PS2 side stays silent")
        return
    dst = out / "audio_media.wav"
    if found[0].parent.resolve() == out.resolve():
        shutil.move(str(found[0]), dst)
    else:
        shutil.copyfile(found[0], dst)
        player.notes.append(f"audio: copied {found[0]}; the original stays where PCSX2 wrote it")
    with wave.open(str(dst)) as w:
        rate, ch, width, n = w.getframerate(), w.getnchannels(), w.getsampwidth(), w.getnframes()
    fields = stop["vsync"] - start["vsync"]
    by_vsync = fields * rate / emrec.TICK_HZ
    sync = {"source": str(found[0]), "rate": rate, "channels": ch, "bits": width * 8, "samples": n,
            "start_gate": start, "stop_gate": stop, "fields": fields, "expected_by_vsync": round(by_vsync, 1),
            "samples_minus_vsync": round(n - by_vsync, 1)}
    try:
        sync["debugserver_cycles_span"] = int(str(stop["cycles"]), 0) - int(str(start["cycles"]), 0)
    except (TypeError, ValueError, KeyError):
        pass
    sync["in_sync"] = bool(rate == 48000 and width == 2 and abs(n - by_vsync) <= rate / emrec.TICK_HZ)
    (out / "audio_sync.json").write_text(json.dumps(sync, indent=1) + "\n")
    if sync["in_sync"]:
        shutil.copyfile(dst, out / "audio.wav")
    player.notes.append(f"audio: media capture {n} samples at {rate} Hz vs {by_vsync:.0f} by vsync "
                        f"({'in sync: audio.wav written' if sync['in_sync'] else 'NOT in sync: no audio.wav'})")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("recording")
    ap.add_argument("--out", required=True)
    ap.add_argument("--stride", type=int, default=4, help="capture ticks whose segment offset %% stride == 0")
    ap.add_argument("--frames", choices=["gs", "screenshot"], default="gs")
    ap.add_argument("--audio", action="store_true", help="record PCSX2's SPU2 WAV log")
    ap.add_argument("--media-audio", action="store_true",
                    help="visible session; wait on audio_start/audio_stop marker files while PCSX2's Media "
                         "Capture (audio only, WAV) is toggled with the session's temporary End hotkey "
                         "(the menu reports its items disabled when driven from the background)")
    ap.add_argument("--emulator-lib-path",
                    help="a folder holding FFmpeg's libraries with the majors PCSX2 asks for, built for the "
                         "emulator's architecture (x86_64 for this PCSX2; Homebrew's arm64 "
                         "/opt/homebrew/opt/ffmpeg/lib is refused), passed to the emulator process only as "
                         "DYLD_FALLBACK_LIBRARY_PATH (user decision 2026-10-09: Homebrew FFmpeg for "
                         "--media-audio); nothing in the emulator install changes")
    ap.add_argument("--media-wait", type=float, default=1800.0, help="seconds to wait at each audio gate")
    ap.add_argument("--state", default=str(TITLE_STATE), help="title save state (never modified)")
    ap.add_argument("--tail", type=int, default=4)
    ap.add_argument("--title-limit", type=int, default=1500)
    ap.add_argument("--lock-poll", type=float, default=30.0)
    ap.add_argument("--resume-dir", action="store_true", help=argparse.SUPPRESS)
    return run(ap.parse_args(argv))


if __name__ == "__main__":
    sys.exit(main())
