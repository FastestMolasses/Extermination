#!/usr/bin/env python3
"""Extermination (SCUS-97112) SShd sound-bank decoder.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Decodes the SShd sound-bank container into individual WAV files.

Container layout (produced by extract_data.py for sound regions):
  +0x00  u32 totalsize    body_offset + body_size
  +0x04  u32 hdrlen
  +0x0C  u32 count
  +hdrlen+4   u32 body_offset   (relative to the region)
  +hdrlen+0xC "SShd" magic, then the voice table
  body        PS2 VAG ADPCM, body_size = totalsize - body_offset

A bank's body holds many sounds concatenated. Each 16-byte VAG frame carries
a flag byte; a frame whose flag has bit 0 set ends a block. Real sounds are
the multi-frame runs; the 1-frame flag-7 blocks between them are terminators.
A sound "loops" if any of its frames has the loop bit (0x02) set.

Many banks reuse a shared SFX set, so `batch` deduplicates by ADPCM content
across all banks and writes each unique sound once, with a manifest mapping
banks to the sounds they use.

The sample rate is not yet located in the header; it defaults to 22050 Hz
(overridable with --rate). Wrong rate => wrong pitch, not a wrong decode.

VOICE.DAT and MUSIC.DAT are different: each is one raw VAG stream with no
SShd container and no VAG end-flags. The `stream` subcommand splits them
into clips on runs of digitally-silent frames. VOICE.DAT is mono; MUSIC.DAT
is interleaved stereo (64-frame / 1024-byte blocks: L, R, L, R, ...) so it
needs --interleave 64.

Usage:
  decode_sound.py batch  --in extract --out wav
  decode_sound.py decode extract/chunk22 --out wav
  decode_sound.py stream STREAM/VOICE.DAT --out voice
  decode_sound.py stream STREAM/MUSIC.DAT --out music --interleave 64
"""
from __future__ import annotations

import argparse
import hashlib
import struct
import sys
import wave
from pathlib import Path

# Standard PS2 / SPU ADPCM predictor coefficients (numerator over 64).
VAG_COEFS = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def region_bytes(region_dir: Path) -> bytes:
    """Concatenate a region's extracted files (f00, f01, ...) in order."""
    parts = sorted(region_dir.glob("f*_id*.bin"), key=lambda p: int(p.name[1:3]))
    return b"".join(p.read_bytes() for p in parts)


def parse_bank(data: bytes):
    """Return (body_offset, body_size) if data is an SShd bank, else None."""
    if len(data) < 0x20:
        return None
    totalsize, hdrlen = struct.unpack_from("<2I", data, 0)
    if not (0x20 <= hdrlen <= 0x400) or hdrlen + 0x10 > len(data):
        return None
    if data[hdrlen + 0xC:hdrlen + 0x10] != b"SShd":
        return None
    body_offset = struct.unpack_from("<I", data, hdrlen + 4)[0]
    body_size = totalsize - body_offset
    if body_size <= 0 or body_offset + body_size > len(data):
        return None
    return body_offset, body_size


def split_sounds(body: bytes) -> list[tuple[int, int]]:
    """Split a bank body into (start_frame, frame_count) per sound.

    A VAG frame whose flag (byte 1) has bit 0 set ends a block. Multi-frame
    runs are real sounds; lone 1-frame blocks are terminators and are dropped.
    """
    nframes = len(body) // 16
    sounds = []
    start = 0
    for i in range(nframes):
        if body[i * 16 + 1] & 1:
            if i - start + 1 > 1:
                sounds.append((start, i - start + 1))
            start = i + 1
    if nframes - start > 1:
        sounds.append((start, nframes - start))
    return sounds


def decode_vag(body: bytes, start_frame: int, n_frames: int) -> tuple[bytes, int]:
    """Decode one VAG sound to little-endian mono PCM16. Returns (pcm, anomalies).

    ADPCM history is local to this call, so each sound decodes independently.
    """
    hist1 = hist2 = 0
    out = bytearray()
    anomalies = 0
    for f in range(start_frame, start_frame + n_frames):
        fpos = f * 16
        b0 = body[fpos]
        shift = b0 & 0xF
        pred = (b0 >> 4) & 0xF
        if pred > 4:
            pred, anomalies = 0, anomalies + 1
        if shift > 12:
            shift = 12
        c1, c2 = VAG_COEFS[pred]
        for i in range(28):
            byte = body[fpos + 2 + (i >> 1)]
            nib = (byte & 0xF) if (i & 1) == 0 else (byte >> 4)
            s = nib - 16 if nib > 7 else nib
            sample = (s << (12 - shift)) + ((hist1 * c1 + hist2 * c2) >> 6)
            sample = max(-32768, min(32767, sample))
            hist2, hist1 = hist1, sample
            out += struct.pack("<h", sample)
    return bytes(out), anomalies


