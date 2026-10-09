"""native.py - play an EMREC recording on the native port (headless) and keep
its frames and audio for the video comparison.  docs/VIDEO_COMPARE.md.

The port (../extermination-port, src/game/em_replay.c) replays the recording
with EM_INPUT_PLAY, writes its playback log with EM_INPUT_RECORD, requests a
frame capture at every captured tick (EM_REPLAY_CAPTURE, a FIFO here: the
24-bit BMP of the headless Metal target streams through it, nothing large
touches the disk) and renders its mixer offline into a WAV
(EM_REPLAY_AUDIO).

A capture of a frame that shows a GS field (every world frame of the
Original profile since the port's chain step GSFRAME) also writes the field
itself to <capture path>.gsfield (512x224x4 bytes, R G B A; port
em_gfx_metal.m gsw_complete: the BMP is written and closed first, then the
field, on the same thread).  Here that path is a symlink to the same FIFO,
so each field follows its own BMP in one ordered stream: no file is
overwritten, nothing can race.  A BMP is recognised by its 54-byte header
(identical for every capture of a run); anything else after a BMP is its
field.  After the run the parsed sequence is checked against the port's
"capture: wrote ..." log lines.

Each capture is reduced to the comparison frame:
  gs    (default) a 512x224 field, like with like with the PS2 side's GS
        field.  Where the port wrote the field, the frame is that exact
        field; field pixels the port's overlay pass drew over (letterbox
        bands, subtitles, fades, HUD: GPU-drawn after the field, not in it)
        take the presented frame's pixel at the field pixel's centre.
        Frames without a field (the status / ITEM / BATTERY pages, loads)
        are the presented frame sampled at the field pixels' centres.
        (Until 2026-10-09 the presented frame was sampled at each field
        pixel's left/top edge, which lands in the neighbouring field pixel
        and duplicated about half the rows and columns: blocky frames.)
  area  the 4:3 game rect box-averaged to 640x480 (smoother; the presented
        frame at a higher resolution than the GS field; fields not used).
frames.json lists, per capture, whether it had a field and the fraction of
its pixels the overlay covered.

Runs with any python3 that has numpy.  Output is local and ignored
(build/video_compare/...).
"""
from __future__ import annotations

import argparse
import concurrent.futures as cf
import json
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


FIELD_BYTES = GS_W * GS_H * 4
BMP_HEADER = 54


def bmp_rgb(data: bytes):
    """The port's 24-bit bottom-up BMP as a top-down RGB array (a view)."""
    import numpy as np
    if data[:2] != b"BM":
        raise ValueError("not a BMP")
    off, = struct.unpack_from("<I", data, 10)
    w, h = struct.unpack_from("<ii", data, 18)
    stride = (w * 3 + 3) & ~3
    img = np.frombuffer(data, dtype=np.uint8, count=stride * h, offset=off).reshape(h, stride)
    return img[::-1, :w * 3].reshape(h, w, 3)[:, :, ::-1]


def centre_samples(img):
    """The presented frame at the centre of each pixel of a 512x224 field
    spread over the 4:3 game rect (the port's EM_GFX_FIELD_SPREAD shows field
    pixel (x, y) over the rect's columns [x*vw/512, (x+1)*vw/512) and rows
    likewise; the port's tools/test_fb2_pixels.py shown_field)."""
    import numpy as np
    h, w, _ = img.shape
    vx, vy, vw, vh = game_rect(w, h)
    xs = np.floor(vx + (np.arange(GS_W) + 0.5) * vw / GS_W).astype(np.int64)
    ys = np.floor(vy + (np.arange(GS_H) + 0.5) * vh / GS_H).astype(np.int64)
    return np.ascontiguousarray(img[ys][:, xs])


