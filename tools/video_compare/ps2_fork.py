"""ps2_fork.py - the PCSX2 pass of the video comparison on the project's
agent-debug PCSX2 fork (`ps2.py --emulator fork`).  docs/VIDEO_COMPARE.md,
"The fork path"; the fork itself: docs/PCSX2_FORK.md and
pcsx2-fork/EXTERMINATION.md (behaviour only; this tool uses the fork's
DebugServer commands and its MIT Python package `pcsx2dbg`, nothing else).

Same rules for the game as ps2.py (the same recording scheduler, pad block
model, phases and movie handling), different machinery:

  * Start: a COLD BOOT of the user's disc image (fixed RTC, so the boot is
    repeatable), the intro movie played, then the title driven to NEW GAME
    with ps2.py's title driver (Cross on NEW GAME while the fade is idle; the
    New Game movie skipped with START when the recording skipped it).  No
    title save state: the fork does not load v2.6.x states.  This is the
    compat stage's start (build/pcsx2-fork/acceptance/scripts/t2_fresh.py).
  * Ticks: `run {until: {ticks: 1}}` with the tick PC at the main-loop top
    0x001AAF28 and a persistent stop probe on the return from the input step
    (ps2.post_input_pc()), where the processed pad block is written with
    `mem_write`; the raw pad goes through v1 `pad_set` as in ps2.py.  A
    second stop probe at the movie driver's entry (func_00203350) is where a
    movie is skipped (START), so the skip lands on the same vsync in every
    run: the whole pass is repeatable.
  * Frames: ps2.py's capture point without save states.  At the main-loop
    top of tick t + 2 the buffer DISPFB2 names (as the EE wrote it) holds
    the field built from tick t; at the loop top of t + 3 that buffer is
    the drawing context's FRAME buffer, its late writes have landed and
    nothing has drawn into it yet (docs/CAPTURES_C7.md 5b), and the tool
    reads it from GS local memory (`mem_read` space gs, 56 pages, PSMCT32
    de-swizzled with ps2.word_map), software renderer.  --fork-field-k K
    reads the displayed field with `gs_field` at loop top t + K instead
    (diagnostics; --fork-extra-k adds such reads to frames_k<K>/).
  * Audio: the fork's emulated-time SPU2 tap (`audio_tap`, 48 kHz s16
    stereo exactly as the SPU2 mixed it).  It is started at the title; at
    every logged loop top the tool reads the tap's sample count (`audio_tap
    status`) and stores it, relative to the first recorded tick, in the
    row's `af` column (as the port does).  ps2/audio.wav is cut from the tap
    to exactly the logged span: sample 0 = the first recorded tick's loop
    top, the end = the last logged loop top.  compose.py places the PS2
    audio by `af` when a run has it.
  * Lock and safety: pcsx2dbg's Session takes build/.pcsx2.lock (mkdir,
    always rmdir), launches hidden on scratch data outside ~/Documents
    (BIOS and ini copies; the ISO APFS-cloned), never touches the user's
    portable-data, memory cards or slots, and shuts every instance down.

Everything written is derived from the user's own disc and stays under
build/video_compare/ (ignored).  Run with the decomp .venv python (numpy).
"""
from __future__ import annotations

import concurrent.futures as cf
import json
import os
import shutil
import struct
import sys
import time
import wave
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import emrec  # noqa: E402

FORK = Path(os.environ.get("PCSX2_FORK_DIR", ROOT.parent / "pcsx2-fork"))
ISO = ROOT / "Extermination-rebuilt.iso"
LOCK = ROOT / "build/.pcsx2.lock"
LOOP_TOP = 0x001AAF28
DISPFB2 = 0x12000090            # GS privileged register as the EE wrote it (fork region gs_priv)
FRAME_IDX = 0x00810E80          # the game's frame index (docs/CAPTURES_C7.md 5b)
FIELD_ADDR = 0x00810E88         # the field the vsync handler sampled (docs/PCSX2_FORK_GS_DIFF.md 5)
FIELD_K = 0                     # 0 = "frame" mode (below); K > 0 = gs_field at loop top t + K
RTC = "2026-01-01 00:00:00"     # the fork launcher's default fixed RTC (repeatable cold boots)
FADE = 0x0028A9A0
MOVIE_DRIVER = 0x00203350       # func_00203350, the blocking movie driver the main loop calls

