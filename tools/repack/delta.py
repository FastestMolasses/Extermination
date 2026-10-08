"""Deterministic typed COPY/XOR/INSERT deltas for locally supplied originals.

The fixed header authenticates both files. There are no comments, extension
fields, filenames or optional metadata in the instruction stream. Variable data
is exposed separately so distribution checks can scan it against the whole ISO.
"""
from __future__ import annotations

import ctypes
import hashlib
import struct

from .original_scan import _native

MAGIC = b'EMDLT1\0\0'
HEADER = struct.Struct('<8sQQ32s32sI')
REFERENCE = struct.Struct('<QQ')
LENGTH = struct.Struct('<Q')
COPY, XOR, INSERT = 1, 2, 3
MAX_FILE = 0xFFFFFFFF
MAX_OPERATIONS = 1_000_000
DEFAULT_APPLY_LIMIT = 1024 * 1024 * 1024


class DeltaError(ValueError):
    pass


def _digest(raw):
    return hashlib.sha256(raw).digest()


def _parse(delta, *, max_output_size=MAX_FILE, payload_ranges=None):
    if type(max_output_size) is not int or not 0 <= max_output_size <= MAX_FILE:
        raise DeltaError('output limit must be an integer within the 32-bit file-size range')
    if not isinstance(delta, bytes) or len(delta) < HEADER.size:
        raise DeltaError('delta must be a complete immutable byte string')
    magic, base_size, edited_size, base_hash, edited_hash, count = HEADER.unpack_from(delta)
    if magic != MAGIC:
        raise DeltaError('unsupported delta signature/version')
    if base_size > MAX_FILE or edited_size > min(MAX_FILE, max_output_size):
        raise DeltaError('delta file size exceeds the allowed limit')
    if count > MAX_OPERATIONS or count > (len(delta) - HEADER.size) // 9:
        raise DeltaError('delta operation count exceeds its bounded instruction stream')
    cursor, produced, previous, operations = HEADER.size, 0, None, []
    view = memoryview(delta)
    for _ in range(count):
        if cursor >= len(delta):
            raise DeltaError('truncated delta opcode')
        code = delta[cursor]
        cursor += 1
        if code in (COPY, XOR):
            if cursor + REFERENCE.size > len(delta):
                raise DeltaError('truncated delta reference')
            offset, length = REFERENCE.unpack_from(delta, cursor)
            cursor += REFERENCE.size
            if offset > base_size or length > base_size - offset:
                raise DeltaError('delta reference escapes the original file')
        elif code == INSERT:
            if cursor + LENGTH.size > len(delta):
                raise DeltaError('truncated delta insertion length')
            offset, length = 0, LENGTH.unpack_from(delta, cursor)[0]
            cursor += LENGTH.size
        else:
            raise DeltaError('unknown delta opcode')
        if not length or length > edited_size - produced:
            raise DeltaError('zero-length operation or output exceeds its declared size')
        payload = view[cursor:cursor]
        if code != COPY:
            if length > len(delta) - cursor:
                raise DeltaError('truncated delta payload')
            payload = view[cursor:cursor + length]
            if payload_ranges is not None:
                payload_ranges.append((cursor, cursor + length))
            cursor += length
            if code == XOR and not any(payload):
                raise DeltaError('all-zero XOR must be represented by COPY')
        if previous and previous[0] == code:
            if code == INSERT or previous[1] + previous[2] == offset:
                raise DeltaError('adjacent mergeable operations are not canonical')
        previous = (code, offset, length)
        operations.append((code, offset, length, payload))
        produced += length
    if cursor != len(delta) or produced != edited_size:
        raise DeltaError('delta has trailing bytes or an incomplete output')
    return dict(base_size=base_size, edited_size=edited_size, base_sha256=base_hash.hex(),
                edited_sha256=edited_hash.hex(), operation_count=count), operations


def validate(delta: bytes, *, max_output_size=MAX_FILE) -> dict:
    """Validate exact grammar and bounds without allocating its output."""
    return _parse(delta, max_output_size=max_output_size)[0]


def distributable_payloads(delta: bytes):
    """Return XOR/INSERT byte views in order, after validating the entire delta."""
    _header, operations = _parse(delta)
    return tuple(payload for code, _offset, _length, payload in operations if code != COPY)


def distributable_payload_ranges(delta: bytes):
    """Return payload [start, end) offsets for scanning enclosing raw containers.

    The whole delta is validated before any ranges escape. Callers can preserve
    literal bytes in their original container position while excluding only the
    typed delta header/instructions from a raw-byte distribution scan.
    """
    ranges = []
    _parse(delta, payload_ranges=ranges)
    return tuple(ranges)


def _xor(left: bytes, right: bytes) -> bytes:
    if len(left) != len(right):
        raise DeltaError('XOR spans have different lengths')
    if len(left) < 4096:
        return bytes(a ^ b for a, b in zip(left, right))
    output = ctypes.create_string_buffer(len(left))
    _native().em_xor(left, right, output, len(left))
    return output.raw