def bmp_to_frame(data: bytes, sampling: str, field: bytes | None = None):
    """-> (frame, overlay fraction or None).  See the module docstring."""
    import numpy as np
    img = bmp_rgb(data)
    h, w, _ = img.shape
    if sampling == "gs":
        shown = centre_samples(img)
        if field is None:
            return shown, None
        exact = np.frombuffer(field, dtype=np.uint8).reshape(GS_H, GS_W, 4)[:, :, :3]
        over = (shown != exact).any(axis=2)            # the overlay pass drew here
        out = exact.copy()
        out[over] = shown[over]
        return out, float(over.mean())
    vx, vy, vw, vh = game_rect(w, h)
    x0, y0, rw, rh = int(round(vx)), int(round(vy)), int(round(vw)), int(round(vh))
    rect = img[y0:y0 + rh, x0:x0 + rw].astype(np.float32)
    fy, fx = rh // 480, rw // 640
    if fy >= 1 and fx >= 1 and rh % 480 == 0 and rw % 640 == 0:
        out = rect.reshape(480, fy, 640, fx, 3).mean(axis=(1, 3))
    else:
        ys = (np.arange(480) * rh // 480)
        xs = (np.arange(640) * rw // 640)
        out = rect[ys][:, xs]
    return np.clip(out + 0.5, 0, 255).astype(np.uint8), None


class FifoReader(threading.Thread):
    """Reads the port's captures from the FIFO as one stream: each BMP, then
    (for a field frame) its 512x224 field, which the port writes through the
    <fifo>.gsfield symlink after closing the BMP.  The reader keeps its end
    open and holds a dummy write end, so the port's open/write/close per
    capture never meets a closed reader (nothing is lost between two writes).
    A BMP is cut from the stream by its own size field; the item after it is
    the next BMP if its first 54 bytes equal the run's BMP header, else that
    BMP's field (FIELD_BYTES)."""

    def __init__(self, fifo: Path, frames: Path, sampling: str):
        super().__init__(daemon=True)
        self.fifo, self.frames, self.sampling = fifo, frames, sampling
        self.count = 0
        self.has_field: list[bool] = []
        self.overlay: dict[int, float] = {}
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

    def _emit(self, bmp: bytes, field: bytes | None) -> None:
        idx = self.count
        self.count += 1
        self.has_field.append(field is not None)
        self.futures.append(self.pool.submit(self._convert, idx, bmp, field))
        while sum(1 for x in self.futures[-16:] if not x.done()) >= 12:
            time.sleep(0.002)          # back-pressure: keep memory bounded

    def run(self) -> None:
        header = pending = None
        while True:
            head = self._read(BMP_HEADER)
            if len(head) < BMP_HEADER:
                if head:
                    self.errors.append(f"stream ended inside an item after frame {self.count}")
                break
            if header is None or head == header:
                if head[:2] != b"BM":
                    self.errors.append(f"stream lost sync after frame {self.count}")
                    break
                header = head
                if pending is not None:
                    self._emit(pending, None)          # a capture without a field
                size, = struct.unpack_from("<I", head, 2)
                data = head + self._read(size - BMP_HEADER)
                if len(data) < size:
                    self.errors.append(f"frame {self.count} truncated")
                    pending = None
                    break
                pending = data
            else:
                if pending is None:
                    self.errors.append(f"a field without its BMP after frame {self.count}")
                    break
                rest = self._read(FIELD_BYTES - BMP_HEADER)
                if len(rest) < FIELD_BYTES - BMP_HEADER:
                    self.errors.append(f"field of frame {self.count} truncated")
                    break
                self._emit(pending, head + rest)
                pending = None
        if pending is not None:
            self._emit(pending, None)

    def _convert(self, idx: int, data: bytes, field: bytes | None) -> None:
        try:
            frame, over = bmp_to_frame(data, self.sampling, field)
            emrec.write_png(self.frames / f"{idx:06d}.png", frame)
            if over is not None:
                self.overlay[idx] = over
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


def logged_captures(log: Path, fifo: Path) -> tuple[list[bool], list[str]]:
    """The port's own record of what it wrote into the FIFO, in order:
    per BMP, whether a field followed (port log 'capture: wrote ...')."""
    seq: list[bool] = []
    errs: list[str] = []
    bmp, fld = f"capture: wrote {fifo} (", f"capture: wrote {fifo}.gsfield ("
    for line in log.read_text(errors="replace").splitlines():
        if line.startswith(bmp):
            seq.append(False)
        elif line.startswith(fld):
            if not seq or seq[-1]:
                errs.append("a logged field without its BMP")
            elif not line[len(fld):].startswith(f"{GS_W}x{GS_H} "):
                errs.append(f"a field that is not {GS_W}x{GS_H}: {line.strip()}")
            else:
                seq[-1] = True
        elif line.startswith("capture: cannot write"):
            errs.append(line.strip())
    return seq, errs


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
    field_link = out / "frames.fifo.gsfield"           # the port's <capture>.gsfield: the same stream
    os.symlink(fifo.name, field_link)
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
    field_link.unlink()
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
    ok = not reader.errors
    if reader.count != len(caps):
        print(f"native: WARNING: {len(caps)} captures requested but {reader.count} frames arrived", flush=True)
        ok = False
    logged, log_errs = logged_captures(out / "port.log", fifo)
    if log_errs or logged != reader.has_field:
        print(f"native: ERROR: the stream does not match the port's capture log ({len(logged)} logged, "
              f"{sum(logged)} with a field; parsed {reader.count}, {sum(reader.has_field)} with a field)"
              + (f"; {log_errs[:3]}" if log_errs else ""), flush=True)
        ok = False
    over = reader.overlay
    n_field = sum(reader.has_field)
    covered = sum(1 for v in over.values() if v > 0)
    print(f"native: {n_field} of {reader.count} frames carry the exact GS field"
          + (f" (overlay pass drawn over {covered} of them)" if args.sampling == "gs" else " (unused: area)"),
          flush=True)
    (out / "frames.json").write_text(json.dumps(
        {"sampling": args.sampling, "frames": [
            {"cap": i, "field": reader.has_field[i], "overlay": (round(over[i], 6) if i in over else None)}
            for i in range(reader.count)]}, indent=0) + "\n")
    return 0 if rc == 0 and ok else 1


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("recording")
    ap.add_argument("--out", required=True)
    ap.add_argument("--port", default=str(DEFAULT_PORT), help="the port checkout (build/extermination, assets/)")
    ap.add_argument("--stride", type=int, default=4)
    ap.add_argument("--sampling", choices=["gs", "area"], default="gs",
                    help="gs (default): the port's exact 512x224 GS field per capture (its .gsfield, streamed "
                         "after its BMP), with the presented frame's field-pixel centres where the overlay pass "
                         "drew (letterbox, subtitles, fades) and for frames without a field (status pages, "
                         "loads); area: the presented 4:3 frame box-averaged to 640x480")
    ap.add_argument("--audio", action="store_true", help="render the port's mixer offline into audio.wav")
    ap.add_argument("--disc-timing", choices=["recorded", "0", "1"], default="recorded",
                    help="EM_PS2_DISC_DRIVE_TIMING for the playback (default: as recorded)")
    ap.add_argument("--max-overrun", type=int, default=1800)
    ap.add_argument("--timeout", type=float, default=1800.0)
    return run(ap.parse_args(argv))


if __name__ == "__main__":
    sys.exit(main())
