"""Lossless-source WAV editing for SShd banks and mono PS2 ADPCM clips.

WAV is a lossy-codec editing view: unchanged PCM reuses the original ADPCM.
Edits can change duration, with frame rounding, loop and bank-table relocation.
Raw MUSIC.DAT/VOICE.DAT cue-stream editing is deliberately outside this module.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import struct
import tempfile
import wave

from tools.audio_export import decode_adpcm
from .archive import safe_output

SCHEMA = "extermination-audio-v1"
COEFFICIENTS = ((0, 0), (60, 0), (115, -52), (98, -55), (122, -60))
SPU_BASES = {1: 0x15040, 2: 0x1A0000, 3: 0x122000, 4: 0x132000}
# Original LIBSD modes0..9 reserve at most0x18040 bytes (modes7/8).
# Sequences can switch modes; growth cannot assume the default mode4.
SPU_LIMITS = {1: 0x122000, 2: 0x1E7FC0, 3: 0x132000, 4: 0x187FC0}


def _default_loop(raw: bytes):
    if raw[-15] & 3 != 3:
        return None
    starts = [i for i in range(len(raw) // 16) if raw[i * 16 + 1] & 4]
    start = starts[-1] if starts else 0
    return "end" if start == len(raw) // 16 - 1 else start


def _flag_template(original: bytes, frames: int, loop) -> bytes:
    """Keep an explicit loop start; its end follows the new final ADPCM frame."""
    if frames < 1:
        raise ValueError("audio must contain at least one PCM sample")
    if loop is not None and loop != "end" and (type(loop) is not int or not 0 <= loop < frames):
        raise ValueError("loop start no longer fits; edit loops.json using ADPCM frame indices")
    if frames == len(original) // 16 and loop == _default_loop(original):
        return original
    flags = [original[min(i, max(0, len(original) // 16 - 2)) * 16 + 1] & ~5 for i in range(frames)]
    if loop is None:
        flags[-1] = (flags[-1] & ~7) | 1
    else:
        flags[-1] |= 3
        flags[-1 if loop == "end" else loop] |= 4
    return b"".join(bytes((12, flag)) + bytes(14) for flag in flags)


def _tone_fields(raw: bytes, bank: dict, banks: list[dict], image_offset: int) -> list[tuple[int, int]]:
    """Enumerate actual tone starts in both program tables, including aliases."""
    header = bank["header"]
    end = min([b["header"] for b in banks if b["header"] > header] + [image_offset])

    def bounded(at, size):
        if at < header + 0x28 or at + size > end:
            raise ValueError("SShd program/tone table is outside its bank header")

    fields = {}
    for slot in (0x10, 0x24):
        offset = _u32(raw, header + slot)
        if offset == 0xFFFFFFFF:
            continue
        region = header + offset
        bounded(region, 2)
        maximum = struct.unpack_from("<H", raw, region)[0]
        bounded(region + 2, 2 * (maximum + 1))
        for number in range(maximum + 1):
            relative = struct.unpack_from("<H", raw, region + 2 + number * 2)[0]
            if relative == 0xFFFF:
                continue
            program = region + relative
            bounded(program, 8)
            count = raw[program + 7] - raw[program + 6] + 1 if raw[program] == 0xFF else raw[program] & 0x7F
            if count < 0:
                raise ValueError("invalid direct-map note range")
            bounded(program + 8, count * 16)
            for tone in range(count):
                at = program + 8 + tone * 16 + 4
                start = struct.unpack_from("<H", raw, at)[0] << 3
                if start % 16 or start >= bank["size"]:
                    raise ValueError("tone sample start is not a valid ADPCM frame in its bank")
                fields[at] = start
    return sorted(fields.items())


def _check_bank_capacity(banks: list[dict], sizes: list[int]) -> list[dict]:
    if len(banks) != len(sizes):
        raise ValueError("bank allocation count changed")
    spans = []
    first = 0
    while first < len(banks):
        kind = banks[first]["type"]
        if kind not in SPU_BASES:
            raise ValueError("resized bank uses an uncharacterized SPU allocation group")
        last = first + 1
        while last < len(banks) and banks[last]["type"] == kind:
            last += 1
        old_end = SPU_BASES[kind]
        for bank in banks[first:last]:
            old_end = ((old_end + 63) & ~63) + bank["size"]
        # Preserve shipped occupied addresses even where larger reverb modes
        # would overlap them. New addresses must stay below every mode's area.
        limit = max(SPU_LIMITS[kind], old_end) if kind in (2, 4) else SPU_LIMITS[kind]
        cursor = SPU_BASES[kind]
        for size in sizes[first:last]:
            cursor = (cursor + 63) & ~63
            if size > 0x80000 or cursor + size > limit:
                raise ValueError("resized sound bank exceeds its supported SPU/IOP allocation capacity")
            spans.append(dict(group=kind, start=cursor, end=cursor + size, limit=limit,
                              original_group_end=old_end))
            cursor += size
        first = last
    return spans


def _hash(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _u32(raw: bytes, at: int) -> int:
    if at < 0 or at + 4 > len(raw):
        raise ValueError("truncated sound header")
    return struct.unpack_from("<I", raw, at)[0]


def _frames(raw: bytes) -> None:
    if not raw or len(raw) % 16:
        raise ValueError("ADPCM must contain complete nonempty 16-byte frames")
    if any(raw[i] >> 4 > 4 or raw[i] & 15 > 12 for i in range(0, len(raw), 16)):
        raise ValueError("ADPCM predictor or shift is outside the supported native range")


def _layout(raw: bytes, kind: str, rate: int) -> dict:
    if kind not in ("auto", "sshd", "raw", "vag"):
        raise ValueError("audio kind must be auto, sshd, raw, or vag")
    if not isinstance(rate, int) or not 1 <= rate <= 384000:
        raise ValueError("WAV preview rate must be between 1 and 384000 Hz")
    looks_bank = (len(raw) >= 0x60 and 0x20 <= _u32(raw, 4) <= len(raw) - 16
                  and raw[_u32(raw, 4) + 12:_u32(raw, 4) + 16] == b"SShd")
    if kind == "auto":
        kind = "sshd" if looks_bank else "vag" if raw[:4] == b"VAGp" else "raw"
    samples = []
    banks = []

    def add(offset, size, path, bank=None):
        _frames(raw[offset:offset + size])
        samples.append({"path": path, "offset": offset, "size": size,
                        "frames": size // 16, "pcm_samples": size // 16 * 28, "bank": bank})

    if kind == "sshd":
        if not looks_bank:
            raise ValueError("input is not an SShd multi-bank container")
        total, count = _u32(raw, 0), _u32(raw, 12)
        image_offset, image_size = _u32(raw, 16), _u32(raw, 20)
        if (not 1 <= count <= 16 or 32 + 16 * count > image_offset
                or image_offset + image_size != total or total > len(raw)):
            raise ValueError("invalid SShd image, count or container size")
        cursor = image_offset
        for bank in range(count):
            body_size, header, bank_type, _ = struct.unpack_from("<4I", raw, 32 + 16 * bank)
            if (header < 32 + 16 * count or header + 0x28 > image_offset
                    or raw[header + 12:header + 16] != b"SShd"
                    or _u32(raw, header + 4) != body_size
                    or cursor + body_size > total):
                raise ValueError("invalid SShd bank header or duplicated body size")
            body = raw[cursor:cursor + body_size]
            _frames(body)
            banks.append({"number": bank, "header": header, "offset": cursor,
                          "size": body_size, "type": bank_type})
            start = sample = 0
            for frame in range(body_size // 16):
                if body[frame * 16 + 1] & 1:
                    length = frame - start + 1
                    # A lone flag-7 frame is a terminator, not an auditionable sample.
                    if length != 1 or body[frame * 16 + 1] != 7:
                        add(cursor + start * 16, length * 16,
                            f"bank{bank:02d}/sample{sample:04d}.wav", bank)
                        sample += 1
                    start = frame + 1
            if start < body_size // 16:
                add(cursor + start * 16, body_size - start * 16,
                    f"bank{bank:02d}/sample{sample:04d}.wav", bank)
            cursor += body_size
        if cursor != total:
            raise ValueError("SShd bank bodies do not fill the sample image")
    elif kind == "vag":
        if len(raw) < 48 or raw[:4] != b"VAGp":
            raise ValueError("expected a mono VAGp header")
        size, rate = struct.unpack_from(">II", raw, 12)
        if size + 48 > len(raw) or not 1 <= rate <= 384000:
            raise ValueError("invalid VAGp sample length or rate")
        add(48, size, "sample.wav")
    else:
        add(0, len(raw), "sample.wav")
    return {"kind": kind, "rate": rate, "channels": 1, "banks": banks, "samples": samples}


def _input(tree: Path, name: str) -> Path:
    relative = Path(name)
    path = tree / relative
    if (relative.is_absolute() or ".." in relative.parts
            or not path.resolve().is_relative_to(tree.resolve()) or not path.is_file()):
        raise ValueError("audio input escapes its loose tree or is absent")
    return path


def _output(out_file: Path, inputs: list[Path]) -> Path:
    output = safe_output(out_file)
    for source in inputs:
        if source.exists() and (output == source.resolve() or output.exists() and output.samefile(source)):
            raise ValueError("audio output aliases an input")
    return output


def unpack_audio(source: Path, out_dir: Path, *, kind="auto", rate=48000) -> dict:
    """Export mono clips as PCM16 WAV, retaining every original byte locally."""
    source, output = Path(source).resolve(), safe_output(out_dir)
    if source.name.upper() in ("MUSIC.DAT", "VOICE.DAT"):
        raise ValueError("whole cue-stream editing requires ELF cue boundaries and is unsupported here")
    if output.exists() and (not output.is_dir() or any(output.iterdir())):
        raise ValueError("audio unpack destination must be new or empty")
    if source.is_relative_to(output):
        raise ValueError("audio unpack destination contains its input")
    raw = source.read_bytes()
    layout = _layout(raw, kind, rate)
    output.mkdir(parents=True, exist_ok=True)
    (output / "original.bin").write_bytes(raw)
    for sample in layout["samples"]:
        path = output / sample["path"]
        path.parent.mkdir(parents=True, exist_ok=True)
        pcm = decode_adpcm(raw[sample["offset"]:sample["offset"] + sample["size"]])
        with wave.open(str(path), "wb") as wav:
            wav.setnchannels(1)
            wav.setsampwidth(2)
            wav.setframerate(layout["rate"])
            wav.writeframes(pcm)
    loops = {sample["path"]: _default_loop(raw[sample["offset"]:sample["offset"] + sample["size"]])
             for sample in layout["samples"]}
    (output / "loops.json").write_text(json.dumps(loops, indent=2) + "\n")
    manifest = {"schema": SCHEMA, "source": str(source), "original_sha256": _hash(raw),
                "layout": layout, "sample_count": len(layout["samples"]),
                "limits": "Mono PCM16 at the exported preview rate; changed duration rounds to28 samples. "
                "loops.json uses ADPCM frame indices, null for one-shot, or end for a terminal-frame loop. "
                "Bank growth is limited by tone addressing and fixed SPU allocations."}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def encode_adpcm(pcm: bytes, original: bytes, *, fast=False, reset_first=False) -> bytes:
    """Deterministically choose predictor/shift by decoded squared error per frame."""
    _frames(original)
    if len(pcm) != len(original) // 16 * 28 * 2:
        raise ValueError("edited PCM must retain the original sample count")
    values = struct.unpack(f"<{len(pcm) // 2}h", pcm)
    output = bytearray()
    hist1 = hist2 = 0
    for frame in range(len(original) // 16):
        target = values[frame * 28:(frame + 1) * 28]
        flags = original[frame * 16 + 1]
        best = None
        # A predictor-free loop-start frame remains valid after the loop wraps.
        predictors = (0,) if flags & 4 or reset_first and frame == 0 else range(5)
        for predictor in predictors:
            c1, c2 = COEFFICIENTS[predictor]
            shifts = range(13)
            if fast:
                # Estimate residual range with the target history, then test
                # neighbouring quantizers with the actual decoded feedback.
                h1, h2, peak = hist1, hist2, 0
                for value in target:
                    peak = max(peak, abs(value - ((h1 * c1 + h2 * c2) >> 6)))
                    h2, h1 = h1, value
                step_bits = max(0, ((peak + 6) // 7 - 1).bit_length())
                center = max(0, min(12, 12 - step_bits))
                shifts = range(max(0, center - 1), min(12, center + 1) + 1)
            for shift in shifts:
                h1, h2, error = hist1, hist2, 0
                nibbles = []
                step = 1 << (12 - shift)
                for value in target:
                    prediction = (h1 * c1 + h2 * c2) >> 6
                    nibble = max(-8, min(7, (value - prediction + step // 2) // step))
                    decoded = max(-32768, min(32767, nibble * step + prediction))
                    error += (value - decoded) ** 2
                    h2, h1 = h1, decoded
                    nibbles.append(nibble & 15)
                if best is None or error < best[0]:
                    best = (error, predictor, shift, nibbles, h1, h2)
        _, predictor, shift, nibbles, hist1, hist2 = best
        output.extend((predictor << 4 | shift, flags))
        output.extend(nibbles[i] | nibbles[i + 1] << 4 for i in range(0, 28, 2))
    return bytes(output)


def pack_audio(tree: Path, out_file: Path) -> dict:
    """Reconstruct original bytes, re-encoding only WAVs whose PCM changed."""
    tree = Path(tree).resolve()
    manifest_path, original_path = _input(tree, "manifest.json"), _input(tree, "original.bin")
    manifest = json.loads(manifest_path.read_text())
    if not isinstance(manifest, dict) or manifest.get("schema") != SCHEMA:
        raise ValueError("unsupported audio manifest")
    raw = original_path.read_bytes()
    if _hash(raw) != manifest.get("original_sha256"):
        raise ValueError("original audio template changed")
    saved = manifest["layout"]
    if not isinstance(saved, dict):
        raise ValueError("audio layout must be an object")
    layout = _layout(raw, saved["kind"], saved["rate"])
    if layout != saved:
        raise ValueError("audio layout changed; edit WAV files only")
    inputs = [manifest_path, original_path, Path(manifest["source"])]
    paths = [_input(tree, sample["path"]) for sample in layout["samples"]]
    loops = {sample["path"]: _default_loop(raw[sample["offset"]:sample["offset"] + sample["size"]])
             for sample in layout["samples"]}
    if (tree / "loops.json").exists():
        loops_path = _input(tree, "loops.json")
        inputs.append(loops_path)
        edits = json.loads(loops_path.read_text())
        if not isinstance(edits, dict) or set(edits) != set(loops):
            raise ValueError("loops.json must contain exactly the exported sample paths")
        loops = edits
    output = _output(out_file, inputs + paths)
    replacements, changed, sample_changes = {}, [], []
    for sample, path in zip(layout["samples"], paths):
        try:
            with wave.open(str(path), "rb") as wav:
                if (wav.getnchannels() != 1 or wav.getsampwidth() != 2 or wav.getcomptype() != "NONE"
                        or wav.getframerate() != layout["rate"] or not wav.getnframes()):
                    raise ValueError("WAV must retain mono PCM16 and preview rate, with at least one sample")
                pcm = wav.readframes(wav.getnframes())
                if len(pcm) != wav.getnframes() * 2:
                    raise ValueError("WAV sample data is truncated")
        except (wave.Error, EOFError) as exc:
            raise ValueError("edited audio must be a complete PCM16 WAV") from exc
        original = raw[sample["offset"]:sample["offset"] + sample["size"]]
        loop = loops[sample["path"]]
        frames = (len(pcm) // 2 + 27) // 28
        template = _flag_template(original, frames, loop)
        if pcm == decode_adpcm(original) and loop == _default_loop(original):
            continue
        if layout["kind"] == "sshd":
            if frames == 1 and loop is not None:
                raise ValueError("a one-frame looping bank sample is indistinguishable from a terminator")
            bank = layout["banks"][sample["bank"]]
            sample_start = sample["offset"] - bank["offset"]
            if any(sample_start < start < sample_start + sample["size"]
                   for _at, start in _tone_fields(raw, bank, layout["banks"], _u32(raw, 16))):
                raise ValueError("editing a sample with independently keyed interior tone aliases is unsupported")
        padding = frames * 28 * 2 - len(pcm)
        encoded = encode_adpcm(pcm + bytes(padding), template)
        replacements[sample["path"]] = encoded
        changed.append(sample["path"])
        sample_changes.append(dict(path=sample["path"], original_bytes=len(original), new_bytes=len(encoded),
                                   padded_pcm_samples=padding // 2, loop_start=loop))

    resized = any(row["original_bytes"] != row["new_bytes"] for row in sample_changes)
    tone_updates, capacity = [], []
    if not resized:
        rebuilt = bytearray(raw)
        for sample in layout["samples"]:
            if sample["path"] in replacements:
                rebuilt[sample["offset"]:sample["offset"] + sample["size"]] = replacements[sample["path"]]
    elif layout["kind"] == "sshd":
        image_start = _u32(raw, 16)
        header = bytearray(raw[:image_start])
        bodies, sizes = [], []
        for bank in layout["banks"]:
            start, end = bank["offset"], bank["offset"] + bank["size"]
            chunks, relocations, cursor, new_cursor = [], [], start, 0
            for sample in (row for row in layout["samples"] if row["bank"] == bank["number"]):
                chunks.append(raw[cursor:sample["offset"]])
                new_cursor += sample["offset"] - cursor
                payload = replacements.get(sample["path"], raw[sample["offset"]:sample["offset"] + sample["size"]])
                relocations.append((sample["offset"] - start, sample["size"], new_cursor, len(payload)))
                chunks.append(payload)
                new_cursor += len(payload)
                cursor = sample["offset"] + sample["size"]
            chunks.append(raw[cursor:end])
            body = b"".join(chunks)
            sizes.append(len(body))
            bodies.append(body)
            if any(old_size != new_size for _begin, old_size, _new_begin, new_size in relocations):
                for at, old in _tone_fields(raw, bank, layout["banks"], image_start):
                    delta = 0
                    for begin, old_size, new_begin, new_size in relocations:
                        if old < begin:
                            break
                        if old < begin + old_size:
                            if old - begin >= new_size:
                                raise ValueError("tone points into a removed part of a resized sample")
                            delta = new_begin - begin
                            break
                        delta = new_begin + new_size - begin - old_size
                    new = old + delta
                    if new % 16 or new >= len(body) or new >> 3 > 0xFFFF:
                        raise ValueError("relocated tone exceeds native 16-bit sample addressing")
                    struct.pack_into("<H", header, at, new >> 3)
                    if new != old:
                        tone_updates.append(dict(bank=bank["number"], field=at, original=old, relocated=new))
            struct.pack_into("<I", header, 32 + bank["number"] * 16, len(body))
            struct.pack_into("<I", header, bank["header"] + 4, len(body))
        capacity = _check_bank_capacity(layout["banks"], sizes)
        total = image_start + sum(sizes)
        struct.pack_into("<I", header, 0, total)
        struct.pack_into("<I", header, 20, sum(sizes))
        rebuilt = header + b"".join(bodies) + raw[_u32(raw, 0):]
    else:
        sample = layout["samples"][0]
        payload = replacements.get(sample["path"], raw[sample["offset"]:sample["offset"] + sample["size"]])
        rebuilt = bytearray(raw[:sample["offset"]] + payload + raw[sample["offset"] + sample["size"]:])
        if layout["kind"] == "vag":
            struct.pack_into(">I", rebuilt, 12, len(payload))
    output.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".audio-", dir=output.parent)
    try:
        with os.fdopen(fd, "wb") as stream:
            stream.write(rebuilt)
        os.replace(temporary, output)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    return {"output": str(output), "sha256": _hash(rebuilt), "changed_samples": changed,
            "byte_identical": rebuilt == raw, "size": len(rebuilt), "sample_changes": sample_changes,
            "tone_updates": tone_updates, "spu_allocations": capacity}
