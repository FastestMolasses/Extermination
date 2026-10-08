"""Editable sector-addressed music/dialogue streams with verified ELF cue patches.

Music uses 1024-byte left/right ADPCM blocks; voice is mono. Cue IDs and
loop flags remain fixed. All disc-derived templates and patches stay local.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import tempfile
import wave

from tools.audio_export import decode_adpcm, deinterleave, interleave_pcm
from .archive import safe_output, sha256_file
from .audio import _frames, _input, _output, encode_adpcm

SCHEMA = 'extermination-stream-v1'
BUNDLE_SCHEMA = 'extermination-stream-bundle-v1'
PROFILES = {
    'music': dict(filename='MUSIC.DAT', vram=0x25DD30, count=68, channels=2),
    'voice': dict(filename='VOICE.DAT', vram=0x25E170, count=179, channels=1),
}
RATE = 48000
SECTOR = 2048


def _hash(raw):
    return hashlib.sha256(raw).hexdigest()


def _table_offset(elf: bytes, kind: str) -> int:
    profile = PROFILES[kind]
    vram, length = profile['vram'], profile['count'] * 16
    if len(elf) < 52 or elf[:6] != b'\x7fELF\x01\x01':
        raise ValueError('cue tables require a complete ELF32 little-endian executable')
    begin = struct.unpack_from('<I', elf, 28)[0]
    stride, count = struct.unpack_from('<HH', elf, 42)
    if stride < 32 or begin + stride * count > len(elf):
        raise ValueError('ELF program headers are outside the file')
    matches = []
    for i in range(count):
        kind_id, offset, address, _physical, size = struct.unpack_from('<5I', elf, begin + i * stride)
        if kind_id == 1 and address <= vram and vram + length <= address + size:
            if offset + size > len(elf):
                raise ValueError('ELF LOAD segment is truncated')
            matches.append(offset + vram - address)
    if len(matches) != 1:
        raise ValueError('cue table must lie in exactly one complete ELF LOAD segment')
    return matches[0]


def _rows(elf: bytes, kind: str) -> list[list[int]]:
    offset = _table_offset(elf, kind)
    return [list(struct.unpack_from('<4I', elf, offset + i * 16)) for i in range(PROFILES[kind]['count'])]


def _validate_rows(rows, kind, size):
    if not isinstance(rows, list) or len(rows) != PROFILES[kind]['count'] or rows[0] != [0, 0, 0, 0]:
        raise ValueError('stream cue IDs/count/null entry changed')
    cursor = 0
    for row in rows[1:]:
        if not isinstance(row, list) or len(row) != 4 or any(type(x) is not int or not 0 <= x <= 0xFFFFFFFF for x in row):
            raise ValueError('cue rows require four unsigned32-bit integers')
        sector, offset, length, flag = row
        if (offset != sector * SECTOR or offset != cursor or not length or length > 0x7FFFF800 or length % SECTOR
                or flag not in (0, 1) or kind == 'voice' and flag):
            raise ValueError('cue spans must tile the stream in sector units with original loop flags')
        cursor += length
    if cursor != size or cursor > 0xFFFFFFFF:
        raise ValueError('cue lengths do not cover the complete stream file')


def _decode(native: bytes, channels: int) -> bytes:
    if channels == 1:
        _frames(native)
        return decode_adpcm(native)
    if len(native) % SECTOR:
        raise ValueError('stereo stream requires complete left/right sectors')
    left, right = deinterleave(native, 64)
    _frames(left)
    _frames(right)
    return interleave_pcm(decode_adpcm(left), decode_adpcm(right))


def _encode(pcm: bytes, kind: str, *, loop=False) -> tuple[bytes, int]:
    profile = PROFILES[kind]
    channels = profile['channels']
    samples = len(pcm) // (2 * channels)
    per_sector = 1792 if channels == 2 else 3584
    # The original one-shot timer subtracts30 video ticks from native duration.
    # Reserve31 ticks after rounding the request up to a tick: one extra tick
    # also covers its float32 conversion rounding. Looped music ignores that
    # timer, so it receives only unavoidable whole-sector PCM padding.
    required = samples if loop else ((samples + 799) // 800 + 31) * 800
    sectors = max(1, (required + per_sector - 1) // per_sector)
    padded_samples = sectors * per_sector
    pcm += bytes((padded_samples - samples) * 2 * channels)
    if channels == 1:
        native = encode_adpcm(pcm, (bytes((12, 0)) + bytes(14)) * (padded_samples // 28),
                              fast=True, reset_first=True)
    else:
        left, right = bytearray(len(pcm) // 2), bytearray(len(pcm) // 2)
        left[0::2], left[1::2] = pcm[0::4], pcm[1::4]
        right[0::2], right[1::2] = pcm[2::4], pcm[3::4]
        template = (bytes((12, 0)) + bytes(14)) * (padded_samples // 28)
        left = encode_adpcm(bytes(left), template, fast=True, reset_first=True)
        right = encode_adpcm(bytes(right), template, fast=True, reset_first=True)
        native = b''.join(left[i:i + 1024] + right[i:i + 1024] for i in range(0, len(left), 1024))
    return native, padded_samples - samples


def _destination(path, sources):
    path = safe_output(path)
    if path.exists() and (not path.is_dir() or any(path.iterdir())):
        raise ValueError('stream output directory must be new or empty')
    if any(Path(source).resolve().is_relative_to(path) for source in sources):
        raise ValueError('stream output directory contains an input')
    return path


def unpack_stream(source: Path, elf: Path, out_dir: Path, *, kind='auto', cues=None) -> dict:
    source, elf = Path(source).resolve(), Path(elf).resolve()
    if kind == 'auto':
        kind = {'MUSIC.DAT': 'music', 'VOICE.DAT': 'voice'}.get(source.name.upper())
    if kind not in PROFILES:
        raise ValueError('stream kind must be music or voice')
    out = _destination(out_dir, [source, elf])
    profile = PROFILES[kind]
    elf_raw = elf.read_bytes()
    rows = _rows(elf_raw, kind)
    _validate_rows(rows, kind, source.stat().st_size)
    selected = list(range(1, len(rows))) if cues is None else sorted(set(cues))
    if any(type(cue) is not int or not 1 <= cue < len(rows) for cue in selected):
        raise ValueError('selected cue ID is outside the existing table')
    out.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, out / 'original.dat')
    shutil.copy2(elf, out / 'original.elf')
    exports = []
    with source.open('rb') as stream:
        for cue in selected:
            stream.seek(rows[cue][1])
            native = stream.read(rows[cue][2])
            pcm = _decode(native, profile['channels'])
            name = f'cue_{cue:03d}.wav'
            with wave.open(str(out / name), 'wb') as wav:
                wav.setnchannels(profile['channels'])
                wav.setsampwidth(2)
                wav.setframerate(RATE)
                wav.writeframes(pcm)
            exports.append(dict(cue=cue, path=name, native_sha256=_hash(native), pcm_sha256=_hash(pcm)))
    result = dict(schema=SCHEMA, kind=kind, source=str(source), elf_source=str(elf),
                  original_stream_sha256=sha256_file(source), original_elf_sha256=_hash(elf_raw),
                  rows=rows, exports=exports, channels=profile['channels'], rate=RATE,
                  limits='Fixed cue IDs and loop flags; changed durations pad to whole native sectors. '
                         'One-shots also receive31 silent video ticks for the native early-stop timer. '
                         'Original bytes are retained for unchanged PCM.')
    (out / 'manifest.json').write_text(json.dumps(result, indent=2) + '\n')
    return result


def pack_stream(tree: Path, out_dir: Path) -> dict:
    tree = Path(tree).resolve()
    manifest_path, original, elf_path = [_input(tree, name) for name in ('manifest.json', 'original.dat', 'original.elf')]
    manifest = json.loads(manifest_path.read_text())
    if not isinstance(manifest, dict) or manifest.get('schema') != SCHEMA or manifest.get('kind') not in PROFILES:
        raise ValueError('unsupported stream editing manifest')
    kind = manifest['kind']
    profile = PROFILES[kind]
    elf = elf_path.read_bytes()
    if _hash(elf) != manifest['original_elf_sha256'] or sha256_file(original) != manifest['original_stream_sha256']:
        raise ValueError('original stream/ELF template changed')
    rows = _rows(elf, kind)
    _validate_rows(rows, kind, original.stat().st_size)
    if rows != manifest['rows'] or manifest['channels'] != profile['channels'] or manifest['rate'] != RATE:
        raise ValueError('stream layout changed; edit exported WAV files only')
    exports = {}
    inputs = [manifest_path, original, elf_path, Path(manifest['source']), Path(manifest['elf_source'])]
    for entry in manifest['exports']:
        cue = entry['cue']
        if type(cue) is not int or not 1 <= cue < len(rows) or cue in exports or entry['path'] != f'cue_{cue:03d}.wav':
            raise ValueError('invalid or duplicate exported cue')
        exports[cue] = _input(tree, entry['path'])
        inputs.append(exports[cue])
    out = _destination(out_dir, inputs)
    if out.is_relative_to(tree) or tree.is_relative_to(out):
        raise ValueError('stream bundle must be separate from its editing tree')
    out.mkdir(parents=True, exist_ok=True)
    native_path = out / profile['filename']
    new_rows, changed, cursor, digest = [[0, 0, 0, 0]], [], 0, hashlib.sha256()
    with original.open('rb') as stream, native_path.open('xb') as target:
        for cue, old_row in enumerate(rows[1:], 1):
            stream.seek(old_row[1])
            native = stream.read(old_row[2])
            if cue in exports:
                try:
                    with wave.open(str(exports[cue]), 'rb') as wav:
                        if (wav.getnchannels() != profile['channels'] or wav.getsampwidth() != 2
                                or wav.getframerate() != RATE or wav.getcomptype() != 'NONE' or not wav.getnframes()):
                            raise ValueError('stream WAV must be nonempty PCM16 at48000Hz with the original channel count')
                        pcm = wav.readframes(wav.getnframes())
                        if len(pcm) != wav.getnframes() * 2 * profile['channels']:
                            raise ValueError('truncated stream WAV')
                except (wave.Error, EOFError) as exc:
                    raise ValueError('invalid stream WAV') from exc
                if pcm != _decode(native, profile['channels']):
                    native, padding = _encode(pcm, kind, loop=bool(old_row[3]))
                    changed.append(dict(cue=cue, original_size=old_row[2], new_size=len(native),
                                        padded_pcm_samples=padding, requested_pcm_samples=len(pcm) // (2 * profile['channels'])))
            new_rows.append([cursor // SECTOR, cursor, len(native), old_row[3]])
            target.write(native)
            digest.update(native)
            cursor += len(native)
    _validate_rows(new_rows, kind, cursor)
    offset = _table_offset(elf, kind)
    result = dict(schema=BUNDLE_SCHEMA, kind=kind, stream_file=profile['filename'], stream_size=cursor,
                  stream_sha256=digest.hexdigest(), original_stream_sha256=manifest['original_stream_sha256'],
                  original_elf_sha256=manifest['original_elf_sha256'], table_vram=profile['vram'], table_count=profile['count'],
                  original_table_sha256=_hash(elf[offset:offset + profile['count'] * 16]),
                  rows=new_rows, changed_cues=changed, byte_identical=not changed)
    (out / 'cue-edits.json').write_text(json.dumps(result, indent=2) + '\n')
    return dict(result, bundle=str(out), stream_path=str(native_path), patch_manifest=str(out / 'cue-edits.json'))


def inspect_bundle(bundle: Path, original_elf: Path) -> dict:
    """Validate stream bytes and cue metadata against a fresh matching boot."""
    bundle = Path(bundle).resolve()
    patch = json.loads(_input(bundle, 'cue-edits.json').read_text())
    if not isinstance(patch, dict) or patch.get('schema') != BUNDLE_SCHEMA or patch.get('kind') not in PROFILES:
        raise ValueError('unsupported cue patch bundle')
    kind = patch['kind']
    profile = PROFILES[kind]
    if (patch['stream_file'] != profile['filename'] or patch['table_vram'] != profile['vram']
            or patch['table_count'] != profile['count']):
        raise ValueError('cue patch attempts to change a non-profile table or file')
    elf = Path(original_elf).read_bytes()
    if _hash(elf) != patch['original_elf_sha256']:
        raise ValueError('cue bundle requires the exact fresh matching original ELF')
    offset = _table_offset(elf, kind)
    old_table = elf[offset:offset + profile['count'] * 16]
    if _hash(old_table) != patch['original_table_sha256']:
        raise ValueError('original cue table hash mismatch')
    native = _input(bundle, patch['stream_file'])
    if native.stat().st_size != patch['stream_size'] or sha256_file(native) != patch['stream_sha256']:
        raise ValueError('packed stream differs from its verified cue metadata')
    _validate_rows(patch['rows'], kind, patch['stream_size'])
    old_rows = _rows(elf, kind)
    if any(old[3] != new[3] for old, new in zip(old_rows, patch['rows'])):
        raise ValueError('cue patch may change only start sector, byte offset, and length; loop flags are immutable')
    return dict(patch, stream_path=str(native), elf_file_offset=offset, bundle=str(bundle))


def apply_cue_patches(fresh_matching_elf: Path, bundle_dirs, out_elf: Path) -> dict:
    """Patch verified data tables only, after the canonical fresh source build."""
    fresh_matching_elf = Path(fresh_matching_elf).resolve()
    bundles = [inspect_bundle(bundle, fresh_matching_elf) for bundle in bundle_dirs]
    if len({bundle['kind'] for bundle in bundles}) != len(bundles):
        raise ValueError('only one bundle per stream kind can be applied')
    inputs = [fresh_matching_elf]
    for bundle in bundles:
        inputs.extend([Path(bundle['stream_path']), Path(bundle['bundle']) / 'cue-edits.json'])
    out = _output(out_elf, inputs)
    raw = fresh_matching_elf.read_bytes()
    rebuilt = bytearray(raw)
    changes = []
    overrides = {}
    for bundle in bundles:
        for cue, row in enumerate(bundle['rows']):
            offset = bundle['elf_file_offset'] + cue * 16
            packed = struct.pack('<3I', *row[:3])
            if raw[offset:offset + 12] != packed:
                changes.append(dict(kind=bundle['kind'], cue=cue, elf_file_offset=offset))
            rebuilt[offset:offset + 12] = packed
        overrides['STREAM/' + bundle['stream_file']] = bundle['stream_path']
    out.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix='.cue-patch-', dir=out.parent)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(rebuilt)
        os.replace(temporary, out)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    return dict(output=str(out), sha256=_hash(rebuilt), original_elf_sha256=_hash(raw),
                byte_identical=rebuilt == raw, changed_rows=changes, overrides=overrides,
                bundles=[dict(kind=b['kind'], stream_size=b['stream_size'], stream_sha256=b['stream_sha256']) for b in bundles])
