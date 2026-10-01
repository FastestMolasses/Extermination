"""emrec.py - the input recording format ("EMREC 1") and the sync model shared
by the native port (src/game/em_replay.c in ../extermination-port) and the
PCSX2 driver (ps2.py).  docs/VIDEO_COMPARE.md describes both.

A recording is a text file: '#' header lines, then one row per main-loop step
with the columns in COLS.  A step whose counter does not advance is a movie
step (mv = 1); every other row is one game tick (the original's main-loop
iteration, main-loop counter 0x70003B64).

Sync phase of a tick (from the state at the start of the tick, i.e. the
original's main-loop top 0x001AAF28):
    T  title / front end   D_00810700 (area) == 0
    L  load                D_00275BD8 != 0, or the slot-0 task record's +9 == 5
    C  cutscene / menu     scratchpad 0x70003B8D != 0
    P  play                otherwise
A segment is a run of ticks with one phase.  Playback applies a recorded
segment's pads tick for tick from the run's own first tick of that phase.

Nothing here reads or embeds disc data; it only names addresses.
"""
from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass, field
from pathlib import Path

MAGIC = "EMREC 1"
COLS = ("step counter mv ph btn lx ly rx ry area sub entry bd8 s8d t9 tb x y z yaw "
        "seg off cap af").split()
INT_COLS = {c for c in COLS if c not in ("ph", "x", "y", "z", "yaw")}
NEUTRAL = (0, 0x80, 0x80, 0x80, 0x80)          # buttons, lx, ly, rx, ry
START = 0x0008                                   # canonical DualShock bit (EM_PAD_START)
TICK_HZ = 60000 / 1001                           # NTSC field rate the logic ticks at
AUDIO_RATE = 48000
FRAMES_PER_TICK = AUDIO_RATE / TICK_HZ           # 800.8

# Original addresses the PCSX2 side samples (the same bytes the port names).
ADDR = {
    "counter": 0x70003B64, "area": 0x00810700, "bd8": 0x00275BD8, "spad": 0x70003B8C,
    "task0": 0x0028A750, "pos": 0x00810350, "yaw": 0x00810374, "movie": 0x00821058,
    "vsync": 0x00810E90, "pad_struct": 0x00810E40,
}


def phase_of(area: int, bd8: int, t9: int, s8d: int) -> str:
    if area == 0:
        return "T"
    if bd8 or t9 == 5:
        return "L"
    if s8d:
        return "C"
    return "P"


# ---------------------------------------------------------------- the file

def _parse_value(col: str, text: str):
    if col == "ph":
        return text
    if col in ("x", "y", "z", "yaw"):
        return float(text)
    return int(text, 0)


@dataclass
class Recording:
    header: dict = field(default_factory=dict)
    rows: list = field(default_factory=list)          # dicts keyed by COLS

    # ---- derived views --------------------------------------------------
    def ticks(self) -> list:
        return [r for r in self.rows if not r["mv"]]

    def segments(self) -> list:
        """[(phase, [tick rows])] in order (movie steps excluded)."""
        segs: list = []
        for r in self.ticks():
            if not segs or segs[-1][0] != r["ph"]:
                segs.append((r["ph"], []))
            segs[-1][1].append(r)
        return segs

    def movies(self) -> list:
        """[(ticks_before, skipped)] per movie, in order: ticks_before is the
        number of game ticks that precede it; skipped = START held at any of
        its steps (playback then holds START to skip it)."""
        out, ticks, open_ = [], 0, False
        for r in self.rows:
            if r["mv"]:
                if not open_:
                    out.append([ticks, False])
                    open_ = True
                if r["btn"] & START:
                    out[-1][1] = True
            else:
                open_ = False
                ticks += 1
        return [tuple(m) for m in out]


def read(path: Path | str) -> Recording:
    rec = Recording()
    magic = cols = False
    with open(path) as f:
        for line in f:
            if line.startswith("#"):
                body = line[1:].strip()
                if body == MAGIC:
                    magic = True
                elif body.startswith("cols "):
                    cols = body[5:].split() == COLS
                else:
                    k, _, v = body.partition(" ")
                    if k == "env":                      # repeated: launch switches to re-apply
                        rec.header.setdefault("env", []).append(v)
                    else:
                        rec.header[k] = v
                continue
            parts = line.split()
            if not parts:
                continue
            if len(parts) != len(COLS):
                raise ValueError(f"{path}: row with {len(parts)} columns: {line!r}")
            rec.rows.append({c: _parse_value(c, p) for c, p in zip(COLS, parts)})
    if not magic or not cols:
        raise ValueError(f"{path}: not an {MAGIC} file with the expected columns")
    return rec


