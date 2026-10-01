"""native.py - play an EMREC recording on the native port (headless) and keep
its frames and audio for the video comparison.  docs/VIDEO_COMPARE.md.

The port (../extermination-port, src/game/em_replay.c) replays the recording
with EM_INPUT_PLAY, writes its playback log with EM_INPUT_RECORD, requests a
frame capture at every captured tick (EM_REPLAY_CAPTURE, a FIFO here: the
24-bit BMP of the headless Metal target streams through it, nothing large
touches the disk) and renders its mixer offline into a WAV
(EM_REPLAY_AUDIO).  Each BMP is reduced to the comparison frame:
  gs    (default) the port's frame sampled at the GS sample points of a
        512x224 field (the same nearest-pixel rule as the port's
        tools/test_fb2_pixels.py): like with like with the PS2 field;
  area  the 4:3 game rect box-averaged to 640x480 (smoother; the port's
        Metal frame at a higher resolution than the GS field).

Runs with any python3 that has numpy.  Output is local and ignored
(build/video_compare/...).
"""
from __future__ import annotations

import argparse
import concurrent.futures as cf
import os
import shutil
import struct
import subprocess
import sys
import threading
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import emrec  # noqa: E402

DEFAULT_PORT = ROOT.parent / "extermination-port"
GS_W, GS_H = 512, 224


def game_rect(w: int, h: int):
    """The port's 4:3 viewport inside its target (em_gfx_metal.m)."""
    vw, vh, vx, vy = float(w), float(h), 0.0, 0.0
    if w * 3.0 >= h * 4.0:
        vw = h * 4.0 / 3.0
        vx = (w - vw) * 0.5
    else:
        vh = w * 3.0 / 4.0
        vy = (h - vh) * 0.5
    return vx, vy, vw, vh


