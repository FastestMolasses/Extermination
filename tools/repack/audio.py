"""Lossless-source WAV editing for SShd banks and mono PS2 ADPCM clips.

WAV is a lossy-codec editing view: unchanged PCM reuses the original ADPCM.
Edits keep the original frame count, frame flags, container headers and padding.
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
    manifest = {"schema": SCHEMA, "source": str(source), "original_sha256": _hash(raw),
                "layout": layout, "sample_count": len(layout["samples"]),
                "limits": "Mono, fixed sample count and preview rate; native frame flags and metadata preserved. No cue-stream editing."}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def encode_adpcm(pcm: bytes, original: bytes) -> bytes:
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
        predictors = (0,) if flags & 4 else range(5)
        for predictor in predictors:
            c1, c2 = COEFFICIENTS[predictor]
            for shift in range(13):
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
    output = _output(out_file, inputs + paths)
    rebuilt = bytearray(raw)
    changed = []
    for sample, path in zip(layout["samples"], paths):
        try:
            with wave.open(str(path), "rb") as wav:
                if (wav.getnchannels() != 1 or wav.getsampwidth() != 2 or wav.getcomptype() != "NONE"
                        or wav.getframerate() != layout["rate"] or wav.getnframes() != sample["pcm_samples"]):
                    raise ValueError("WAV must retain mono PCM16, preview rate, and original sample count")
                pcm = wav.readframes(wav.getnframes())
        except (wave.Error, EOFError) as exc:
            raise ValueError("edited audio must be a complete PCM16 WAV") from exc
        original = raw[sample["offset"]:sample["offset"] + sample["size"]]
        if pcm == decode_adpcm(original):
            continue
        encoded = encode_adpcm(pcm, original)
        rebuilt[sample["offset"]:sample["offset"] + sample["size"]] = encoded
        changed.append(sample["path"])
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
            "byte_identical": rebuilt == raw, "size": len(rebuilt)}