def sound_loops(body: bytes, start_frame: int, n_frames: int) -> bool:
    """True if any frame in the sound has the VAG loop bit (0x02) set."""
    return any(body[(start_frame + k) * 16 + 1] & 2 for k in range(n_frames))


ZERO_DATA = b"\x00" * 14  # the 14 data bytes of a digitally-silent VAG frame


def find_voice_clips(data: bytes, gap_frames: int) -> list[tuple[int, int]]:
    """Split a raw VAG stream (VOICE.DAT) into clips on silence gaps.

    VOICE.DAT carries no VAG end-flags, so clips are delimited instead by
    runs of digitally-silent frames. A run of >= gap_frames silent frames
    ends a clip; shorter silent runs are kept as in-clip pauses.
    """
    nframes = len(data) // 16
    silent = [data[i * 16 + 2:i * 16 + 16] == ZERO_DATA for i in range(nframes)]
    clips = []
    i = 0
    while i < nframes:
        if silent[i]:
            i += 1
            continue
        start = i
        last_voiced = i
        run = 0
        while i < nframes and run < gap_frames:
            if silent[i]:
                run += 1
            else:
                run = 0
                last_voiced = i
            i += 1
        clips.append((start, last_voiced + 1))
    return clips


def deinterleave(data: bytes, block_frames: int) -> tuple[bytes, bytes]:
    """Split an interleaved stereo VAG stream into (left, right) byte streams.

    The stream alternates fixed-size blocks: L, R, L, R, ... MUSIC.DAT uses
    64-frame (1024-byte) blocks.
    """
    blk = block_frames * 16
    left = bytearray()
    right = bytearray()
    for n, pos in enumerate(range(0, len(data), blk)):
        (left if n % 2 == 0 else right).extend(data[pos:pos + blk])
    return bytes(left), bytes(right)


def interleave_lr(lpcm: bytes, rpcm: bytes) -> bytes:
    """Interleave two mono PCM16 byte streams into stereo PCM16."""
    n = min(len(lpcm), len(rpcm))
    n -= n % 2
    out = bytearray(2 * n)
    out[0::4] = lpcm[0:n:2]
    out[1::4] = lpcm[1:n:2]
    out[2::4] = rpcm[0:n:2]
    out[3::4] = rpcm[1:n:2]
    return bytes(out)


def write_wav(path: Path, pcm: bytes, rate: int, channels: int = 1) -> None:
    with wave.open(str(path), "wb") as w:
        w.setnchannels(channels)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(pcm)


def loop_pad(pcm: bytes, rate: int, target: float = 1.0) -> bytes:
    """Repeat a short looping sound so it is long enough to audition."""
    dur = len(pcm) / 2 / rate
    if 0 < dur < 0.5:
        return pcm * (int(target / dur) + 1)
    return pcm


def iter_banks(root: Path):
    """Yield (region_dir, body) for every region that is an SShd bank."""
    for region_dir in sorted(p for p in root.iterdir() if p.is_dir()):
        data = region_bytes(region_dir)
        bank = parse_bank(data)
        if bank:
            body_offset, body_size = bank
            yield region_dir, data[body_offset:body_offset + body_size]


def cmd_decode(args) -> int:
    region_dir = Path(args.region)
    data = region_bytes(region_dir)
    bank = parse_bank(data)
    if bank is None:
        print(f"{region_dir}: not an SShd sound bank")
        return 1
    body_offset, body_size = bank
    body = data[body_offset:body_offset + body_size]
    out = Path(args.out) / region_dir.name
    out.mkdir(parents=True, exist_ok=True)
    sounds = split_sounds(body)
    for idx, (sf, nf) in enumerate(sounds):
        pcm, _ = decode_vag(body, sf, nf)
        if args.loop_pad and sound_loops(body, sf, nf):
            pcm = loop_pad(pcm, args.rate)
        write_wav(out / f"s{idx:02d}.wav", pcm, args.rate)
    print(f"{region_dir.name}: {len(sounds)} sounds -> {out}/")
    return 0


