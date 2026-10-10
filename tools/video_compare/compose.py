"""compose.py - align the PS2 and native frame streams, label them, speed them
up and encode a side-by-side H.264 MP4 (plus the drift report).
docs/VIDEO_COMPARE.md.

Needs numpy and Pillow (labels are drawn with Pillow; this ffmpeg build has
no drawtext) and ffmpeg/ffprobe on PATH.

Alignment: both runs log, per tick, the recorded segment they applied and the
offset inside it (columns seg/off).  Frames were captured at offsets that are
multiples of the capture stride, so frame k of segment s on one side shows the
same game tick, relative to the segment start, as frame k on the other.
  * play / cutscene segments: paired by offset; if one side's segment is
    longer, the other side's last frame is held and tagged "held".
  * load segments: --loads trim (default) keeps as many frames as the shorter
    side has and drops the longer side's middle frames (its first and last
    frames stay, so the veil's start and its fade-out line up); the dropped
    count is tagged.  --loads hold keeps every frame of the longer side and
    holds the shorter side's last load frame, tagged "loading".
Audio (--audio original|native|both|none): each output frame takes its own
side's audio for the game ticks it stands for; held or missing frames are
silent.  both = the original in the LEFT channel and the port in the RIGHT
(each downmixed to mono), so one track works in every player (Discord's
inline player shows no track menu).  With --speed > 1 the audio is sped up
with ffmpeg's atempo (pitch kept), so it stays in sync with the video.
"""
from __future__ import annotations

import json
import math
import shutil
import struct
import subprocess
import sys
import wave
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import emrec  # noqa: E402