def fmt_float(v: float) -> str:
    return f"{v:.9g}"


def write(path: Path | str, header: dict, rows: list) -> None:
    with open(path, "w") as f:
        f.write(f"# {MAGIC}\n")
        for k, v in header.items():
            f.write(f"# {k} {v}\n")
        f.write("#cols " + " ".join(COLS) + "\n")
        for r in rows:
            out = []
            for c in COLS:
                v = r.get(c, -1 if c in ("seg", "off", "cap", "af") else 0)
                if c == "btn":
                    out.append(f"0x{v:04x}")
                elif c in ("x", "y", "z", "yaw"):
                    out.append(fmt_float(v))
                else:
                    out.append(str(v))
            f.write(" ".join(out) + "\n")


# ---------------------------------------------------------- the scheduler

SHORT_SEG = 8


class Scheduler:
    """Segment playback, the same rules as em_replay.c's filter():
    at the run's first tick take the first recorded segment of its phase
    (from `first`; the PCSX2 side starts after the title).  At a live phase
    change take the next recorded segment if its phase matches, or one up to
    4 ahead when everything in between is short (<= SHORT_SEG ticks);
    otherwise the change is a flicker of the live run and the current
    segment's timeline continues (also when the phase returns to it).
    Inside a segment: the recorded pad at the same offset; past its end, its
    last pad."""

    def __init__(self, rec: Recording, first: int = 0, max_overrun: int = 1800):
        self.segs = rec.segments()
        self.cur = None
        self.off = 0
        self.prev = None
        self.first = first
        self.max_overrun = max_overrun
        self.done = False
        self.notes: list[str] = []

    def _next(self, phase: str):
        if self.cur is None:
            return next((k for k in range(self.first, min(self.first + 4, len(self.segs)))
                         if self.segs[k][0] == phase), None)
        for k in range(self.cur + 1, min(self.cur + 5, len(self.segs))):
            if self.segs[k][0] == phase:
                return k
            if len(self.segs[k][1]) > SHORT_SEG:
                break
        return None

    def next(self, phase: str):
        """-> (pad tuple, segment index, offset); sets self.done on the last
        recorded tick or on a desync."""
        if self.prev is None or phase != self.prev:
            j = None if (self.prev is not None and phase == self.segs[self.cur][0]) else self._next(phase)
            if self.cur is None and j is None:
                self.notes.append(f"desync: the run starts in phase {phase}, the recording does not")
                self.done = True
                j = self.first
            if j is not None:
                if self.cur is not None and j > self.cur + 1:
                    self.notes.append(f"skipped short recorded segment(s) {self.cur + 1}..{j - 1}")
                self.cur, self.off = j, 0
            else:
                self.off += 1
        else:
            self.off += 1
        self.prev = phase
        ph, rows = self.segs[self.cur]
        r = rows[min(self.off, len(rows) - 1)]
        if self.off - len(rows) > self.max_overrun:
            self.notes.append(f"desync: segment {self.cur} overran the recording by {self.max_overrun}")
            self.done = True
        if self.cur + 1 == len(self.segs) and self.off + 1 >= len(rows):
            self.done = True
        return (r["btn"], r["lx"], r["ly"], r["rx"], r["ry"]), self.cur, self.off


def first_play_segment(rec: Recording) -> int:
    segs = rec.segments()
    return next(i for i, (ph, _) in enumerate(segs) if ph != "T")


# ----------------------------------------------- the pad block (001B5940)