def cmd_batch(args) -> int:
    root = Path(args.input)
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)

    uniq: dict[bytes, int] = {}        # adpcm hash -> sound id
    pcms: dict[int, bytes] = {}        # sound id -> pcm
    loops: set[int] = set()
    used_by: dict[int, list[str]] = {}
    bank_sounds: list[tuple[str, list[int]]] = []
    anomalies = 0

    for region_dir, body in iter_banks(root):
        ids = []
        for sf, nf in split_sounds(body):
            raw = body[sf * 16:(sf + nf) * 16]
            h = hashlib.sha1(raw).digest()
            sid = uniq.get(h)
            if sid is None:
                sid = len(uniq)
                uniq[h] = sid
                pcm, anom = decode_vag(body, sf, nf)
                anomalies += anom
                pcms[sid] = pcm
                if sound_loops(body, sf, nf):
                    loops.add(sid)
                used_by[sid] = []
            ids.append(sid)
            if region_dir.name not in used_by[sid]:
                used_by[sid].append(region_dir.name)
        bank_sounds.append((region_dir.name, ids))

    out_dur: dict[int, float] = {}
    for sid, pcm in pcms.items():
        out_pcm = loop_pad(pcm, args.rate) if (args.loop_pad and sid in loops) else pcm
        write_wav(out / f"snd_{sid:04d}.wav", out_pcm, args.rate)
        out_dur[sid] = len(out_pcm) / 2 / args.rate

    lines = ["# Extermination sound banks -> unique sounds", ""]
    for name, ids in bank_sounds:
        lines.append(f"{name}: " + " ".join(f"snd_{s:04d}" for s in ids))
    lines += ["", "# unique sounds (duration is the written WAV; loop sounds may be repeated)"]
    for sid in sorted(pcms):
        dur = out_dur[sid]
        tag = "loop" if sid in loops else "oneshot"
        lines.append(f"snd_{sid:04d}  {dur:7.3f}s  {tag:7}  "
                     f"used by {len(used_by[sid])}: {' '.join(used_by[sid])}")
    (out / "manifest.txt").write_text("\n".join(lines) + "\n")

    total = sum(len(ids) for _, ids in bank_sounds)
    print(f"{len(bank_sounds)} banks, {total} sound references, "
          f"{len(pcms)} unique sounds -> {out}/")
    if anomalies:
        print(f"warning: {anomalies} frames had an out-of-range predictor")
    print(f"manifest: {out / 'manifest.txt'}")
    return 0


def cmd_stream(args) -> int:
    data = Path(args.file).read_bytes()
    if len(data) % 16:
        print(f"warning: {len(data) % 16} trailing bytes are not a whole VAG frame")
    if args.interleave:
        left, right = deinterleave(data, args.interleave)
        channels = 2
    else:
        left, right, channels = data, b"", 1
    clips = find_voice_clips(left, args.gap)
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    written = 0
    total_frames = 0
    anomalies = 0
    for sf, ef in clips:
        if ef - sf < args.min_frames:
            continue
        lpcm, anom = decode_vag(left, sf, ef - sf)
        anomalies += anom
        if channels == 2:
            rpcm, anom2 = decode_vag(right, sf, ef - sf)
            anomalies += anom2
            pcm = interleave_lr(lpcm, rpcm)
            total_frames += len(pcm) // 4
        else:
            pcm = lpcm
            total_frames += len(pcm) // 2
        write_wav(out / f"clip_{written:04d}.wav", pcm, args.rate, channels)
        written += 1
    kind = "stereo" if channels == 2 else "mono"
    print(f"{len(clips)} clips found, {written} written to {out}/ "
          f"({total_frames / args.rate:.0f}s total @ {args.rate}Hz, gap={args.gap}, {kind})")
    if anomalies:
        print(f"warning: {anomalies} frames had an out-of-range predictor")
    return 0


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination SShd sound-bank decoder")
    sub = p.add_subparsers(dest="cmd", required=True)

    def add_common(sp):
        sp.add_argument("--out", default="wav", help="output directory (default: wav/)")
        sp.add_argument("--rate", type=int, default=22050, help="sample rate (default: 22050)")
        sp.add_argument("--no-loop-pad", dest="loop_pad", action="store_false",
                        help="do not repeat short looping sounds")

    sp = sub.add_parser("decode", help="decode one region into per-sound WAVs")
    sp.add_argument("region", help="path to an extracted region dir, e.g. extract/chunk22")
    add_common(sp)
    sp.set_defaults(func=cmd_decode)

    sp = sub.add_parser("batch", help="decode all banks, deduplicated, with a manifest")
    sp.add_argument("--in", dest="input", default="extract", help="extraction directory")
    add_common(sp)
    sp.set_defaults(func=cmd_batch)

    sp = sub.add_parser("stream", help="decode a raw VAG stream (VOICE.DAT/MUSIC.DAT) into clips")
    sp.add_argument("file", help="path to the raw VAG stream (VOICE.DAT or MUSIC.DAT)")
    sp.add_argument("--out", default="stream", help="output directory (default: stream/)")
    # 48000 Hz: clip_0000 (End Credits) matches an official-soundtrack rip
    # (also 48000 Hz) in duration only at this rate. The rate is not stored in
    # the audio data; treat as final pending confirmation from the decomp.
    sp.add_argument("--rate", type=int, default=48000, help="sample rate (default: 48000)")
    sp.add_argument("--gap", type=int, default=64,
                    help="silent frames that delimit a clip (default: 64)")
    sp.add_argument("--min-frames", type=int, default=8,
                    help="drop clips shorter than this many frames (default: 8)")
    sp.add_argument("--interleave", type=int, default=0,
                    help="stereo deinterleave block in frames (0=mono; MUSIC.DAT uses 64)")
    sp.set_defaults(func=cmd_stream)

    args = p.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