# (address, length) reads per loop-top sample; all taken while the VM is paused
READS = {"scratch": (0x70003B64, 0x30), "area": (emrec.ADDR["area"], 4), "bd8": (emrec.ADDR["bd8"], 4),
         "task0": (emrec.ADDR["task0"], 0x20), "pos": (emrec.ADDR["pos"], 0x28),
         "movie": (emrec.ADDR["movie"], 4), "pad": (emrec.ADDR["pad_struct"], 0x54), "fade": (FADE, 4)}


def log(*a) -> None:
    print("ps2-fork:", *a, flush=True)


def scratch_base() -> Path:
    base = Path(os.environ.get("PCSX2_FORK_SCRATCH") or Path(os.environ.get("TMPDIR", "/tmp")) / "pcsx2-fork-vc")
    if str(base.resolve()).startswith(str((Path.home() / "Documents").resolve())):
        raise SystemExit(f"fork scratch must be outside ~/Documents: {base}")
    return base


class ForkPlayer:
    def __init__(self, rec: emrec.Recording, out: Path, stride: int, audio: bool, field_k: int = FIELD_K,
                 extra_k: tuple = (), max_ticks: int | None = None, tail: int = 4, title_limit: int = 1500,
                 mtvu: bool | None = None, renderer: int = 13, app: str | None = None, present: bool = True,
                 title_delay: int = 0):
        import ps2
        self.ps2 = ps2
        self.rec, self.out, self.stride, self.audio = rec, out, stride, audio
        self.field_k, self.extra_k = field_k, tuple(k for k in extra_k if k != field_k)
        self.armed: dict[int, dict] = {}              # frame mode: cap -> DISPFB2 read at loop top t + 2
        self.max_ticks, self.tail, self.title_limit = max_ticks, tail, title_limit
        self.mtvu, self.renderer, self.app, self.present = mtvu, renderer, app, present
        self.title_delay = title_delay      # hold the title driver back: moves the New Game commit
        self.rows: list[dict] = []
        self.extra: list[dict] = []
        self.notes: list[str] = []
        self.pending: dict[int, list] = {}            # loop-top counter -> [(k, cap, row)]
        self.captures = 0
        self.pool = cf.ThreadPoolExecutor(max_workers=3)
        self.futures: list = []
        self.raw_pad = None
        self.post_pc = ps2.post_input_pc()
        segs = rec.segments()
        self.first = emrec.first_play_segment(rec)
        title_ticks = sum(len(rows) for _ph, rows in segs[:self.first])
        movies = rec.movies()
        before = [m for m in movies if m[0] <= title_ticks]
        self.title_movie_skip = before[-1][1] if before else True
        self.later_movies = [m[1] for m in movies if m[0] > title_ticks]
        self.movie_count = 0
        self.info: dict = {}
        self.movies: list[dict] = []

    # -- low level ---------------------------------------------------------
    def rd(self, addr: int, n: int) -> bytes:
        return self.c.read(addr, n)

    def sample(self) -> dict:
        raw = {k: self.rd(a, n) for k, (a, n) in READS.items()}
        sc, t0, pad = raw["scratch"], raw["task0"], raw["pad"]
        x, y, z, _w = struct.unpack_from("<4f", raw["pos"], 0)
        return {"counter": struct.unpack_from("<I", sc, 0)[0], "s8d": sc[0x29], "area": raw["area"][0],
                "sub": raw["area"][1], "entry": raw["area"][2], "bd8": raw["bd8"][0], "t9": t0[9],
                "tb": t0[0xB], "x": x, "y": y, "z": z, "yaw": struct.unpack_from("<f", raw["pos"], 0x24)[0],
                "movie": raw["movie"][0], "vsync": struct.unpack_from("<I", pad, 0x50)[0], "fade": raw["fade"],
                "pad": pad[:0x40]}

    def set_raw(self, pad) -> None:
        if pad != self.raw_pad:
            btn, lx, ly, rx, ry = pad
            self.c.call("pad_set", buttons=btn, lx=lx, ly=ly, rx=rx, ry=ry)
            self.raw_pad = pad

    def run_to(self, until: dict, skip: bool | None = None, timeout: float = 1800.0):
        """Run until `until`.  A movie is caught by a stop probe at the entry of the
        blocking movie driver func_00203350 (docs/STARTUP.md): if `skip`, START is set
        as the raw pad right there and the run goes on.  The probe stop is an emulated
        point, so the skip lands on the same vsync in every run (ps2.py's host-polled
        hook does not: on 2026-10-09 two fork runs committed New Game 3 vsyncs apart).
        -> (stop, movie_seen)"""
        seen = False
        while True:
            stop = self.c.call("run", until=until, timeout_s=timeout, timeout=timeout + 60)["stop"]
            if stop.get("reason") == "probe" and int(stop["pc"], 16) == MOVIE_DRIVER:
                seen = True
                self.movie_count += 1
                self.movies.append({"vsync": stop["vsync"], "tick": stop["tick"], "skip": bool(skip)})
                log(f"movie at vsync {stop['vsync']}: {'skipping with START' if skip else 'playing it'}")
                if skip:
                    self.set_raw((emrec.START, 0x80, 0x80, 0x80, 0x80))
                continue
            return stop, seen

    def tick(self, skip: bool | None = None, timeout: float = 1800.0):
        """To the next loop top, passing through post-input probe stops (title, boot)."""
        seen_any = False
        while True:
            stop, seen = self.run_to({"ticks": 1}, skip, timeout)
            seen_any |= seen
            if stop["reason"] == "tick":
                return stop, seen_any
            if stop["reason"] != "probe":
                raise RuntimeError(f"unexpected stop {stop}")

    def audio_samples(self) -> int | None:
        if not self.audio:
            return None
        return int(self.c.call("audio_tap", action="status")["samples"])

    # -- frames ------------------------------------------------------------
    def capture(self, k: int, cap: int, tick_row: int, base: dict | None = None) -> None:
        reply, blob = self.c.call_blob("gs_field", format="blob")
        regs = reply.get("regs", {})
        info = {**(base or {}), "k": k, "source": "gs_field", "circuit": reply.get("circuit"), "width": reply["width"],
                "height": reply["height"], "psm": reply.get("psm"), "dispfb2": regs.get("DISPFB2"),
                "display2": regs.get("DISPLAY2"), "rgba_xxh3": reply.get("rgba_xxh3"),
                "renderer": reply.get("renderer")}
        folder = self.out / ("frames" if k == self.field_k else f"frames_k{k}")
        png = folder / f"{cap:06d}.png"
        self.futures.append((k, cap, tick_row, info, self.pool.submit(self._write, blob, info, png)))

    def arm(self, cap: int) -> None:
        """Frame mode, loop top t + 2: the buffer DISPFB2 names now holds tick t's field."""
        v = int.from_bytes(self.rd(DISPFB2, 8), "little")
        self.armed[cap] = {"dispfb2": v, "fbp": v & 0x1FF, "fbw": ((v >> 9) & 0x3F) * 64}

    def capture_buffer(self, cap: int, tick_row: int) -> None:
        """Frame mode, loop top t + 3: read that buffer from GS local memory.  It is the
        drawing context's FRAME buffer now (ps2.py's capture point: its late writes have
        landed and the iteration starting here has not drawn yet; docs/CAPTURES_C7.md 5b)."""
        a = self.armed.pop(cap)
        now = int.from_bytes(self.rd(DISPFB2, 8), "little")
        w, h = 512, 224
        info = {"source": "gs_buffer", "fbp": a["fbp"], "fbw": a["fbw"], "dispfb2_t2": hex(a["dispfb2"]),
                "dispfb2_t3": hex(now), "width": w, "height": h}
        if a["fbw"] != 512 or a["fbp"] + 56 > 512 or (now & 0x1FF) == a["fbp"]:
            # an unexpected display set-up: fall back to the displayed field (gs_field) and say so
            info["fallback"] = "gs_field"
            return self.capture(0, cap, tick_row, info)
        _r, blob = self.c.call_blob("mem_read", space="gs", addr=a["fbp"] * 8192, len=56 * 8192, **{"as": "blob"})
        png = self.out / "frames" / f"{cap:06d}.png"
        self.futures.append((0, cap, tick_row, info, self.pool.submit(self._write_buffer, blob, info, png)))

    @staticmethod
    def _write_buffer(blob: bytes, info: dict, png: Path) -> None:
        import numpy as np
        import ps2
        words = np.frombuffer(blob, dtype="<u4")
        px = words[ps2.word_map(info["width"], info["height"], info["fbw"])]
        rgb = px.astype("<u4").view(np.uint8).reshape(info["height"], info["width"], 4)[..., :3]
        emrec.write_png(png, np.ascontiguousarray(rgb))

    @staticmethod
    def _write(blob: bytes, info: dict, png: Path) -> None:
        import numpy as np
        w, h = info["width"], info["height"]
        rgba = np.frombuffer(blob, dtype=np.uint8).reshape(h, w, 4)
        emrec.write_png(png, np.ascontiguousarray(rgba[..., :3]))

    # -- the session -------------------------------------------------------
    def run(self) -> None:
        sys.path.insert(0, str(FORK / "extermination/python"))
        from pcsx2dbg.launcher import LaunchConfig, Session, sources_fingerprint
        for k in (self.field_k, *self.extra_k):
            (self.out / ("frames" if k == self.field_k else f"frames_k{k}")).mkdir(parents=True, exist_ok=True)
        kw = {}
        if self.app:
            kw["app"] = Path(self.app)
        # the disc boots its own SCUS_971.12, the original boot path (an -elf override also
        # boots since fork commit 1e22fdc0c, which passes the scratch copy's real path)
        # present=True: frames are copied to the (hidden) window; presentation only paces the
        # run (120 Hz cap), not emulation.  It was needed before fork 0.2.2: with presentation
        # off those builds never submitted GPU work and aborted after ~27,000 vsyncs (twice on
        # 2026-10-09: a long free run, and ~10 s into the level-exit movie E001).  0.2.2 fixed
        # it (fork CHANGELOG), so --fork-no-present is safe there.
        cfg = LaunchConfig(iso=ISO, rtc=RTC, unlimited=True, renderer=self.renderer, lease_s=1800,
                           mtvu=self.mtvu, present=self.present, **kw)
        fp = sources_fingerprint(cfg)
        base = scratch_base()
        log(f"waiting for / taking the run lock {LOCK}")
        t_wait = time.monotonic()
        while True:
            try:
                sess = Session(scratch_base=base, lock=LOCK)
                sess.open()
                break
            except Exception as exc:                  # the lock is held by another lane: poll
                if "run lock held" not in str(exc):
                    raise
                time.sleep(3)          # other lanes run short sessions back to back: poll often
        self.info["lock_wait_s"] = round(time.monotonic() - t_wait, 1)
        t0 = time.monotonic()
        try:
            inst = sess.launch("vc", cfg)
            self.c = inst.client()
            self.c.wait_vm(120)
            hello = self.c.call("hello")
            self.info["emulator"] = hello.get("emulator")
            self.info["server_version"] = hello.get("server_version")
            self.c.call("set_tick_pc", pc=LOOP_TOP)
            self.c.call("probe_add", pc=self.post_pc, action="stop", label="video_compare pad block")
            self.c.call("probe_add", pc=MOVIE_DRIVER, action="stop", label="video_compare movie")
            self._drive(inst)
            if self.audio:
                self.info["audio_stop"] = self.c.call("audio_tap", action="stop")
                self._collect_audio(inst)
            self.info["emulator_log_tail"] = inst.log.read_text(errors="replace").splitlines()[-10:]
        finally:
            exits = sess.close()
            self.info["shutdown"] = exits
            self.info["session_s"] = round(time.monotonic() - t0, 1)
            for k, cap, row, info, fut in self.futures:
                try:
                    fut.result()
                    if k == self.field_k:
                        self.rows[row]["cap"] = cap
                        self.extra[row]["frame"] = info
                    else:
                        self.extra[row].setdefault("frames_k", {})[str(k)] = info
                except Exception as exc:
                    self.notes.append(f"capture {cap} (k {k}) failed: {exc}")
            self.pool.shutdown()
            logs = self.out / "pcsx2_logs"
            logs.mkdir(exist_ok=True)
            for f in sess.dir.glob("*/emulator.log"):
                shutil.copyfile(f, logs / "emulator.log")
            if sess.dir.exists() and base in sess.dir.parents:
                shutil.rmtree(sess.dir, ignore_errors=True)       # BIOS copies, ini, tap files
        self.info["sources_unchanged"] = fp == sources_fingerprint(cfg)

    def _drive(self, inst) -> None:
        c = self.c
        self.set_raw(emrec.NEUTRAL)
        # Cold boot: tick until an iteration takes a movie (the intro, played),
        # then until the title is idle (area 0, fade clear).
        t_boot = time.monotonic()
        intro_done, i = False, 0
        while True:
            _stop, seen = self.tick(skip=False, timeout=300.0)      # the intro movie: about 60 s
            row = self.sample()
            intro_done |= seen
            if intro_done and row["area"] == 0 and row["fade"] == b"\0\0\0\0":
                break
            i += 1
            if i > 20000:
                raise TimeoutError("the cold boot did not reach the title")
        self.info["boot"] = {"title_counter": row["counter"], "title_vsync": row["vsync"],
                             "seconds": round(time.monotonic() - t_boot, 1)}
        log(f"title reached: counter {row['counter']} after {self.info['boot']['seconds']} s")
        if self.audio:
            self.info["audio_start"] = c.call("audio_tap", action="start", path="audio_tap.wav")
        # Title: ps2.py's title driver.
        n, presses = 0, 0
        self.movie_count = 0
        while True:
            row = self.sample()
            if emrec.phase_of(row["area"], row["bd8"], row["t9"], row["s8d"]) != "T":
                break
            m = n - self.title_delay
            if m in (5, 125, 245, 365) and presses < 4 and row["fade"] == b"\0\0\0\0":
                self.set_raw((0x4000, 0x80, 0x80, 0x80, 0x80))
                presses += 1
            elif m >= 0 and m % 120 == 9:
                self.set_raw(emrec.NEUTRAL)
            _stop, seen = self.tick(skip=self.title_movie_skip)
            if seen:
                self.set_raw(emrec.NEUTRAL)
            n += 1
            if n > self.title_limit:
                raise TimeoutError("the title did not start a New Game")
        self.info["title"] = {"ticks": n, "presses": presses, "commit_counter": row["counter"],
                              "commit_vsync": row["vsync"], "title_delay": self.title_delay,
                              "commit_frame_index": self.rd(FRAME_IDX, 4)[0],
                              "commit_field": self.rd(FIELD_ADDR, 4)[0]}
        log(f"New Game committed after {n} title ticks ({presses} Cross press(es)); counter {row['counter']}")
        sched = emrec.Scheduler(self.rec, first=self.first)
        model = emrec.PadBlock.from_memory(row["pad"])
        self.movie_count = 0
        pad = emrec.NEUTRAL
        tail, t = 0, 0
        s0 = None
        t_play = time.monotonic()
        ks = ((self.field_k,) if self.field_k else ()) + self.extra_k
        while True:
            row = self.sample()
            st = c.call("state")
            samples = self.audio_samples()
            if s0 is None:
                s0 = samples
            for k, cap, tick_row in self.pending.pop(row["counter"], []):
                if k == "arm":
                    self.arm(cap)
                elif k == "buf":
                    self.capture_buffer(cap, tick_row)
                else:
                    self.capture(k, cap, tick_row)
            phase = emrec.phase_of(row["area"], row["bd8"], row["t9"], row["s8d"])
            self.end = {"counter": row["counter"], "vsync": row["vsync"], "agent_vsync": st["vsync"],
                        "iop_cycle": st["iop_cycle"], "samples": samples}
            if sched.done or (self.max_ticks is not None and t >= self.max_ticks):
                if not self.pending or tail >= self.tail:
                    break
                tail += 1
                seg, off = -1, -1
            else:
                pad, seg, off = sched.next(phase)
            model.unpack(pad)
            self.set_raw(pad)
            r = {"step": t, "counter": row["counter"], "mv": 0, "ph": phase, "btn": pad[0], "lx": pad[1],
                 "ly": pad[2], "rx": pad[3], "ry": pad[4], "area": row["area"], "sub": row["sub"],
                 "entry": row["entry"], "bd8": row["bd8"], "s8d": row["s8d"], "t9": row["t9"],
                 "tb": row["tb"], "x": row["x"], "y": row["y"], "z": row["z"], "yaw": row["yaw"],
                 "seg": seg, "off": off, "cap": -1, "af": (samples - s0) if samples is not None else -1}
            self.rows.append(r)
            ex = {"vsync": row["vsync"], "agent_vsync": st["vsync"], "ee_cycle": st["ee_cycle"],
                  "iop_cycle": st["iop_cycle"], "tap_samples": samples,
                  "frame_idx": self.rd(FRAME_IDX, 4)[0], "field": self.rd(FIELD_ADDR, 4)[0]}
            self.extra.append(ex)
            if seg >= 0 and off % self.stride == 0:
                here = len(self.rows) - 1
                for k in ks:
                    self.pending.setdefault(row["counter"] + k, []).append((k, self.captures, here))
                if not self.field_k:
                    self.pending.setdefault(row["counter"] + 2, []).append(("arm", self.captures, here))
                    self.pending.setdefault(row["counter"] + 3, []).append(("buf", self.captures, here))
                self.captures += 1
            # step C runs; then the recorded pad block replaces the original's own unpack
            stop, _seen = self.run_to({"ticks": 1}, timeout=120)
            if stop["reason"] != "probe" or int(stop["pc"], 16) != self.post_pc:
                raise RuntimeError(f"expected the post-input stop, got {stop}")
            own = self.rd(0x00810E40, 0x40)
            ex["own_block"] = own[0x30:0x3C].hex()
            ex["own_sticks_gait"] = own[0x24:0x28].hex() + f"{own[0x17]:02x}"
            ex["model_block"] = model.block_bytes().hex()
            ex["model_sticks_gait"] = bytes([model.lx, model.ly, model.rx, model.ry, model.gait]).hex()
            c.call("mem_write", addr=0x00810E57, hex=bytes([model.gait]).hex())
            c.call("mem_write", addr=0x00810E64, hex=bytes([model.lx, model.ly, model.rx, model.ry]).hex())
            c.call("mem_write", addr=0x00810E70, hex=model.block_bytes().hex())
            later_skip = (self.later_movies[self.movie_count] if self.movie_count < len(self.later_movies)
                          else True)
            before = self.movie_count
            stop, seen = self.run_to({"ticks": 1}, skip=later_skip)
            if stop["reason"] != "tick":
                raise RuntimeError(f"expected the loop top, got {stop}")
            if seen:          # the port's movie steps unpack the movie pad at least twice
                for _ in range(2):
                    model.unpack((emrec.START if later_skip else 0, 0x80, 0x80, 0x80, 0x80))
                ex["movie"] = True
                ex["movie_skipped"] = bool(later_skip)
                assert self.movie_count == before + 1
            t += 1
            if t % 1000 == 0:
                log(f"tick {t}: counter {row['counter']} phase {phase} segment {seg} offset {off}, "
                    f"{self.captures} captures, {t / (time.monotonic() - t_play):.0f} ticks/s")
            if sched.done and tail == 0 and "used_up" not in self.info:
                self.info["used_up"] = t
                log(f"recording used up after {t} ticks (counter {row['counter']})")
        self.info["movies"] = self.movies
        self.info["play"] = {"ticks": t, "seconds": round(time.monotonic() - t_play, 1),
                             "first_samples": s0, "end": self.end}
        self.notes += sched.notes

    # -- audio -------------------------------------------------------------
    def _collect_audio(self, inst) -> None:
        """Cut the tap to the logged span: [first loop top, last loop top)."""
        tap = inst.root / "audio_tap.wav"
        s0, s1 = self.info["play"]["first_samples"], self.end["samples"]
        with wave.open(str(tap)) as w:
            rate, ch, width, n = w.getframerate(), w.getnchannels(), w.getsampwidth(), w.getnframes()
            w.setpos(s0)
            data = w.readframes(s1 - s0)
        if (rate, ch, width) != (emrec.AUDIO_RATE, 2, 2):
            raise RuntimeError(f"unexpected tap format {rate} Hz, {ch} ch, {width * 8} bit")
        with wave.open(str(self.out / "audio.wav"), "wb") as w:
            w.setnchannels(2)
            w.setsampwidth(2)
            w.setframerate(rate)
            w.writeframes(data)
        marks = (Path(str(tap) + ".marks")).read_bytes()
        c0, = struct.unpack_from("<Q", marks, 16)
        # each logged loop top: its tap count vs the SPU2 clock (one sample per 768 IOP cycles from c0)
        dev = [ex["tap_samples"] - (ex["iop_cycle"] - c0) / 768 for ex in self.extra if ex.get("tap_samples") is not None]
        span_fields = self.end["agent_vsync"] - self.extra[0]["agent_vsync"]
        sync = {"tap_samples_total": n, "rate": rate, "first_loop_top_sample": s0, "last_loop_top_sample": s1,
                "audio_wav_samples": s1 - s0, "audio_wav_seconds": round((s1 - s0) / rate, 3),
                "rows": len(self.rows), "agent_vsync_span": span_fields,
                "expected_by_vsync": round(span_fields * rate / emrec.TICK_HZ, 1),
                "samples_minus_vsync_expectation": round(s1 - s0 - span_fields * rate / emrec.TICK_HZ, 1),
                "iop_cycle_sample0": c0,
                "loop_top_count_minus_spu2_clock": {"min": round(min(dev), 3), "max": round(max(dev), 3)},
                "audio_start": self.info.get("audio_start"), "audio_stop": self.info.get("audio_stop")}
        (self.out / "audio_sync.json").write_text(json.dumps(sync, indent=1, default=str) + "\n")
        self.notes.append(f"audio: fork SPU2 tap, {s1 - s0} samples ({(s1 - s0) / rate:.1f} s) over the logged "
                          f"span; row af = sample index at the row's loop top")


