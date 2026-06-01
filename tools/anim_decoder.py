#!/usr/bin/env python3
"""
anim_decoder.py -- Decode keyframed animation streams out of id 0x71 entries.

Format (reverse-engineered from func_001C8F10 + func_001C84D0 (rotation) and
func_001C90D0 + func_001C85D0 (translation); see docs/FINDINGS.md ->
"Keyframe stream format" for the long-form derivation):

  Each id 0x71 *clip* entry has a small header (offsets to section1/section2),
  followed by per-section data.

  - Section 1 holds ROTATION keyframes (quat).
  - Section 2 holds TRANSLATION keyframes (vec3).

  Each section starts with a u32 directory: directory[bone_idx] is the byte
  offset (from section base) to that bone's keyframe stream. Each stream is
  a contiguous list of 12-byte keyframe records:

      offset  size  field
      +0x00   10 B  packed sample
      +0x0A    2 B  u16 t_next   (the frame index at which THIS sample is held;
                                  the previous record's +0x0A is t_prev)

  The packed sample is a sequence of bit-fields, each of which is the
  TOP W bits of a standard IEEE-754 single-precision float (the low
  mantissa bits are zeroed). Width per channel:

      rotation     W = 20 bits  -> 4 channels * 20 = 80 bits = 10 bytes
      translation  W = 26 bits  -> 3 channels * 26 = 78 bits + 2 bits pad

  Decode: take the W-bit field, shift LEFT by (32 - W), reinterpret as float.

This module is *self-contained* and does no I/O against the disc -- the caller
is expected to slice an id 0x71 entry into bytes already (e.g. from a local
copy extracted by the user's own tooling per the project's hard rules).
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
from typing import List, Tuple


# ---------------------------------------------------------------------------
# bit-cast helpers


def _u32_to_f32(u: int) -> float:
    return struct.unpack("<f", struct.pack("<I", u & 0xFFFFFFFF))[0]


def _read_bits_le(buf: bytes, bit_off: int, width: int) -> int:
    """Read `width` bits from `buf` starting at bit offset `bit_off`,
    little-endian byte order, LSB-first within each byte. Returns
    unsigned int."""
    val = 0
    out_bit = 0
    while out_bit < width:
        byte = buf[bit_off >> 3]
        in_bit = bit_off & 7
        take = min(8 - in_bit, width - out_bit)
        chunk = (byte >> in_bit) & ((1 << take) - 1)
        val |= chunk << out_bit
        bit_off += take
        out_bit += take
    return val


def _decode_truncfloat(packed: int, width: int) -> float:
    """Reverse of: packed = bit_cast<u32>(float) >> (32 - width).
    Shifts the W-bit field back into the high bits and reinterprets."""
    return _u32_to_f32((packed & ((1 << width) - 1)) << (32 - width))


# ---------------------------------------------------------------------------
# Public API


@dataclass
class Keyframe:
    """One decoded keyframe."""
    t_next: int                       # u16; this sample is held until time hits this
    values: Tuple[float, ...]         # (qx, qy, qz, qw) for rot; (tx, ty, tz) for trans


def parse_section(section_bytes: bytes,
                  num_bones: int,
                  channel_widths: Tuple[int, ...]) -> List[List[Keyframe]]:
    """Parse one section (rotation = section1, or translation = section2).

    Returns a list of per-bone keyframe lists. `channel_widths` is the
    per-channel bit width (20 for rotation, 26 for translation).

    The stream end for each bone is detected heuristically as the next
    bone's directory entry (or section end for the last bone)."""
    # directory of u32 offsets
    dir_size = num_bones * 4
    offsets = list(struct.unpack(f"<{num_bones}I", section_bytes[:dir_size]))
    # establish each stream's end byte
    sorted_offs = sorted(offsets) + [len(section_bytes)]
    end_of = {}
    for i, o in enumerate(sorted_offs[:-1]):
        end_of[o] = sorted_offs[i + 1]

    nchan = len(channel_widths)
    bytes_per_sample = (sum(channel_widths) + 7) // 8   # 10 for both
    record_size = 12                                    # sample(10) + u16 t_next

    out = []
    for bone_idx in range(num_bones):
        start = offsets[bone_idx]
        end = end_of[start]
        frames: List[Keyframe] = []
        cur = start
        while cur + record_size <= end:
            sample = section_bytes[cur:cur + bytes_per_sample]
            t_next = struct.unpack("<H", section_bytes[cur + 0x0A:cur + 0x0C])[0]
            # decode each channel
            vals = []
            bit_off = 0
            for w in channel_widths:
                raw = _read_bits_le(sample, bit_off, w)
                vals.append(_decode_truncfloat(raw, w))
                bit_off += w
            frames.append(Keyframe(t_next=t_next, values=tuple(vals)))
            # sentinel detection: t_next == 0xFFFF (or wrapping back to a small value
            # that is < previous t_next) is a plausible end-of-stream marker; we let
            # the byte-boundary check handle termination instead.
            cur += record_size
        out.append(frames)
    return out