def bmp_to_frame(data: bytes, sampling: str):
    import numpy as np
    if data[:2] != b"BM":
        raise ValueError("not a BMP")
    off, = struct.unpack_from("<I", data, 10)
    w, h = struct.unpack_from("<ii", data, 18)
    stride = (w * 3 + 3) & ~3
    img = np.frombuffer(data, dtype=np.uint8, count=stride * h, offset=off).reshape(h, stride)
    img = img[::-1, :w * 3].reshape(h, w, 3)[:, :, ::-1]          # top-down RGB (a view)
    vx, vy, vw, vh = game_rect(w, h)
    if sampling == "gs":
        xs = np.floor(vx + np.arange(GS_W) * vw / GS_W).astype(np.int64)
        ys = np.floor(vy + np.arange(GS_H) * vh / GS_H).astype(np.int64)
        return np.ascontiguousarray(img[ys][:, xs])
    x0, y0, rw, rh = int(round(vx)), int(round(vy)), int(round(vw)), int(round(vh))
    rect = img[y0:y0 + rh, x0:x0 + rw].astype(np.float32)
    fy, fx = rh // 480, rw // 640
    if fy >= 1 and fx >= 1 and rh % 480 == 0 and rw % 640 == 0:
        out = rect.reshape(480, fy, 640, fx, 3).mean(axis=(1, 3))
    else:
        ys = (np.arange(480) * rh // 480)
        xs = (np.arange(640) * rw // 640)
        out = rect[ys][:, xs]
    return np.clip(out + 0.5, 0, 255).astype(np.uint8)


class FifoReader(threading.Thread):
    """Reads the port's BMPs from the FIFO as one stream.  The reader keeps
    its end open and holds a dummy write end, so the port's open/write/close
    per capture never meets a closed reader (no frame is lost between two
    captures) and each BMP is cut from the stream by its own size field."""

    def __init__(self, fifo: Path, frames: Path, sampling: str):
        super().__init__(daemon=True)
        self.fifo, self.frames, self.sampling = fifo, frames, sampling
        self.count = 0
        self.errors: list[str] = []
        self.pool = cf.ThreadPoolExecutor(max_workers=4)
        self.futures: list = []
        self.rfd = os.open(fifo, os.O_RDONLY | os.O_NONBLOCK)
        self.wfd = os.open(fifo, os.O_WRONLY)
        os.set_blocking(self.rfd, True)

    def _read(self, n: int) -> bytes:
        buf = bytearray()
        while len(buf) < n:
            chunk = os.read(self.rfd, min(1 << 20, n - len(buf)))
            if not chunk:
                break
            buf += chunk
        return bytes(buf)

    def run(self) -> None:
        while True:
            head = self._read(6)
            if len(head) < 6:
                break
            if head[:2] != b"BM":
                self.errors.append(f"stream lost sync after frame {self.count}")
                break
            size, = struct.unpack_from("<I", head, 2)
            data = head + self._read(size - 6)
            if len(data) < size:
                self.errors.append(f"frame {self.count} truncated")
                break
            idx = self.count
            self.count += 1
            self.futures.append(self.pool.submit(self._convert, idx, data))
            while sum(1 for x in self.futures[-16:] if not x.done()) >= 12:
                time.sleep(0.002)          # back-pressure: keep memory bounded

    def _convert(self, idx: int, data: bytes) -> None:
        try:
            emrec.write_png(self.frames / f"{idx:06d}.png", bmp_to_frame(data, self.sampling))
        except Exception as exc:          # noqa: BLE001
            self.errors.append(f"frame {idx}: {exc}")

    def finish(self) -> None:
        """After the port has exited: drop the dummy writer, drain, join."""
        os.close(self.wfd)
        self.join(timeout=60)
        os.close(self.rfd)
        for fut in self.futures:
            fut.result()
        self.pool.shutdown()


def run(args) -> int:
    rec_path = Path(args.recording).resolve()
    out = Path(args.out).resolve()
    port = Path(args.port).resolve()
    binary = port / "build/extermination"
    if not binary.exists():
        sys.exit(f"native: {binary} not found; build the port first (make -C {port} all)")
    if out.exists():
        shutil.rmtree(out)
    frames = out / "frames"
    frames.mkdir(parents=True)
    fifo = out / "frames.fifo"
    os.mkfifo(fifo)
    env = {k: v for k, v in os.environ.items() if not k.startswith("EM_")}
    env.update(EM_HEADLESS="1", EM_UNCAPPED="1", EM_INPUT_PLAY=str(rec_path),
               EM_INPUT_RECORD=str(out / "native.rec"), EM_REPLAY_CAPTURE=str(fifo),
               EM_REPLAY_CAPTURE_EVERY=str(args.stride), EM_REPLAY_MAX_OVERRUN=str(args.max_overrun))
    rec = emrec.read(rec_path)
    timing = args.disc_timing
    if timing == "recorded":
        timing = rec.header.get("ps2_disc_drive_timing", "0")
    env["EM_PS2_DISC_DRIVE_TIMING"] = "1" if str(timing) == "1" else "0"
    for item in rec.header.get("env", []):                   # launch switches the recording used
        var, _, val = item.partition("=")
        if var in ("EM_NEW_GAME", "EM_SKIP_STARTUP"):
            env[var] = val
    if args.audio:
        env["EM_REPLAY_AUDIO"] = str(out / "audio.wav")
    reader = FifoReader(fifo, frames, args.sampling)
    reader.start()
    t0 = time.monotonic()
    with open(out / "port.log", "w") as logf:
        proc = subprocess.Popen([str(binary)], cwd=port, env=env, stdout=logf, stderr=subprocess.STDOUT)
        try:
            rc = proc.wait(timeout=args.timeout)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
            rc = -9
    reader.finish()
    fifo.unlink()
    seconds = time.monotonic() - t0
    log_rec = emrec.read(out / "native.rec")
    caps = [r for r in log_rec.rows if r["cap"] >= 0]
    print(f"native: exit {rc} in {seconds:.1f} s; {len(log_rec.ticks())} ticks, {len(caps)} captures requested, "
          f"{reader.count} frames received, drive timing {env['EM_PS2_DISC_DRIVE_TIMING']}"
          + (f"; errors: {reader.errors[:3]}" if reader.errors else ""), flush=True)
    tail = (out / "port.log").read_text(errors="replace").splitlines()
    for line in tail:
        if line.startswith("replay:"):
            print("  " + line)
    if reader.count != len(caps):
        print(f"native: WARNING: {len(caps)} captures requested but {reader.count} frames arrived", flush=True)
    return 0 if rc == 0 else 1


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("recording")
    ap.add_argument("--out", required=True)
    ap.add_argument("--port", default=str(DEFAULT_PORT), help="the port checkout (build/extermination, assets/)")
    ap.add_argument("--stride", type=int, default=4)
    ap.add_argument("--sampling", choices=["gs", "area"], default="gs")
    ap.add_argument("--audio", action="store_true", help="render the port's mixer offline into audio.wav")
    ap.add_argument("--disc-timing", choices=["recorded", "0", "1"], default="recorded",
                    help="EM_PS2_DISC_DRIVE_TIMING for the playback (default: as recorded)")
    ap.add_argument("--max-overrun", type=int, default=1800)
    ap.add_argument("--timeout", type=float, default=1800.0)
    return run(ap.parse_args(argv))


if __name__ == "__main__":
    sys.exit(main())