def run(args, rec: emrec.Recording, out: Path) -> int:
    t0 = time.monotonic()
    extra_k = tuple(int(k) for k in args.fork_extra_k.split(",") if k) if args.fork_extra_k else ()
    player = ForkPlayer(rec, out, args.stride, audio=args.audio, field_k=args.fork_field_k, extra_k=extra_k,
                        max_ticks=args.max_ticks, tail=args.tail, title_limit=args.title_limit,
                        mtvu=args.fork_mtvu, renderer=args.fork_renderer, app=args.fork_app,
                        present=not args.fork_no_present, title_delay=args.fork_title_delay)
    import signal

    def on_term(signum, frame):          # a killed driver must still close its session (lock, emulator)
        raise KeyboardInterrupt(f"signal {signum}")
    old = signal.signal(signal.SIGTERM, on_term)
    try:
        player.run()
    finally:
        signal.signal(signal.SIGTERM, old)
        log(f"no emulator process left: {not __import__('ps2').emulator_running()}")
    emrec.write(out / "ps2.rec", {"source": "pcsx2-fork", "mode": "playback-log", "state": "cold boot",
                                  "stride": args.stride, "frames": "gs_field", "field_k": args.fork_field_k,
                                  "audio": "fork-tap" if args.audio else "none"}, player.rows)
    (out / "ps2_extra.json").write_text(json.dumps({
        "emulator": "fork", "notes": player.notes, "seconds_total": round(time.monotonic() - t0, 1),
        "captures": player.captures, "info": player.info,
        "first_vsync": player.extra[0]["vsync"] if player.extra else None,
        "end_vsync": player.end["vsync"] if hasattr(player, "end") else None,
        "rows": player.extra}, indent=0, default=str) + "\n")
    log(f"{len(player.rows)} ticks, {player.captures} frames, notes: {player.notes}")
    return 0