def parse_rotation_section(section1_bytes: bytes, num_bones: int) -> List[List[Keyframe]]:
    """Section 1 of an id 0x71 clip entry: per-bone quat keyframes."""
    return parse_section(section1_bytes, num_bones, (20, 20, 20, 20))


def parse_translation_section(section2_bytes: bytes, num_bones: int) -> List[List[Keyframe]]:
    """Section 2 of an id 0x71 clip entry: per-bone translation keyframes."""
    return parse_section(section2_bytes, num_bones, (26, 26, 26))


# ---------------------------------------------------------------------------
# Single-frame sampling


def _lerp(a: float, b: float, t: float) -> float:
    return a + (b - a) * t


def _normalize_quat(q: Tuple[float, float, float, float]) -> Tuple[float, float, float, float]:
    n = sum(c * c for c in q) ** 0.5
    if n == 0.0:
        return (0.0, 0.0, 0.0, 1.0)
    return tuple(c / n for c in q)  # type: ignore[return-value]


def sample_bone(frames: List[Keyframe], time_frames: float, normalize: bool = True):
    """Sample the value at `time_frames` (can be fractional). The caller-side
    blend factor `t = (now - t_prev) / (t_next - t_prev)` (see FINDINGS).

    For rotation, this is *linear* interpolation followed by renormalisation
    (NLERP). The engine itself uses a SLERP-like routine downstream
    (func_001CA0A0); NLERP is close enough for validation."""
    if not frames:
        return None
    # find the interval [t_prev, t_next) such that t_prev <= time < t_next
    # records[i].t_next is the END of record i; t_prev of record i is t_next
    # of record i-1 (with t_prev of record 0 = 0).
    for i, kf in enumerate(frames):
        t_prev = 0 if i == 0 else frames[i - 1].t_next
        t_next = kf.t_next
        if t_prev <= time_frames < t_next:
            # record i holds the SAMPLE that is reached at t_next; the
            # previous record's sample is t_prev. So we interpolate from
            # frames[i-1].values -> frames[i].values across [t_prev, t_next).
            if i == 0:
                blended = kf.values  # hold first sample until we leave t_next
            else:
                t = (time_frames - t_prev) / max(1.0, (t_next - t_prev))
                blended = tuple(
                    _lerp(frames[i - 1].values[k], kf.values[k], t)
                    for k in range(len(kf.values))
                )
            if normalize and len(blended) == 4:
                return _normalize_quat(blended)  # type: ignore[arg-type]
            return blended
    # past the end: clamp to last sample
    last = frames[-1].values
    if normalize and len(last) == 4:
        return _normalize_quat(last)  # type: ignore[arg-type]
    return last


# ---------------------------------------------------------------------------
# Section 3 -- per-clip EVENT TABLE (reverse-engineered 2026-05-27 from
# func_001C64F0 + func_001C8480; see docs/FINDINGS.md -> "Section 3 / event
# table" for the derivation).
#
# Section 3 of an id 0x71 entry is a fixed 0x4b0-byte VIF-priming block (the
# 30 inner offsets + 30 * 36-byte empty priming records) followed by an
# optional EVENT TABLE at section3_base + 0x4b0:
#
#     +0x000  s16 count            # number of event records
#     +0x002  s16 reserved (0)
#     +0x004  count * { s16 frame, u16 flag }
#     +ENDED  0xFFFFFFFF sentinel  # one or two u32 sentinels of padding
#
# Per-frame consumer (func_001C64F0) walks the table linearly, comparing
# `frame` against int(clip_time_in_frames), and on the FIRST match OR-merges
# `flag` into the bone state's return-status word (low 12 bits) before
# breaking. `count == 0` means "no events this clip" (the block degenerates
# to the sentinel only).
#
# The event table's existence for a given entry is signalled by the entry
# header's u32 at +0x14 being non-zero: when set, it equals
# section3_off + 0x4b0 (so the +0x14 word is purely a fast-path pointer to
# what's already locatable at section3_base + 0x4b0).
#
# Empirically (player rig copies across chunk05/06/07/08/11/12/20): of 400
# id 0x71 entries scanned, 35 carry a non-empty event table totalling 77
# event records; every observed `flag` payload is 0x0009 -- consistent with
# a single "footstep / sound trigger" event class for the player's locomotion
# clips. Other rigs/clips may use different flag values.


