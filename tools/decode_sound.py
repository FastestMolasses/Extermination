#!/usr/bin/env python3
"""Extermination (SCUS-97112) SShd sound-bank decoder.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Decodes the SShd sound-bank container into WAV files. The container is the
structure produced by extract_data.py for sound regions:

  +0x00  u32 totalsize    body_offset + body_size
  +0x04  u32 hdrlen       = 0x20 + 0x10*count
  +0x08  u32 0
  +0x0C  u32 count
  +0x10  count records of 0x10 bytes, then a 0x10-byte trailer
  +hdrlen      u32 c, u32 body_offset, u32 0, "SShd"
  +hdrlen+0xC  "SShd" magic, then the voice table
  body_offset  PS2 VAG ADPCM payload, body_size = totalsize - body_offset

body_offset is relative to the start of the region (the concatenation of the
region's extracted files, of which the SShd container is the first).

The PS2 ADPCM sample rate is not yet located in the header; it defaults to
22050 Hz and is overridable with --rate. If a clip plays too fast/slow the
rate is wrong, not the decode.

Usage:
  decode_sound.py batch  --in extract --out wav
  decode_sound.py decode extract/chunk50 --out wav --rate 22050
"""
from __future__ import annotations

import argparse
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


def decode_vag(body: bytes) -> tuple[bytes, int]:
    """Decode PS2 VAG ADPCM to little-endian mono PCM16. Returns (pcm, anomalies)."""
    hist1 = hist2 = 0
    out = bytearray()
    anomalies = 0
    for fpos in range(0, len(body) - 15, 16):
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


def write_wav(path: Path, pcm: bytes, rate: int) -> None:
    with wave.open(str(path), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(pcm)


def decode_region(region_dir: Path, out_dir: Path, rate: int) -> str | None:
    data = region_bytes(region_dir)
    bank = parse_bank(data)
    if bank is None:
        return None
    body_offset, body_size = bank
    body = data[body_offset:body_offset + body_size]
    pcm, anomalies = decode_vag(body)
    out_dir.mkdir(parents=True, exist_ok=True)
    wav = out_dir / f"{region_dir.name}.wav"
    write_wav(wav, pcm, rate)
    secs = len(pcm) // 2 / rate
    note = f"  ({anomalies} bad frames)" if anomalies else ""
    return f"{wav.name}: body {body_size:#x}, {len(pcm) // 2} samples, {secs:.2f}s @ {rate}Hz{note}"


def cmd_decode(args) -> int:
    out = Path(args.out)
    line = decode_region(Path(args.region), out, args.rate)
    if line is None:
        print(f"{args.region}: not an SShd sound bank")
        return 1
    print(line)
    return 0


def cmd_batch(args) -> int:
    root = Path(args.input)
    out = Path(args.out)
    regions = sorted(p for p in root.iterdir() if p.is_dir())
    decoded = 0
    for region_dir in regions:
        line = decode_region(region_dir, out, args.rate)
        if line:
            print(line)
            decoded += 1
    print(f"\ndecoded {decoded} sound banks to {out}/")
    return 0 if decoded else 1


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination SShd sound-bank decoder")
    sub = p.add_subparsers(dest="cmd", required=True)

    sp = sub.add_parser("decode", help="decode one region directory")
    sp.add_argument("region", help="path to an extracted region dir, e.g. extract/chunk50")
    sp.add_argument("--out", default="wav", help="output directory (default: wav/)")
    sp.add_argument("--rate", type=int, default=22050, help="sample rate (default: 22050)")
    sp.set_defaults(func=cmd_decode)

    sp = sub.add_parser("batch", help="decode every sound bank under a directory")
    sp.add_argument("--in", dest="input", default="extract", help="extraction directory")
    sp.add_argument("--out", default="wav", help="output directory (default: wav/)")
    sp.add_argument("--rate", type=int, default=22050, help="sample rate (default: 22050)")
    sp.set_defaults(func=cmd_batch)

    args = p.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
