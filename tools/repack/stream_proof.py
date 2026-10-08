"""Read-only evidence that the original IOP driver plays edited disc sectors.

The driver offsets and flag substitutions follow its independent reference
oracle in the native port. This checks captured state; it never runs an emulator.
"""
from __future__ import annotations

from pathlib import Path
import struct

from .archive import sha256_file
from .streams import _rows, _validate_rows, PROFILES


def _driver_signature(irx: bytes) -> bytes:
    if irx[:6] != b'\x7fELF\x01\x01' or len(irx) < 52:
        raise ValueError('expected original ELF32 IOP module')
    start = struct.unpack_from('<I', irx, 32)[0]
    stride, count, strings_index = struct.unpack_from('<3H', irx, 46)
    if stride < 40 or strings_index >= count or start + stride * count > len(irx):
        raise ValueError('invalid IOP section headers')
    sections = [struct.unpack_from('<10I', irx, start + i * stride) for i in range(count)]
    names = sections[strings_index]
    strings = irx[names[4]:names[4] + names[5]]
    for section in sections:
        if strings[section[0]:].split(b'\0', 1)[0] == b'.text':
            if section[5] < 0x64C or section[4] + section[5] > len(irx):
                raise ValueError('IOP text is truncated')
            return irx[section[4] + 0x634:section[4] + 0x64C]
    raise ValueError('IOP module has no text section')


def _driver_state(iop: bytes, spu: bytes, signature: bytes):
    location = iop.find(signature)
    if location < 0 or iop.find(signature, location + 1) >= 0:
        raise ValueError('original SNDN2DRV cannot be uniquely located in IOP RAM')
    base = location - 0x634
    if base < 0 or base + 0x778C > len(iop):
        raise ValueError('truncated IOP driver state')
    voices = [struct.unpack_from('<13I', iop, base + 0x76C0 + v * 0x34) for v in range(4)]
    if any(row[3] != 0x4000 or row[2] + row[3] > 0x200000 for row in voices):
        raise ValueError('unsupported original stream SPU buffer configuration')
    staging = iop[base + 0x46B0:base + 0x46B0 + 0x2000]
    position, candidates = spu.find(staging), set()
    while position >= 0:
        for row in voices:
            for half in (0, 1):
                candidate = position - row[2] - half * 0x2000
                if (0 <= candidate <= len(spu) - 0x200000
                        and all(spu[candidate + voice[2] + h * 0x2000 + 1] == flag
                                for voice in voices[:2] for h, flag in ((0, 6), (1, 2)))):
                    candidates.add(candidate)
        position = spu.find(staging, position + 1)
    if len(candidates) != 1:
        raise ValueError('SPU RAM location is not uniquely established by the driver staging transfer')
    return base, candidates.pop(), voices