@dataclass
class AnimEvent:
    frame: int  # signed 16-bit frame index this event fires on
    flag: int   # unsigned 16-bit event-flag payload (OR'd into bone status)


def parse_event_section(section3_bytes: bytes,
                        table_off: int = 0x4b0) -> List[AnimEvent]:
    """Decode the optional event table inside section 3.

    `section3_bytes` is the full section3 slice (from section3_base up to
    the start of the next entry / EOF). `table_off` defaults to 0x4b0,
    the empirically-uniform offset of the event table head past the
    per-bone empty-priming block. Returns [] if count == 0.
    """
    if table_off + 4 > len(section3_bytes):
        return []
    count = struct.unpack_from("<h", section3_bytes, table_off)[0]
    if count <= 0 or table_off + 4 + 4 * count > len(section3_bytes):
        return []
    events: List[AnimEvent] = []
    base = table_off + 4
    for i in range(count):
        frame = struct.unpack_from("<h", section3_bytes, base + 4 * i)[0]
        flag = struct.unpack_from("<H", section3_bytes, base + 4 * i + 2)[0]
        events.append(AnimEvent(frame=frame, flag=flag))
    return events


# ---------------------------------------------------------------------------
# self-test


def _selftest() -> None:
    # Build a synthetic record: rotation channel where qx=1.0, qy=qz=qw=0.0
    # 1.0 as float = 0x3F800000; truncate to top 20 bits = 0x3F800.
    bits = 0
    for chan_idx, val in enumerate([1.0, 0.0, 0.0, 0.0]):
        u = struct.unpack("<I", struct.pack("<f", val))[0]
        trunc = u >> (32 - 20)
        bits |= trunc << (chan_idx * 20)
    # pack to 10 bytes + u16 t_next
    sample = bits.to_bytes(10, "little")
    rec = sample + struct.pack("<H", 30)  # held until frame 30
    # directory: 1 bone, offset = 4 (right after the directory u32)
    section = struct.pack("<I", 4) + rec + rec  # two identical kfs
    frames = parse_rotation_section(section, num_bones=1)[0]
    assert len(frames) == 2, frames
    qx, qy, qz, qw = frames[0].values
    # truncating the bottom 12 mantissa bits of 1.0 leaves 1.0 exactly
    assert abs(qx - 1.0) < 1e-6, qx
    assert abs(qy) < 1e-6 and abs(qz) < 1e-6 and abs(qw) < 1e-6
    print("rotation pack/unpack roundtrip OK:", frames[0].values)

    # Translation: tx = 12.5, ty = -3.25, tz = 100.0
    bits = 0
    widths = (26, 26, 26)
    for chan_idx, val in enumerate([12.5, -3.25, 100.0]):
        u = struct.unpack("<I", struct.pack("<f", val))[0]
        trunc = u >> (32 - widths[chan_idx])
        bits |= trunc << (chan_idx * widths[chan_idx])
    sample = bits.to_bytes(10, "little")
    rec = sample + struct.pack("<H", 60)
    section = struct.pack("<I", 4) + rec
    tframes = parse_translation_section(section, num_bones=1)[0]
    tx, ty, tz = tframes[0].values
    assert abs(tx - 12.5) < 1e-3, tx
    assert abs(ty - -3.25) < 1e-3, ty
    assert abs(tz - 100.0) < 1e-2, tz
    print("translation pack/unpack roundtrip OK:", tframes[0].values)

    # Event-table round-trip: build a synthetic section3 with two events.
    body = bytes(0x4b0)  # empty priming block (don't care for this test)
    body += struct.pack("<hH", 2, 0)
    body += struct.pack("<hH", 12, 0x0009)
    body += struct.pack("<hH", 32, 0x0009)
    body += b"\xff\xff\xff\xff"  # sentinel
    evts = parse_event_section(body)
    assert len(evts) == 2 and evts[0].frame == 12 and evts[0].flag == 0x0009, evts
    assert evts[1].frame == 32 and evts[1].flag == 0x0009, evts
    # empty case
    body0 = bytes(0x4b0) + struct.pack("<hH", 0, 0) + b"\xff\xff\xff\xff\xff\xff\xff\xff"
    assert parse_event_section(body0) == [], parse_event_section(body0)
    print("event table pack/unpack roundtrip OK:", evts)
    print("self-test PASS")


if __name__ == "__main__":
    import sys
    if len(sys.argv) == 1 or "--selftest" in sys.argv[1:]:
        _selftest()
    else:
        print(__doc__)
