#!/usr/bin/env python3
"""Extermination (SCUS-97112) audio exporter — SPU2 ADPCM ("VAG") to WAV.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing. All output goes
under the git-ignored extract/audio_decoded/ by default.

This is the consolidated audio pillar tool (supersedes ad-hoc use of
tools/decode_sound.py for export jobs). It covers all three audio sources:

  STREAM/MUSIC.DAT   one raw interleaved-stereo VAG stream, no container.
                     Alternating fixed-size L/R blocks; the block size is
                     64 frames = 1024 bytes (empirically verified, see
                     `detect-interleave` below). Tracks are delimited by
                     runs of digitally-silent frames -> 55 stereo tracks.
  STREAM/VOICE.DAT   one raw mono VAG stream, same silence-delimited split
                     -> 116 mono clips.
  SShd sound banks   inside DATA.DAT regions (extract/<chunk>/f*_id*.bin).
                     Container: u32 totalsize, u32 hdrlen, ..., u32 count;
                     at +hdrlen+4 u32 body_offset, at +hdrlen+0xC "SShd".
                     The body is concatenated VAG sounds; a frame whose
                     flag byte has bit 0 set ends a block, 1-frame blocks
                     are terminators. Banks heavily share content, so the
                     `sfx` command deduplicates by ADPCM bytes and writes
                     each unique sound once with a manifest.

SPU2 ADPCM format (the "VAG" block codec): 16-byte frames =
  byte 0   = (predictor << 4) | shift    predictor 0..4, shift 0..12
  byte 1   = flags (bit 0 = end of block, bit 1 = loop-region marker,
             bit 2 = loop start; 0x03/0x07 are the common end/loop ends)
  bytes 2..15 = 28 4-bit nibbles, low nibble first
Decode: s = (nibble sign-extended) << (12 - shift)
            + (hist1*c1 + hist2*c2) >> 6,  with the standard 5 coefficient
pairs (c1,c2) in 1/64 units: (0,0) (60,0) (115,-52) (98,-55) (122,-60).

Sample rates: NOT stored anywhere in the data (SPU2 pitch is a runtime
voice parameter). Streams play at 48000 Hz (evidence: clip_0000 "End
Credits" duration-matches an official soundtrack rip; voice formants are
natural at 48000). SFX banks have no single rate — each voice is repitched
at trigger time; 22050 is the extraction default (see docs/FINDINGS.md
"Audio" for the empirical analysis).

Usage (run from the repo root; ISO mounted or STREAM files copied locally):
  audio_export.py music  /Volumes/<disc>/STREAM/MUSIC.DAT
  audio_export.py voice  /Volumes/<disc>/STREAM/VOICE.DAT
  audio_export.py sfx    --in extract
  audio_export.py detect-interleave /Volumes/<disc>/STREAM/MUSIC.DAT
"""
from __future__ import annotations

import argparse
import hashlib
import struct
import sys
import wave
from pathlib import Path

# Standard SPU/SPU2 ADPCM predictor coefficients (numerators over 64).
COEFS = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]

FRAME = 16                      # bytes per ADPCM frame
SAMPLES_PER_FRAME = 28
ZERO_DATA = b"\x00" * 14        # data bytes of a digitally-silent frame

DEFAULT_OUT = Path("extract/audio_decoded")
STREAM_RATE = 48000             # streams: evidence-based (FINDINGS "Audio")
SFX_RATE = 22050                # banks: extraction default, no stored rate


# ----------------------------------------------------------------- decode

def decode_adpcm(data: bytes, start_frame: int = 0,
                 n_frames: int | None = None) -> bytes:
    """Decode SPU2 ADPCM frames to little-endian mono PCM16 bytes."""
    if n_frames is None:
        n_frames = len(data) // FRAME - start_frame
    hist1 = hist2 = 0
    out = bytearray(n_frames * SAMPLES_PER_FRAME * 2)
    w = 0
    for f in range(start_frame, start_frame + n_frames):
        p = f * FRAME
        b0 = data[p]
        shift = b0 & 0xF
        pred = b0 >> 4
        if pred > 4:            # out-of-range predictor: treat as flat
            pred = 0
        if shift > 12:
            shift = 12
        c1, c2 = COEFS[pred]
        for i in range(SAMPLES_PER_FRAME):
            byte = data[p + 2 + (i >> 1)]
            nib = byte & 0xF if (i & 1) == 0 else byte >> 4
            if nib > 7:
                nib -= 16
            s = (nib << (12 - shift)) + ((hist1 * c1 + hist2 * c2) >> 6)
            s = -32768 if s < -32768 else (32767 if s > 32767 else s)
            hist2, hist1 = hist1, s
            struct.pack_into("<h", out, w, s)
            w += 2
    return bytes(out)