LEFT_LABEL = "Original \u2014 PS2 (PCSX2)"
RIGHT_LABEL = "Native port"
FONT_CANDIDATES = ["/System/Library/Fonts/Helvetica.ttc", "/System/Library/Fonts/SFNS.ttf",
                   "/System/Library/Fonts/Supplemental/Arial.ttf",
                   "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "C:/Windows/Fonts/arial.ttf"]


def font(size: int, bold: bool = False):
    from PIL import ImageFont
    for path in FONT_CANDIDATES:
        try:
            return ImageFont.truetype(path, size, index=1 if bold and path.endswith(".ttc") else 0)
        except OSError:
            continue
    return ImageFont.load_default(size=size)


# --------------------------------------------------------------- alignment

def load_side(folder: Path, logname: str):
    rec = emrec.read(folder / logname)
    rows = [r for r in rec.ticks() if r["seg"] >= 0]
    caps = {}
    for r in rows:
        if r["cap"] >= 0:
            caps.setdefault(r["seg"], []).append(r)
    return rec, rows, caps


def seg_lengths(rows: list) -> dict:
    out: dict = {}
    for r in rows:
        out[r["seg"]] = out.get(r["seg"], 0) + 1
    return out


def align(native: dict, ps2: dict, loads: str, n_len: dict | None = None, p_len: dict | None = None) -> list:
    """-> list of entries {"seg", "ph", "n": row|None, "p": row|None, "tag_n", "tag_p"}.
    n_len / p_len: every segment's tick count per side (for the trim tag)."""
    out = []
    for seg in sorted(set(native) | set(ps2)):
        a, b = native.get(seg, []), ps2.get(seg, [])
        ph = (a or b)[0]["ph"]
        if ph == "L" and loads == "trim" and a and b and len(a) != len(b):
            n = min(len(a), len(b))
            longer, shorter = (a, b) if len(a) > len(b) else (b, a)
            keep = longer[:(n + 1) // 2] + longer[len(longer) - n // 2:] if n else []
            side = "n" if longer is a else "p"
            if n_len is not None and p_len is not None:
                dropped_ticks = abs(n_len.get(seg, 0) - p_len.get(seg, 0))
            else:
                dropped_ticks = longer[-1]["off"] - shorter[-1]["off"]
            for x, y in zip(keep, shorter):
                e = {"seg": seg, "ph": ph, "n": x if side == "n" else y, "p": y if side == "n" else x,
                     "tag_n": None, "tag_p": None}
                e["tag_" + side] = f"load trimmed: {dropped_ticks} ticks longer"
                out.append(e)
            continue
        m = max(len(a), len(b))
        for i in range(m):
            e = {"seg": seg, "ph": ph, "n": a[i] if i < len(a) else (a[-1] if a else None),
                 "p": b[i] if i < len(b) else (b[-1] if b else None), "tag_n": None, "tag_p": None,
                 "held_n": i >= len(a), "held_p": i >= len(b)}
            for s, held in (("n", e["held_n"]), ("p", e["held_p"])):
                if held and e[s] is not None:
                    e["tag_" + s] = "held: the other side is still loading" if ph == "L" else "held: segment ended earlier here"
                elif e[s] is None:
                    e["tag_" + s] = "no frame"
            out.append(e)
    return out


# ------------------------------------------------------------------ drift

def drift(native_rows: list, ps2_rows: list, tol_pos: float, tol_yaw: float) -> dict:
    by = {(r["seg"], r["off"]): r for r in ps2_rows if r["seg"] >= 0}
    first = None
    compared = exact = 0
    worst = (0.0, None)
    seg_len_n, seg_len_p = {}, {}
    for r in native_rows:
        seg_len_n[r["seg"]] = seg_len_n.get(r["seg"], 0) + 1
    for r in ps2_rows:
        if r["seg"] >= 0:
            seg_len_p[r["seg"]] = seg_len_p.get(r["seg"], 0) + 1
    phases = {r["seg"]: r["ph"] for r in native_rows}
    phases.update({r["seg"]: r["ph"] for r in ps2_rows if r["seg"] >= 0 and r["seg"] not in phases})
    for r in native_rows:
        q = by.get((r["seg"], r["off"]))
        if q is None or r["ph"] not in "PC" or q["ph"] not in "PC":
            continue                     # loads and the title carry no player position
        compared += 1
        d = math.dist((r["x"], r["y"], r["z"]), (q["x"], q["y"], q["z"]))
        dy = abs(math.remainder(r["yaw"] - q["yaw"], 2 * math.pi))
        same = all(struct.pack("<f", r[k]) == struct.pack("<f", q[k]) for k in ("x", "y", "z", "yaw"))
        exact += same
        if d > worst[0]:
            worst = (d, (r["seg"], r["off"]))
        if first is None and (d > tol_pos or dy > tol_yaw):
            first = {"seg": r["seg"], "off": r["off"], "phase": r["ph"], "native_counter": r["counter"],
                     "ps2_counter": q["counter"], "native_pos": [r["x"], r["y"], r["z"]],
                     "ps2_pos": [q["x"], q["y"], q["z"]], "native_yaw": r["yaw"], "ps2_yaw": q["yaw"],
                     "distance": d, "yaw_difference": dy}
    segs = [{"seg": s, "phase": phases.get(s), "native_ticks": seg_len_n.get(s, 0),
             "ps2_ticks": seg_len_p.get(s, 0)} for s in sorted(set(seg_len_n) | set(seg_len_p))]
    return {"ticks_compared": compared, "position_heading_bit_exact": exact,
            "tolerance": {"position": tol_pos, "yaw": tol_yaw}, "first_drift": first,
            "largest_position_difference": {"distance": worst[0], "at_seg_off": worst[1]},
            "segments": segs}


def drift_text(d: dict) -> str:
    lines = [f"play/cutscene ticks compared (same segment and offset): {d['ticks_compared']}; position and heading "
             f"bit-exact on {d['position_heading_bit_exact']}"]
    f = d["first_drift"]
    if f:
        lines.append(f"first drift beyond the tolerance (position {d['tolerance']['position']}, yaw "
                     f"{d['tolerance']['yaw']} rad): segment {f['seg']} ({f['phase']}) offset {f['off']}, "
                     f"native counter {f['native_counter']} / PS2 counter {f['ps2_counter']}: distance "
                     f"{f['distance']:.4f}, yaw difference {f['yaw_difference']:.4f}")
    else:
        lines.append("no drift beyond the tolerance")
    lines.append("segments (recorded index, phase, ticks native / PS2):")
    for s in d["segments"]:
        mark = "" if s["native_ticks"] == s["ps2_ticks"] else "   <- lengths differ"
        lines.append(f"  {s['seg']:3d} {s['phase']}  {s['native_ticks']:6d} / {s['ps2_ticks']:6d}{mark}")
    return "\n".join(lines)


# ------------------------------------------------------------------ audio

def read_wav(path: Path):
    import numpy as np
    with wave.open(str(path)) as w:
        ch, rate, width, n = w.getnchannels(), w.getframerate(), w.getsampwidth(), w.getnframes()
        data = w.readframes(n)
    if width != 2:
        raise ValueError(f"{path}: {width * 8}-bit WAV (16-bit expected)")
    a = np.frombuffer(data, dtype="<i2").reshape(-1, ch).astype(np.float32)
    if ch == 1:
        a = np.repeat(a, 2, axis=1)
    return a[:, :2], rate


def ps2_audio_index(ps2_dir: Path, wav_frames: int, rate: int):
    """Legacy runs (no `af` column).  PS2 sample index of a tick's loop top: the WAV ends where the session
    stopped (the last logged loop top, the VM stays paused there until the
    emulator exits); earlier loop tops are placed by the vsync counter
    0x00810E90 (one NTSC field = rate / 59.94 samples)."""
    doc = json.loads((ps2_dir / "ps2_extra.json").read_text())
    extra = doc["rows"]
    per_field = rate / emrec.TICK_HZ
    end_vsync = doc.get("end_vsync") or extra[-1]["vsync"]
    return lambda row: int(round(wav_frames - (end_vsync - extra[row["step"]]["vsync"]) * per_field))


def build_audio(entries: list, side: str, src, index, ticks_per_frame: int, rate: int):
    import numpy as np
    per = ticks_per_frame * rate / emrec.TICK_HZ
    total = int(round(len(entries) * per))
    out = np.zeros((total, 2), dtype=np.float32)
    if src is None:
        return out
    for i, e in enumerate(entries):
        a, b = int(round(i * per)), int(round((i + 1) * per))
        r = e[side]
        if r is None or e.get("held_" + side) or (e.get("tag_" + side) or "").startswith("held"):
            continue
        s = index(r)
        if s is None or s < 0:
            continue
        chunk = src[s:s + (b - a)]
        out[a:a + len(chunk)] = chunk
    return out


def write_wav(path: Path, a, rate: int) -> None:
    import numpy as np
    pcm = np.clip(np.round(a), -32768, 32767).astype("<i2")
    with wave.open(str(path), "wb") as w:
        w.setnchannels(2)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(pcm.tobytes())


# ----------------------------------------------------------------- frames

class Renderer:
    def __init__(self, side_h: int, title: str | None, subtitle: str | None, speed_text: str,
                 scale_filter: str, overlay: bool, sound: str | None = None):
        from PIL import Image, ImageDraw
        self.Image, self.ImageDraw = Image, ImageDraw
        self.side_h = side_h - side_h % 2
        self.side_w = (self.side_h * 4 // 3) // 2 * 2
        self.gap = max(4, self.side_h // 120) // 2 * 2
        s = self.side_h / 480
        self.f_label = font(int(26 * s), bold=True)
        self.f_title = font(int(24 * s), bold=True)
        self.f_sub = font(int(18 * s))
        self.f_tag = font(int(16 * s), bold=True)
        self.f_info = font(int(14 * s))
        pad = int(8 * s)
        lines = [x for x in (title, subtitle) if x]
        title_h = sum((int(30 * s) if i == 0 and title else int(24 * s)) for i, _ in enumerate(lines))
        self.label_h = int(40 * s)
        self.header_h = pad + title_h + self.label_h
        self.footer_h = int(22 * s) if overlay else 0
        self.W = self.side_w * 2 + self.gap
        self.H = self.header_h + self.side_h + self.footer_h
        self.W += self.W % 2
        self.H += self.H % 2
        self.filter = Image.NEAREST if scale_filter == "nearest" else Image.BILINEAR
        self.overlay = overlay
        base = Image.new("RGB", (self.W, self.H), (16, 16, 18))
        d = ImageDraw.Draw(base)
        y = pad
        if title:
            d.text((self.W // 2, y), title, font=self.f_title, fill=(240, 240, 240), anchor="mt")
            y += int(30 * s)
        if subtitle:
            d.text((self.W // 2, y), subtitle, font=self.f_sub, fill=(190, 190, 190), anchor="mt")
            y += int(24 * s)
        ly = self.header_h - self.label_h // 2
        d.text((self.side_w // 2, ly), LEFT_LABEL, font=self.f_label, fill=(255, 255, 255), anchor="mm")
        d.text((self.side_w + self.gap + self.side_w // 2, ly), RIGHT_LABEL, font=self.f_label,
               fill=(255, 255, 255), anchor="mm")
        if speed_text:
            d.rounded_rectangle((self.W - int(70 * s), pad, self.W - pad, pad + int(30 * s)),
                                radius=int(6 * s), fill=(200, 60, 40))
            d.text((self.W - int(70 * s) // 2 - pad // 2, pad + int(15 * s)), speed_text, font=self.f_title,
                   fill=(255, 255, 255), anchor="mm")
        if sound:
            d.text((pad, pad), sound, font=self.f_info, fill=(170, 170, 170), anchor="lt")
        self.base = base
        self.cache: dict = {}

    def side(self, path: Path | None):
        if path is None:
            return self.Image.new("RGB", (self.side_w, self.side_h), (0, 0, 0))
        key = str(path)
        if key in self.cache:
            return self.cache[key]
        im = self.Image.open(path).convert("RGB")
        if im.size == (512, 224):                                     # a GS field: 224 lines shown as 448
            im = im.resize((512, 448), self.Image.NEAREST)
        im = im.resize((self.side_w, self.side_h), self.filter)
        if len(self.cache) > 8:
            self.cache.pop(next(iter(self.cache)))
        self.cache[key] = im
        return im

    def tag(self, d, x: int, y: int, text: str) -> None:
        s = self.side_h / 480
        w = d.textlength(text, font=self.f_tag)
        d.rounded_rectangle((x, y, x + w + int(14 * s), y + int(24 * s)), radius=int(5 * s), fill=(0, 0, 0))
        d.text((x + int(7 * s), y + int(12 * s)), text, font=self.f_tag, fill=(255, 210, 80), anchor="lm")

    def frame(self, left: Path | None, right: Path | None, tag_l: str | None, tag_r: str | None,
              info: str | None) -> bytes:
        im = self.base.copy()
        im.paste(self.side(left), (0, self.header_h))
        im.paste(self.side(right), (self.side_w + self.gap, self.header_h))
        d = self.ImageDraw.Draw(im)
        s = self.side_h / 480
        if tag_l:
            self.tag(d, int(8 * s), self.header_h + int(8 * s), tag_l)
        if tag_r:
            self.tag(d, self.side_w + self.gap + int(8 * s), self.header_h + int(8 * s), tag_r)
        if self.overlay and info:
            d.text((self.W // 2, self.H - self.footer_h // 2), info, font=self.f_info, fill=(170, 170, 170),
                   anchor="mm")
        return im.tobytes()


# ------------------------------------------------------------------ main

def ffprobe(path: Path) -> dict:
    r = subprocess.run(["ffprobe", "-v", "error", "-show_entries",
                        "format=duration,size,bit_rate:stream=codec_name,width,height,nb_frames,r_frame_rate,"
                        "sample_rate,channels", "-of", "json", str(path)], capture_output=True, text=True)
    return json.loads(r.stdout or "{}")


def atempo_chain(speed: float) -> str:
    parts, s = [], speed
    while s > 2.0 + 1e-9:
        parts.append("atempo=2.0")
        s /= 2.0
    parts.append(f"atempo={s:.6f}")
    return ",".join(parts)


def compose(args) -> dict:
    native_dir, ps2_dir, out = Path(args.native), Path(args.ps2), Path(args.out)
    work = out.parent / (out.stem + "_work")
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    _, n_rows, n_caps = load_side(native_dir, "native.rec")
    p_rec, p_rows, p_caps = load_side(ps2_dir, "ps2.rec")
    entries = align(n_caps, p_caps, args.loads, seg_lengths(n_rows), seg_lengths(p_rows))
    if not entries:
        raise SystemExit("compose: no captured frames to align")
    stride = int(p_rec.header.get("stride", args.stride))
    m = max(1, round(args.speed * emrec.TICK_HZ / (args.fps * stride)))
    fps = args.speed * emrec.TICK_HZ / (m * stride)
    picked = entries[::m]
    d = drift(n_rows, p_rows, args.tol_pos, args.tol_yaw)
    (out.parent / (out.stem + "_drift.json")).write_text(json.dumps(d, indent=1) + "\n")
    (out.parent / (out.stem + "_drift.txt")).write_text(drift_text(d) + "\n")
    speed_text = (f"{args.speed:g}\u00d7" if args.speed != 1 else "1\u00d7")
    notes = []
    audio = args.audio
    have_n, have_p = (native_dir / "audio.wav").exists(), (ps2_dir / "audio.wav").exists()
    if audio in ("original", "both") and not have_p:
        notes.append("no PS2 audio.wav: " + ("the video is silent" if audio == "original" or not have_n
                                             else "only the port's sound is used (labelled)"))
        audio = "native" if audio == "both" and have_n else "none"
    if audio == "native" and not have_n:
        notes.append("no native audio.wav: the video is silent")
        audio = "none"
    sound = {"native": "sound: native port only", "original": "sound: original only",
             "both": "sound: original LEFT, native port RIGHT"}.get(audio)
    r = Renderer(args.height, args.title, args.subtitle, speed_text, args.scale_filter, args.info, sound)

    def frame_path(folder: Path, row):
        return None if row is None else folder / "frames" / f"{row['cap']:06d}.png"

    video = work / "video.mkv"
    enc = subprocess.Popen(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s",
                            f"{r.W}x{r.H}", "-r", f"{fps:.6f}", "-i", "-", "-c:v", "libx264rgb", "-qp", "0",
                            "-preset", "ultrafast", str(video)], stdin=subprocess.PIPE)
    summary_frames = int(round(args.summary_seconds * fps)) if args.summary_seconds > 0 else 0
    for e in picked:
        info = None
        if args.info:
            info = (f"segment {e['seg']} ({e['ph']})  native counter "
                    f"{e['n']['counter'] if e['n'] else '-'}  |  PS2 counter {e['p']['counter'] if e['p'] else '-'}")
        enc.stdin.write(r.frame(frame_path(ps2_dir, e["p"]), frame_path(native_dir, e["n"]), e["tag_p"],
                                e["tag_n"], info))
    if summary_frames:
        enc.stdin.write(summary_card(r, d, args) * summary_frames)
    enc.stdin.close()
    if enc.wait() != 0:
        raise SystemExit("compose: ffmpeg (intermediate) failed")
    duration = (len(picked) + summary_frames) / fps
    # audio
    audio_wav = None
    if audio != "none":
        rate = emrec.AUDIO_RATE
        n_src = p_src = None
        if audio in ("native", "both"):
            n_src, nr = read_wav(native_dir / "audio.wav")
            assert nr == rate
        if audio in ("original", "both"):
            p_src, rate_p = read_wav(ps2_dir / "audio.wav")
            if rate_p != rate:
                import numpy as np
                idx = (np.arange(int(len(p_src) * rate / rate_p)) * rate_p / rate).astype(int)
                p_src = p_src[idx]
        p_index = None
        if p_src is not None:
            # a fork run logs each tick's sample index in audio.wav (column af, sample-exact);
            # a legacy run is placed by the vsync counter from the WAV's end
            p_index = ((lambda row: row["af"] if row["af"] >= 0 else None) if any(r["af"] >= 0 for r in p_rows)
                       else ps2_audio_index(ps2_dir, len(p_src), rate))
        tpf = m * stride
        a_n = build_audio(picked, "n", n_src, lambda row: row["af"] if row["af"] >= 0 else None, tpf, rate)
        a_p = build_audio(picked, "p", p_src, p_index, tpf, rate)
        import numpy as np
        if audio == "both":
            mix = np.stack([a_p.mean(axis=1), a_n.mean(axis=1)], axis=1)
        elif audio == "native":
            mix = a_n
        else:
            mix = a_p
        pad = int(round(summary_frames * tpf * rate / emrec.TICK_HZ))
        mix = np.concatenate([mix, np.zeros((pad, 2), dtype=np.float32)])
        audio_wav = work / "audio.wav"
        write_wav(audio_wav, mix, rate)
    # final encode
    cmd = ["ffmpeg", "-v", "error", "-y", "-i", str(video)]
    if audio_wav:
        cmd += ["-i", str(audio_wav)]
    vcodec = ["-c:v", "libx264", "-pix_fmt", "yuv420p", "-preset", "slow", "-profile:v", "high",
              "-movflags", "+faststart", "-tag:v", "avc1"]
    acodec = []
    if audio_wav:
        acodec = ["-c:a", "aac", "-b:a", f"{args.audio_kbps}k"]
        if args.speed != 1:
            acodec = ["-filter:a", atempo_chain(args.speed)] + acodec
    if args.target_mb:
        total_kbps = args.target_mb * 8 * 1000 * 0.92 / duration       # MB = 10^6 bytes; 8 % margin (rate control, container)
        v_kbps = max(100, int(total_kbps - (args.audio_kbps if audio_wav else 0)))
        passlog = str(work / "x264pass")
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", str(video)] + vcodec +
                       ["-b:v", f"{v_kbps}k", "-pass", "1", "-passlogfile", passlog, "-an", "-f", "mp4",
                        "/dev/null"], check=True)
        subprocess.run(cmd + vcodec + ["-b:v", f"{v_kbps}k", "-pass", "2", "-passlogfile", passlog] + acodec +
                       ["-shortest", str(out)], check=True)
    else:
        subprocess.run(cmd + vcodec + ["-crf", str(args.crf)] + acodec + ["-shortest", str(out)], check=True)
    if not args.keep_work:
        shutil.rmtree(work)
    probe = ffprobe(out)
    result = {"output": str(out), "frames": len(picked) + summary_frames, "fps": round(fps, 4),
              "capture_stride": stride, "ticks_per_output_frame": m * stride,
              "effective_speed": round(m * stride * fps / emrec.TICK_HZ, 4), "aligned_entries": len(entries),
              "duration_s": round(duration, 3), "audio": audio, "notes": notes, "ffprobe": probe,
              "drift": {k: d[k] for k in ("ticks_compared", "position_heading_bit_exact", "first_drift")}}
    (out.parent / (out.stem + "_summary.json")).write_text(json.dumps(result, indent=1) + "\n")
    return result


def summary_card(r: Renderer, d: dict, args) -> bytes:
    from PIL import Image, ImageDraw
    im = Image.new("RGB", (r.W, r.H), (16, 16, 18))
    dr = ImageDraw.Draw(im)
    s = r.side_h / 480
    lines = [args.title or "Extermination (PS2) \u2014 original vs native port",
             f"same recorded inputs on both sides, synced by game tick; {args.speed:g}\u00d7 speed",
             f"{d['ticks_compared']} play/cutscene ticks compared: player position and heading "
             f"bit-exact on {d['position_heading_bit_exact']}"]
    f = d["first_drift"]
    lines.append("no position/heading drift beyond the tolerance" if not f else
                 f"first drift: segment {f['seg']} offset {f['off']} ({f['distance']:.3f} units)")
    y = r.H // 2 - int(60 * s)
    for i, text in enumerate(lines):
        dr.text((r.W // 2, y), text, font=r.f_title if i == 0 else r.f_sub, fill=(235, 235, 235), anchor="mm")
        y += int(36 * s)
    return im.tobytes()


def main(argv=None) -> int:
    import argparse
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--native", required=True)
    ap.add_argument("--ps2", required=True)
    ap.add_argument("--out", required=True)
    add_compose_args(ap)
    res = compose(ap.parse_args(argv))
    print(json.dumps(res, indent=1))
    return 0


def add_compose_args(ap) -> None:
    ap.add_argument("--speed", type=float, default=2.0, help="playback speed (frames are dropped, not blended)")
    ap.add_argument("--fps", type=float, default=30.0, help="target output frame rate")
    ap.add_argument("--stride", type=int, default=4, help=argparse_suppress())
    ap.add_argument("--loads", choices=["trim", "hold"], default="trim")
    ap.add_argument("--audio", choices=["original", "native", "both", "none"], default="none",
                    help="none (default), native = the port's sound only (labelled), original/both need a PS2 "
                         "audio.wav (the fork pass writes one: ps2.py --emulator fork --audio)")
    ap.add_argument("--audio-kbps", type=int, default=128)
    ap.add_argument("--title", default=None)
    ap.add_argument("--subtitle", default=None)
    ap.add_argument("--height", type=int, default=480, help="height of each side (4:3)")
    ap.add_argument("--scale-filter", choices=["nearest", "bilinear"], default="nearest")
    ap.add_argument("--info", action="store_true", help="footer with segment and counters")
    ap.add_argument("--summary-seconds", type=float, default=0.0, help="end card with the drift summary")
    ap.add_argument("--target-mb", type=float, default=None, help="two-pass encode to this size (10^6 bytes)")
    ap.add_argument("--crf", type=int, default=20)
    ap.add_argument("--tol-pos", type=float, default=0.01)
    ap.add_argument("--tol-yaw", type=float, default=0.001)
    ap.add_argument("--keep-work", action="store_true")


def argparse_suppress():
    import argparse
    return argparse.SUPPRESS


if __name__ == "__main__":
    sys.exit(main())