def _matching_half(channel: bytes, actual: bytes, half: int):
    # Ignore only the two boundary flag bytes which the original transfer
    # consumer replaces. Compare the complete8192-byte transfer afterward.
    prefix = actual[0x20:0x60]
    position = channel.find(prefix)
    while position >= 0:
        start = position - 0x20
        if start >= 0 and start % 0x400 == 0:
            # EE refills wrap to the cue's first sector, including while a
            # one-shot timer is still active. A short loop can repeat several
            # times inside one physical SPU half.
            first = channel[start:start + 0x2000]
            extra = 0x2000 - len(first)
            expected = bytearray(first + channel * (extra // len(channel)) + channel[:extra % len(channel)])
            expected[1], expected[-15] = (6, 2) if half == 0 else (2, 3)
            if expected == actual:
                return start
        position = channel.find(prefix, position + 1)
    return None


def verify_playback(stream_file: Path, elf: Path, driver_irx: Path, captures: list[dict],
                    *, kind: str, cues: list[int], require_cue_identity=True) -> dict:
    """Require matching active playback with advancing NAX in two captures.

    Each capture has iop_memory/spu_state paths, stream_lanes from the EE
    snapshot receipt and an optional label. Disabling require_cue_identity is
    useful only for byte-comparison diagnostics, not proof of a specific cue.
    ELF must be the boot from the edited disc, so its rows address this stream.
    """
    if kind not in PROFILES or len(captures) < 2 or not cues:
        raise ValueError('playback proof requires a stream kind, cue IDs and at least two captures')
    stream_file, elf, driver_irx = map(Path, (stream_file, elf, driver_irx))
    rows = _rows(elf.read_bytes(), kind)
    _validate_rows(rows, kind, stream_file.stat().st_size)
    if any(type(cue) is not int or not 1 <= cue < len(rows) for cue in cues):
        raise ValueError('playback cue ID is outside the native table')
    payloads = {}
    with stream_file.open('rb') as source:
        for cue in cues:
            source.seek(rows[cue][1])
            raw = source.read(rows[cue][2])
            payloads[cue] = ([b''.join(raw[i + 0x400 * c:i + 0x400 * (c + 1)]
                                     for i in range(0, len(raw), 0x800)) for c in (0, 1)]
                             if kind == 'music' else [raw])
    signature = _driver_signature(driver_irx.read_bytes())
    observations = []
    for capture in captures:
        lanes = capture.get('stream_lanes')
        if require_cue_identity and not isinstance(lanes, list):
            raise ValueError('playback cue attribution requires captured EE stream_lanes metadata')
        iop_path, spu_path = Path(capture['iop_memory']), Path(capture['spu_state'])
        iop, spu = iop_path.read_bytes(), spu_path.read_bytes()
        base, ram_offset, voices = _driver_state(iop, spu, signature)
        found = []
        for voice in ((0, 1) if kind == 'music' else (2, 3)):
            row = voices[voice]
            flags, _stride, start, size, _iop_buffer, _iop_size, left, right, rate, nax = row[:10]
            if not (flags & 1 and rate and (left or right) and start <= nax < start + size):
                continue
            playing_half = (nax - start) // 0x2000
            channel = 1 - voice if kind == 'music' else 0
            for cue, channels in payloads.items():
                if require_cue_identity and not any(
                        lane.get('active') == 2 and lane.get('cue') == cue
                        and (lane.get('lane') == 0 if kind == 'music'
                             else lane.get('lane') in (1, 2) and lane.get('voice') == voice)
                        for lane in lanes):
                    continue
                matches = []
                for half in (0, 1):
                    at = ram_offset + start + half * 0x2000
                    position = _matching_half(channels[channel], spu[at:at + 0x2000], half)
                    if position is not None:
                        matches.append(dict(half=half, channel_byte_offset=position))
                if any(match['half'] == playing_half for match in matches):
                    found.append(dict(cue=cue, voice=voice, active=True, volume=[left, right], rate=rate,
                                      nax=nax, spu_start=start, playing_half=playing_half, halves=matches))
        observations.append(dict(label=capture.get('label', str(iop_path.parent)), driver_base=base,
                                 spu_ram_offset=ram_offset, iop_sha256=sha256_file(iop_path),
                                 spu_sha256=sha256_file(spu_path), matches=found))
    advancing = []
    for index, first in enumerate(observations):
        for later in observations[index + 1:]:
            for a in first['matches']:
                for b in later['matches']:
                    if (a['voice'], a['cue']) == (b['voice'], b['cue']) and a['nax'] != b['nax']:
                        advancing.append(dict(cue=a['cue'], voice=a['voice'],
                                              first=first['label'], later=later['label'], nax=[a['nax'], b['nax']]))
    if not advancing or kind == 'music' and not any(
            {row['voice'] for row in advancing if row['cue'] == cue} == {0, 1} for cue in cues):
        raise ValueError('no two captures prove advancing active playback of the requested cue bytes')
    return dict(kind=kind, cue_identity_verified=require_cue_identity,
                stream_sha256=sha256_file(stream_file), elf_sha256=sha256_file(elf),
                observations=observations, advancing=advancing)