def write_wav(path: Path, pcm: bytes, rate: int, channels: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as wv:
        wv.setnchannels(channels)
        wv.setsampwidth(2)
        wv.setframerate(rate)
        wv.writeframes(pcm)


def interleave_pcm(lpcm: bytes, rpcm: bytes) -> bytes:
    """Two mono PCM16 byte streams -> one interleaved stereo PCM16 stream."""
    n = min(len(lpcm), len(rpcm))
    n -= n % 2
    out = bytearray(2 * n)
    out[0::4] = lpcm[0:n:2]
    out[1::4] = lpcm[1:n:2]
    out[2::4] = rpcm[0:n:2]
    out[3::4] = rpcm[1:n:2]
    return bytes(out)


# ----------------------------------------------------------------- streams

def deinterleave(data: bytes, block_frames: int) -> tuple[bytes, bytes]:
    """Split alternating fixed-size L/R blocks into two channel streams."""
    blk = block_frames * FRAME
    left, right = bytearray(), bytearray()
    for n, pos in enumerate(range(0, len(data), blk)):
        (left if n % 2 == 0 else right).extend(data[pos:pos + blk])
    return bytes(left), bytes(right)


def split_on_silence(data: bytes, gap_frames: int) -> list[tuple[int, int]]:
    """Split a raw VAG stream into (start_frame, end_frame) clips.

    The streams carry no end-flags; clips are delimited by runs of
    >= gap_frames digitally-silent frames (shorter runs stay in-clip).
    """
    nframes = len(data) // FRAME
    clips = []
    i = 0
    while i < nframes:
        if data[i * FRAME + 2:i * FRAME + FRAME] == ZERO_DATA:
            i += 1
            continue
        start = last = i
        run = 0
        while i < nframes and run < gap_frames:
            if data[i * FRAME + 2:i * FRAME + FRAME] == ZERO_DATA:
                run += 1
            else:
                run, last = 0, i
            i += 1
        clips.append((start, last + 1))
    return clips


def boundary_discontinuity(data: bytes, block_frames: int,
                           n_blocks: int = 400) -> tuple[float, float]:
    """Empirical interleave metric: ADPCM continuity at block boundaries.

    Deinterleave at the candidate size, decode the head of the left
    channel, and compare the mean |sample step| across block boundaries
    with the mean |sample step| inside blocks. A wrong candidate splices
    unrelated audio (other channel / other time) at every boundary, so
    the boundary step is far larger than the in-block step; the correct
    candidate makes them comparable. Returns (boundary_mean, inblock_mean).
    """
    left, _ = deinterleave(data[:block_frames * FRAME * 2 * n_blocks],
                           block_frames)
    pcm = decode_adpcm(left)
    samples = struct.unpack("<%dh" % (len(pcm) // 2), pcm)
    spb = block_frames * SAMPLES_PER_FRAME      # samples per block
    bnd, bnd_n, inb, inb_n = 0, 0, 0, 0
    for i in range(1, len(samples)):
        d = abs(samples[i] - samples[i - 1])
        if i % spb == 0:
            bnd, bnd_n = bnd + d, bnd_n + 1
        else:
            inb, inb_n = inb + d, inb_n + 1
    return (bnd / max(bnd_n, 1), inb / max(inb_n, 1))


def cmd_detect_interleave(args) -> int:
    data = Path(args.file).read_bytes()[:args.bytes]
    print(f"candidate  boundary-step  in-block-step  ratio")
    best, best_ratio = None, None
    for cand in (16, 32, 64, 128, 256, 512):
        b, i = boundary_discontinuity(data, cand)
        ratio = b / max(i, 1e-9)
        print(f"{cand:6d}     {b:10.1f}     {i:10.1f}   {ratio:6.2f}")
        if best_ratio is None or ratio < best_ratio:
            best, best_ratio = cand, ratio
    print(f"-> interleave = {best} frames ({best * FRAME} bytes/block); "
          f"correct size has ratio ~1 (continuous audio across boundaries)")
    return 0


def cmd_stream(args, stereo: bool) -> int:
    data = Path(args.file).read_bytes()
    if len(data) % FRAME:
        print(f"warning: {len(data) % FRAME} trailing non-frame bytes ignored")
    out = Path(args.out) / ("music" if stereo else "voice")
    if stereo:
        left, right = deinterleave(data, args.interleave)
    else:
        left, right = data, b""
    clips = split_on_silence(left, args.gap)
    written = total_samples = 0
    for sf, ef in clips:
        nf = ef - sf
        if nf < args.min_frames:
            continue
        lpcm = decode_adpcm(left, sf, nf)
        if stereo:
            pcm = interleave_pcm(lpcm, decode_adpcm(right, sf, nf))
            total_samples += len(pcm) // 4
        else:
            pcm = lpcm
            total_samples += len(pcm) // 2
        write_wav(out / f"track_{written:04d}.wav", pcm, args.rate,
                  2 if stereo else 1)
        written += 1
    print(f"{written} {'stereo' if stereo else 'mono'} tracks -> {out}/ "
          f"({total_samples / args.rate:.0f}s @ {args.rate} Hz)")
    return 0


# ------------------------------------------------------------------- banks

def region_bytes(region_dir: Path) -> bytes:
    """Concatenate a region's extracted files (f00, f01, ...) in order."""
    parts = sorted(region_dir.glob("f*_id*.bin"), key=lambda p: int(p.name[1:3]))
    return b"".join(p.read_bytes() for p in parts)


def parse_bank(data: bytes) -> tuple[int, int] | None:
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


def split_bank_sounds(body: bytes) -> list[tuple[int, int]]:
    """(start_frame, n_frames) per sound; 1-frame end-marker blocks dropped."""
    nframes = len(body) // FRAME
    sounds, start = [], 0
    for i in range(nframes):
        if body[i * FRAME + 1] & 1:             # end-of-block flag
            if i - start + 1 > 1:
                sounds.append((start, i - start + 1))
            start = i + 1
    if nframes - start > 1:
        sounds.append((start, nframes - start))
    return sounds


def cmd_sfx(args) -> int:
    root = Path(args.input)
    out = Path(args.out) / "sfx"
    out.mkdir(parents=True, exist_ok=True)
    uniq: dict[bytes, int] = {}
    manifest = ["# Extermination SShd banks -> unique SFX "
                f"(decoded at {args.rate} Hz; no rate is stored in the bank)",
                ""]
    n_banks = refs = 0
    for region_dir in sorted(p for p in root.iterdir() if p.is_dir()):
        data = region_bytes(region_dir)
        bank = parse_bank(data)
        if not bank:
            continue
        n_banks += 1
        body = data[bank[0]:bank[0] + bank[1]]
        ids = []
        for sf, nf in split_bank_sounds(body):
            raw = body[sf * FRAME:(sf + nf) * FRAME]
            sid = uniq.get(hashlib.sha1(raw).digest())
            if sid is None:
                sid = len(uniq)
                uniq[hashlib.sha1(raw).digest()] = sid
                loops = any(raw[k * FRAME + 1] & 2 for k in range(nf))
                pcm = decode_adpcm(body, sf, nf)
                write_wav(out / f"snd_{sid:04d}.wav", pcm, args.rate, 1)
                manifest.append(
                    f"snd_{sid:04d}  {len(pcm) / 2 / args.rate:7.3f}s  "
                    f"{'loop' if loops else 'oneshot'}")
            ids.append(sid)
            refs += 1
        manifest.append(f"{region_dir.name}: "
                        + " ".join(f"snd_{s:04d}" for s in ids))
    (out / "manifest.txt").write_text("\n".join(manifest) + "\n")
    print(f"{n_banks} banks, {refs} sound references, "
          f"{len(uniq)} unique sounds -> {out}/")
    return 0


# -------------------------------------------------------------------- main

def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(
        description="Extermination audio exporter (SPU2 ADPCM -> WAV)")
    sub = p.add_subparsers(dest="cmd", required=True)

    sp = sub.add_parser("music", help="decode MUSIC.DAT (interleaved stereo)")
    sp.add_argument("file", help="path to STREAM/MUSIC.DAT")
    sp.add_argument("--out", default=str(DEFAULT_OUT))
    sp.add_argument("--rate", type=int, default=STREAM_RATE)
    sp.add_argument("--interleave", type=int, default=64,
                    help="L/R block size in frames (default 64, verified)")
    sp.add_argument("--gap", type=int, default=64,
                    help="silent frames that delimit a track")
    sp.add_argument("--min-frames", type=int, default=8)
    sp.set_defaults(func=lambda a: cmd_stream(a, stereo=True))

    sp = sub.add_parser("voice", help="decode VOICE.DAT (mono)")
    sp.add_argument("file", help="path to STREAM/VOICE.DAT")
    sp.add_argument("--out", default=str(DEFAULT_OUT))
    sp.add_argument("--rate", type=int, default=STREAM_RATE)
    sp.add_argument("--gap", type=int, default=64)
    sp.add_argument("--min-frames", type=int, default=8)
    sp.set_defaults(func=lambda a: cmd_stream(a, stereo=False))

    sp = sub.add_parser("sfx", help="decode all SShd banks (deduplicated)")
    sp.add_argument("--in", dest="input", default="extract",
                    help="extract_data.py output directory")
    sp.add_argument("--out", default=str(DEFAULT_OUT))
    sp.add_argument("--rate", type=int, default=SFX_RATE)
    sp.set_defaults(func=cmd_sfx)

    sp = sub.add_parser("detect-interleave",
                        help="empirically determine a stream's L/R block size")
    sp.add_argument("file", help="path to the stream (e.g. MUSIC.DAT)")
    sp.add_argument("--bytes", type=int, default=8 << 20,
                    help="how much of the stream to analyze (default 8 MiB)")
    sp.set_defaults(func=cmd_detect_interleave)

    args = p.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