def _common(left: bytes, right: bytes, *, backwards=False, limit=None) -> int:
    """Native bytes comparisons skip large unchanged prefixes/suffixes cheaply."""
    limit = min(len(left), len(right)) if limit is None else limit
    done = 0
    while done < limit:
        count = min(65536, limit - done)
        a = left[len(left) - done - count:len(left) - done] if backwards else left[done:done + count]
        b = right[len(right) - done - count:len(right) - done] if backwards else right[done:done + count]
        if a == b:
            done += count
            continue
        low, high = 0, count
        while low < high:
            middle = (low + high + 1) // 2
            equal = a[-middle:] == b[-middle:] if backwards else a[:middle] == b[:middle]
            if equal:
                low = middle
            else:
                high = middle - 1
        return done + low
    return done


def encode(base: bytes, edited: bytes, *, prefer_xor=True) -> bytes:
    """Encode a deterministic delta; native sparse anchors handle moved spans.

    Changed gaps prefer XOR masks. prefer_xor=False allows a distribution gate
    to retry INSERT if an incidental XOR mask matches unrelated original bytes.
    Both representations must pass the whole-original distribution scan.
    """
    if not isinstance(base, bytes) or not isinstance(edited, bytes):
        raise TypeError('delta inputs must be immutable bytes')
    if max(len(base), len(edited)) > MAX_FILE:
        raise DeltaError('PS2 file delta exceeds the 32-bit file-size limit')
    operations = []

    def append(code, offset, length, payload=b''):
        if not length:
            return
        if code == XOR and not any(payload):
            code, payload = COPY, b''
        if operations and operations[-1][0] == code:
            old_code, old_offset, old_length, old_payload = operations[-1]
            if code == INSERT or old_offset + old_length == offset:
                operations[-1] = (code, old_offset, old_length + length, old_payload + payload)
                return
        operations.append((code, offset, length, payload))
        if len(operations) > MAX_OPERATIONS:
            raise DeltaError('edited file needs too many delta operations')

    prefix = _common(base, edited)
    suffix = _common(base, edited, backwards=True, limit=min(len(base), len(edited)) - prefix)
    append(COPY, 0, prefix)
    original = base[prefix:len(base) - suffix]
    changed = edited[prefix:len(edited) - suffix]

    def gap(start, end, original_hint):
        data = changed[start:end]
        available = min(len(data), max(0, len(original) - original_hint)) if prefer_xor else 0
        if available:
            append(XOR, prefix + original_hint, available,
                   _xor(original[original_hint:original_hint + available], data[:available]))
        append(INSERT, 0, len(data) - available, data[available:])

    if min(len(original), len(changed)) < 64:
        gap(0, len(changed), 0)
    else:
        lib = _native()
        index = lib.em_match_make(original, len(original))
        if not index:
            raise MemoryError('could not allocate original delta anchor index')
        try:
            cursor, hint = 0, 0
            found = (ctypes.c_uint64 * 3)()
            while lib.em_match_next(index, changed, len(changed), cursor, found):
                target, source, length = map(int, found)
                gap(cursor, target, hint)
                append(COPY, prefix + source, length)
                cursor, hint = target + length, source + length
            gap(cursor, len(changed), hint)
        finally:
            lib.em_index_free(index)
    append(COPY, len(base) - suffix, suffix)
    out = bytearray(HEADER.pack(MAGIC, len(base), len(edited), _digest(base), _digest(edited), len(operations)))
    for code, offset, length, payload in operations:
        out.append(code)
        out.extend(REFERENCE.pack(offset, length) if code != INSERT else LENGTH.pack(length))
        out.extend(payload)
    result = bytes(out)
    validate(result)
    return result


def apply(base: bytes, delta: bytes, *, max_output_size=DEFAULT_APPLY_LIMIT) -> bytes:
    """Apply only to the exact original; bound allocation before reconstruction."""
    if not isinstance(base, bytes):
        raise TypeError('delta original must be immutable bytes')
    header, operations = _parse(delta, max_output_size=max_output_size)
    if len(base) != header['base_size'] or _digest(base).hex() != header['base_sha256']:
        raise DeltaError('delta original size/hash mismatch')
    output = bytearray(header['edited_size'])
    target, source, cursor = memoryview(output), memoryview(base), 0
    for code, offset, length, payload in operations:
        if code == COPY:
            target[cursor:cursor + length] = source[offset:offset + length]
        elif code == XOR:
            target[cursor:cursor + length] = _xor(base[offset:offset + length], bytes(payload))
        else:
            target[cursor:cursor + length] = payload
        cursor += length
    if _digest(output).hex() != header['edited_sha256']:
        raise DeltaError('delta edited output hash mismatch')
    return bytes(output)