class PadBlock:
    """The processed pad block the original's step C leaves (pad struct
    0x00810E40: +0x17 gait, +0x24..+0x27 sticks; 0x00810E70..7B: held,
    prev held, pressed, prev pressed, repeat, repeat timer), computed with
    the port's em_pad_unpack rules (em_input.c) from the raw DualShock bytes
    em_pad_raw builds: analog pad on port 0.  The PCSX2 driver writes it
    after the original's own unpack each tick, so the original's logic sees
    the same pad on the same tick as the port's (no libpad latency)."""

    def __init__(self, held=0, prev_held=0, pressed=0, prev_pressed=0, repeat=0, repeat_timer=0,
                 sticks=(0x80, 0x80, 0x80, 0x80), gait=0):
        self.held, self.prev_held, self.pressed, self.prev_pressed = held, prev_held, pressed, prev_pressed
        self.repeat, self.repeat_timer = repeat, repeat_timer
        self.lx, self.ly, self.rx, self.ry = sticks
        self.gait = gait

    @classmethod
    def from_memory(cls, struct_bytes: bytes) -> "PadBlock":
        """`struct_bytes` = 0x40 bytes at 0x00810E40 (pad struct + the block)."""
        h = struct.unpack_from("<6H", struct_bytes, 0x30)
        return cls(h[0], h[1], h[2], h[3], h[4], struct.unpack("<h", struct.pack("<H", h[5]))[0],
                   tuple(struct_bytes[0x24:0x28]), struct_bytes[0x17])

    @staticmethod
    def _gait(x: int, y: int) -> int:
        dx, dy = x - 0x80, y - 0x80
        r2 = dx * dx + dy * dy
        return 0 if r2 <= 48 * 48 else 1 if r2 <= 88 * 88 else 2 if r2 <= 122 * 122 else 3

    @staticmethod
    def _quantize(v: int) -> int:
        t = v + 2
        return (t & 0xFC) if t < 0x100 else 0xFC

    @staticmethod
    def _stick_dpad(v: int, axis: int) -> int:
        if v < 0x10:
            return 0x1000 if axis else 0x8000
        if v >= 0xE1:
            return 0x4000 if axis else 0x2000
        return 0

    def _dpad_stick(self) -> None:
        b, x, y = self.held, 0x80, 0x80
        self.gait = 3
        if b & 0x1000:
            x, y = (0xF0, 0x10) if b & 0x2000 else (0x10, 0x10) if b & 0x8000 else (0x80, 0x00)
        elif b & 0x4000:
            x, y = (0xF0, 0xF0) if b & 0x2000 else (0x10, 0xF0) if b & 0x8000 else (0x80, 0xFF)
        elif b & 0x2000:
            x = 0xFF
        elif b & 0x8000:
            x = 0x00
        else:
            self.gait = 0
        self.lx, self.ly = x, y

    def unpack(self, pad) -> None:
        """One step-C unpack of pad = (canonical buttons, lx, ly, rx, ry)."""
        btn, lx, ly, rx, ry = pad
        self.prev_held = self.held
        swapped = ((btn << 8) | (btn >> 8)) & 0xFFFF          # canonical -> original layout
        self.held = swapped
        self.rx, self.ry, self.lx, self.ly = rx, ry, lx, ly
        self.gait = self._gait(lx, ly)
        if self.gait == 0:
            if self.held & 0xF000:
                self._dpad_stick()
        else:
            self.held &= 0x0FFF
            self.held |= self._stick_dpad(self.lx, 0) | self._stick_dpad(self.ly, 1)
            self.lx, self.ly = self._quantize(self.lx), self._quantize(self.ly)
        self.prev_pressed = self.pressed
        self.pressed = self.held & ~self.prev_held & 0xFFFF
        dpad, prev_dpad = self.held & 0xF000, self.prev_held & 0xF000
        if dpad != prev_dpad or dpad == 0:
            self.repeat_timer = 0x20
            self.repeat = self.pressed
        else:
            self.repeat_timer -= 1
            if self.repeat_timer == 0:
                self.repeat = dpad | (self.pressed & 0x0FFF)
                self.repeat_timer = 10
            else:
                self.repeat = self.pressed & 0x0FFF

    def block_bytes(self) -> bytes:
        """0x810E70..0x810E7B."""
        return struct.pack("<5Hh", self.held, self.prev_held, self.pressed, self.prev_pressed,
                           self.repeat, self.repeat_timer)


# ------------------------------------------------------------- frames

def write_png(path: Path | str, rgb, level: int = 1) -> None:
    """Minimal 8-bit RGB PNG writer (numpy (h, w, 3) uint8), no PIL needed."""
    h, w, _ = rgb.shape
    raw = b"".join(b"\x00" + rgb[y].tobytes() for y in range(h))

    def chunk(kind: bytes, data: bytes) -> bytes:
        c = kind + data
        return struct.pack(">I", len(data)) + c + struct.pack(">I", zlib.crc32(c) & 0xFFFFFFFF)
    Path(path).write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                           + chunk(b"IDAT", zlib.compress(raw, level)) + chunk(b"IEND", b""))
