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
                     `detect-interleave` below). Authoritative track
                     boundaries live in the engine's MUSIC CUE TABLE in the
                     boot ELF (vram 0x25DD30, 68 x 16-byte entries; see
                     docs/FINDINGS.md "Music cue table"): pass --elf to
                     split on it and name tracks cue_NNN.wav (67 cues).
                     Without --elf, falls back to the older silence split
                     (-> 55 tracks; adjacent cues separated by < gap
                     silent frames get merged).
  STREAM/VOICE.DAT   one raw mono VAG stream. Same deal: the VOICE CUE
                     TABLE at vram 0x25E170 (179 entries) gives 178 exact
                     clips with --elf; silence split -> 116 merged clips.
  SShd sound banks   inside DATA.DAT regions (extract/<chunk>/f*_id*.bin).
                     One *container* holds 1..4 banks (decompiled from the
                     boot ELF's loader/trigger path, 2026-06-10 — see
                     docs/FINDINGS.md "SShd bank format"):
                       +0x00 u32 total size
                       +0x04 u32 offset of first bank header (== 0x?50)
                       +0x08 u32 0
                       +0x0C u32 bank count N
                       +0x10 {u32 img_off, u32 img_size, u32 img_off, 0}
                       +0x20 N rows {u32 body_size, u32 hdr_off, u32 type, 0}
                     The sample image (all banks' concatenated VAG bodies)
                     spans [img_off, img_off+img_size); bank i's body starts
                     at img_off + sum(body_size of rows before it).
                     Each bank header ("SShd" magic at hdr+0xC) holds region
                     offsets; its program records contain 16-byte TONE
                     RECORDS: {note_lo, note_hi, center, s8 fine, u16
                     sample_off>>3, u16 adsr1, u16 adsr2, vol?, pan?, ?,
                     bend_range, ?, flags}.  sample_off<<3 is relative to
                     the bank's SPU upload base == its body start.
                     The per-tone playback rate is exact (see tone_rate()).

SPU2 ADPCM format (the "VAG" block codec): 16-byte frames =
  byte 0   = (predictor << 4) | shift    predictor 0..4, shift 0..12
  byte 1   = flags (bit 0 = end of block, bit 1 = loop-region marker,
             bit 2 = loop start; 0x03/0x07 are the common end/loop ends)
  bytes 2..15 = 28 4-bit nibbles, low nibble first
Decode: s = (nibble sign-extended) << (12 - shift)
            + (hist1*c1 + hist2*c2) >> 6,  with the standard 5 coefficient
pairs (c1,c2) in 1/64 units: (0,0) (60,0) (115,-52) (98,-55) (122,-60).

Sample rates: streams play at 48000 Hz (evidence: clip_0000 "End Credits"
duration-matches an official soundtrack rip; voice formants are natural at
48000).  SFX rates are now EXACT, derived from the engine's voice-trigger
path (func_00115E50/func_00115850 -> func_00117918, boot ELF):
    pitch = T[0xD0 + 16*(note-center) + fine + ((bend-64)*range>>2)]
with T the 2^(x/192) ladder at vram 0x241CA4 (T_eng[0xD0] = 12542 =
4096*2^(310/192)), then pitch*44100/48000 into the SPU2 pitch register
(0x1000 = 48000 Hz).  SFX sound records store bend=0 and the tone records
store bend_range=12, so the net per-tone rate is
    rate = 44100 * 2^((310 - 192 + 16*(note-center) + fine) / 192)
For the direct-map programs used by SFX banks (prog[0] == 0xFF) the trigger
note for tone t is prog.base_note + t, so every tone has one fixed rate.

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

# Engine stream-cue tables in the user's boot ELF (SCUS_971.12, decompiled
# from func_001FA790: entry = {u32 start_sector, u32 start_byte, u32
# byte_len, u32 flag}; start_sector is relative to the file's first LBA,
# flag=1 marks the looping in-level BGM cues).  Entry 0 of each table is
# null padding; real cue ids start at 1.
DEFAULT_ELF = Path("elf/SCUS_971.12.elf")
MUSIC_CUE_TABLE = (0x0025DD30, 68)      # (vram, entry count) -> cues 1..67
VOICE_CUE_TABLE = (0x0025E170, 179)     # -> cues 1..178


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

def read_cue_table(elf_path: Path, stereo: bool) -> list[tuple[int, int, int, int]]:
    """Read a stream cue table out of the user's locally-supplied boot ELF.

    Returns the raw 16-byte entries as (start_sector, start_byte, byte_len,
    flag) tuples, index == engine cue id.  The tables are how the game
    itself seeks MUSIC.DAT/VOICE.DAT (func_001FA790 adds start_sector to
    the file's first LBA), so they are the authoritative track boundaries.
    """
    data = elf_path.read_bytes()
    if data[:4] != b"\x7fELF":
        raise ValueError(f"{elf_path} is not an ELF")
    (phoff,) = struct.unpack_from("<I", data, 0x1C)
    phentsize, phnum = struct.unpack_from("<2H", data, 0x2A)
    segs = []
    for i in range(phnum):
        p_type, p_offset, p_vaddr, _, p_filesz = struct.unpack_from(
            "<5I", data, phoff + i * phentsize)
        if p_type == 1 and p_filesz:                      # PT_LOAD
            segs.append((p_vaddr, p_offset, p_filesz))

    def file_off(vaddr: int) -> int:
        for va, off, sz in segs:
            if va <= vaddr < va + sz:
                return off + (vaddr - va)
        raise ValueError(f"vaddr {vaddr:#x} not in any LOAD segment")

    vram, count = MUSIC_CUE_TABLE if stereo else VOICE_CUE_TABLE
    base = file_off(vram)
    rows = [struct.unpack_from("<4I", data, base + i * 16) for i in range(count)]
    for i, (sect, byte_off, _, _) in enumerate(rows):
        if byte_off != sect * 2048:
            raise ValueError(f"cue {i}: start_byte {byte_off:#x} != "
                             f"start_sector*2048 ({sect:#x}) - wrong ELF?")
    return rows


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

    cues = None
    if args.elf:
        elf_path = Path(args.elf)
        if elf_path.is_file():
            cues = read_cue_table(elf_path, stereo)
        elif args.elf != str(DEFAULT_ELF):
            print(f"error: --elf {elf_path} not found")
            return 1
        else:
            print(f"note: {elf_path} not found - using silence split "
                  f"(tracks named track_NNNN, adjacent cues may merge)")

    if cues is not None:
        # Engine cue ids -> exact clips. A cue's [start_byte, +byte_len)
        # range in the raw stream maps to per-channel frames: each 2048-byte
        # sector holds one 1024-byte L + one 1024-byte R block (64 frames
        # each) for music, or 128 mono frames for voice.
        clips = []
        for cue_id, (_, byte_off, byte_len, _) in enumerate(cues):
            if byte_len == 0:
                continue                      # entry 0 null padding
            if stereo:
                sf, nf = byte_off // 2048 * 64, byte_len // 2048 * 64
            else:
                sf, nf = byte_off // FRAME, byte_len // FRAME
            clips.append((cue_id, sf, sf + nf))
    else:
        clips = [(None, sf, ef) for sf, ef in split_on_silence(left, args.gap)]

    written = total_samples = 0
    for cue_id, sf, ef in clips:
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
        name = (f"cue_{cue_id:03d}.wav" if cue_id is not None
                else f"track_{written:04d}.wav")
        write_wav(out / name, pcm, args.rate, 2 if stereo else 1)
        written += 1
    print(f"{written} {'stereo' if stereo else 'mono'} "
          f"{'cues' if cues is not None else 'tracks'} -> {out}/ "
          f"({total_samples / args.rate:.0f}s @ {args.rate} Hz)")
    return 0


# ------------------------------------------------------------------- banks

def region_bytes(region_dir: Path) -> bytes:
    """Concatenate a region's extracted files (f00, f01, ...) in order."""
    parts = sorted(region_dir.glob("f*_id*.bin"), key=lambda p: int(p.name[1:3]))
    return b"".join(p.read_bytes() for p in parts)


def parse_container(data: bytes) -> dict | None:
    """Parse an SShd multi-bank container (layout in the module docstring).

    Returns {'total', 'img_off', 'img_size', 'banks': [{'hd', 'body_base',
    'body_size', 'type'}]} or None.  body_base is a file offset into `data`
    and is the bank's SPU-upload base: tone-record sample offsets (<<3) are
    relative to it.
    """
    if len(data) < 0x60:
        return None
    total, hdr0 = struct.unpack_from("<2I", data, 0)
    if not (0x20 <= hdr0 <= 0x400) or hdr0 + 0x10 > len(data):
        return None
    if data[hdr0 + 0xC:hdr0 + 0x10] != b"SShd":
        return None
    nbanks = struct.unpack_from("<I", data, 0xC)[0]
    img_off, img_size = struct.unpack_from("<2I", data, 0x10)
    if img_off + img_size != total or not (1 <= nbanks <= 16):
        return None
    banks, base = [], img_off
    for i in range(nbanks):
        body_size, hd, btype, _ = struct.unpack_from("<4I", data, 0x20 + 16 * i)
        if data[hd + 0xC:hd + 0x10] != b"SShd":
            return None
        banks.append(dict(hd=hd, body_base=base, body_size=body_size,
                          type=btype))
        base += body_size
    if base != total:
        return None
    return dict(total=total, img_off=img_off, img_size=img_size, banks=banks)


def parse_programs(data: bytes, hd: int) -> list[dict]:
    """All program records of one bank (both region slots, music + SFX).

    A program region (header offset hd+0x10 = music, hd+0x24 = SFX;
    0xFFFFFFFF = absent) is: u16 max_program_index, u16 offsets[idx+1]
    (0xFFFF = absent, relative to the region base), each program is
    {u8 ntones|0x80 or 0xFF, u8 mvol, u8 mpan, u8, u8 bend_range, u8,
    u8 base_note, u8 top_note} + ntones 16-byte tone records.
    For 0xFF (direct-map) programs ntones = top_note - base_note + 1 and
    tone t is triggered only by note base_note + t.
    """
    progs = []
    for reg_field in (0x10, 0x24):
        reg = struct.unpack_from("<I", data, hd + reg_field)[0]
        if reg == 0xFFFFFFFF:
            continue
        pr = hd + reg
        maxidx = struct.unpack_from("<H", data, pr)[0]
        offs = struct.unpack_from(f"<{maxidx + 1}H", data, pr + 2)
        for pi, po in enumerate(offs):
            if po == 0xFFFF:
                continue
            p = pr + po
            h = data[p:p + 8]
            direct = h[0] == 0xFF
            ntones = (h[7] - h[6] + 1) if direct else (h[0] & 0x7F)
            tones = []
            for t in range(ntones):
                te = data[p + 8 + t * 16:p + 8 + (t + 1) * 16]
                if len(te) < 16:
                    break
                tones.append(dict(
                    note_lo=te[0], note_hi=te[1], center=te[2],
                    fine=struct.unpack("b", te[3:4])[0],
                    samp_off=struct.unpack_from("<H", te, 4)[0] << 3,
                    adsr1=struct.unpack_from("<H", te, 6)[0],
                    adsr2=struct.unpack_from("<H", te, 8)[0],
                    bend_range=te[0xD], flags=te[0xF],
                    note=(h[6] + t) if direct else te[0]))
            progs.append(dict(index=pi, region=reg_field, direct=direct,
                              base_note=h[6], top_note=h[7],
                              prog_bend_range=h[4], tones=tones))
    return progs


def tone_rate(tone: dict, bend: int = 0) -> float:
    """Engine-exact playback rate in Hz for one tone record.

    Mirrors func_00117918 + the 44100/48000 rescale in func_00115E50/
    func_00115850: a 2^(x/192) u16 ladder (4096-anchored at vram 0x241CA4;
    the code's base pointer D_00241D70 is 102 entries in and indexes it at
    +0xD0, so the trigger-time anchor is 4096*2^(310/192)), stepped by
    16/semitone, +fine, + MIDI-style bend ((bend-64)*range>>2 steps).
    SFX sound records store bend = 0 (wheel fully down -> -range
    semitones); range = tone bend_range (12 in every bank inspected), or
    the program's if tone flags bit 4 is set (not observed).
    SPU2 pitch 0x1000 = 48000 Hz; the engine multiplies by 44100/48000,
    so rate = 44100 * ladder/4096.
    """
    rng = tone["bend_range"]
    steps = (310 + 16 * (tone["note"] - tone["center"]) + tone["fine"]
             + (((bend - 0x40) * rng) >> 2))
    return 44100.0 * 2.0 ** (steps / 192.0)


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


def vag_block_at(data: bytes, off: int) -> bytes:
    """The ADPCM block starting at `off` (frames until the end flag)."""
    end = off
    while end + FRAME <= len(data):
        end += FRAME
        if data[end - FRAME + 1] & 1:
            break
    return data[off:end]


def cmd_sfx(args) -> int:
    root = Path(args.input)
    out = Path(args.out) / "sfx"
    out.mkdir(parents=True, exist_ok=True)
    uniq: dict[tuple[bytes, int], int] = {}
    manifest = [
        "# Extermination SShd banks -> SFX at engine-exact rates",
        "# rate = 44100 * 2^((310-192 + 16*(note-center) + fine)/192)"
        "  (see docs/FINDINGS.md 'SShd bank format')",
        "# columns: region bank prog tone note center fine rate_hz"
        " body_off size wav", ""]
    n_containers = n_banks = refs = bad_offsets = 0
    sources: list[tuple[str, bytes]] = []
    for region_dir in sorted(p for p in root.iterdir() if p.is_dir()):
        data = region_bytes(region_dir)
        if parse_container(data):
            # container starts at f00 and spans the region's files
            sources.append((region_dir.name, data))
            continue
        # otherwise look for a container wholly inside one file
        # (e.g. the global player/weapon/UI bank in chunk00/f05_id05.bin)
        for f in sorted(region_dir.glob("f*_id*.bin")):
            fdata = f.read_bytes()
            if parse_container(fdata):
                sources.append((f"{region_dir.name}/{f.name}", fdata))
    for src_name, data in sources:
        cont = parse_container(data)
        n_containers += 1
        for bi, bank in enumerate(cont["banks"]):
            n_banks += 1
            for prog in parse_programs(data, bank["hd"]):
                for ti, tone in enumerate(prog["tones"]):
                    pos = bank["body_base"] + tone["samp_off"]
                    if pos >= cont["total"]:
                        bad_offsets += 1
                        continue
                    raw = vag_block_at(data, pos)
                    if len(raw) < 2 * FRAME:    # terminator / silence stub
                        continue
                    rate = int(round(tone_rate(tone)))
                    key = (hashlib.sha1(raw).digest(), rate)
                    sid = uniq.get(key)
                    if sid is None:
                        sid = len(uniq)
                        uniq[key] = sid
                        pcm = decode_adpcm(raw)
                        write_wav(out / f"snd_{sid:04d}.wav", pcm, rate, 1)
                    manifest.append(
                        f"{src_name} bank{bi} prog{prog['index']}"
                        f" tone{ti:02d} note=0x{tone['note']:02X}"
                        f" center={tone['center']:3d} fine={tone['fine']:4d}"
                        f" rate={rate:5d} off=0x{tone['samp_off']:06X}"
                        f" len={len(raw):6d} snd_{sid:04d}.wav")
                    refs += 1
    (out / "manifest.txt").write_text("\n".join(manifest) + "\n")
    print(f"{n_containers} containers, {n_banks} banks, {refs} tone refs, "
          f"{len(uniq)} unique (sound,rate) -> {out}/"
          + (f"  [{bad_offsets} out-of-range offsets skipped]"
             if bad_offsets else ""))
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
                    help="silent frames that delimit a track (fallback split)")
    sp.add_argument("--min-frames", type=int, default=8)
    sp.add_argument("--elf", default=str(DEFAULT_ELF),
                    help="boot ELF to read the engine cue table from "
                         "(authoritative track boundaries + cue-id names); "
                         "pass --elf '' to force the silence split")
    sp.set_defaults(func=lambda a: cmd_stream(a, stereo=True))

    sp = sub.add_parser("voice", help="decode VOICE.DAT (mono)")
    sp.add_argument("file", help="path to STREAM/VOICE.DAT")
    sp.add_argument("--out", default=str(DEFAULT_OUT))
    sp.add_argument("--rate", type=int, default=STREAM_RATE)
    sp.add_argument("--gap", type=int, default=64)
    sp.add_argument("--min-frames", type=int, default=8)
    sp.add_argument("--elf", default=str(DEFAULT_ELF),
                    help="boot ELF to read the engine cue table from; "
                         "pass --elf '' to force the silence split")
    sp.set_defaults(func=lambda a: cmd_stream(a, stereo=False))

    sp = sub.add_parser(
        "sfx", help="decode all SShd banks at engine-exact per-tone rates")
    sp.add_argument("--in", dest="input", default="extract",
                    help="extract_data.py output directory")
    sp.add_argument("--out", default=str(DEFAULT_OUT))
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
