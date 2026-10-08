"""Relocating translation views for decoded message-bank OUTER/TEXT tables.

Native glyph bytes map one-to-one through Latin-1 in JSON; this is a byte
encoding, not a claim that the game's font uses Unicode. Unknown markup,
padding, directory words and control records remain in a protected template.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import struct
import tempfile

from .archive import safe_output

SCHEMA = "extermination-text-table-v1"


def _words(raw: bytes, at: int) -> tuple[int, int, int, int]:
    if at < 0 or at + 16 > len(raw):
        raise ValueError("truncated native text table")
    return struct.unpack_from("<4I", raw, at)


def _outer(raw: bytes, at: int, limit: int, group: int) -> dict:
    text_offset, count, records_size, directory = _words(raw, at)
    if directory != 16 or not 0 < count <= 65535 or text_offset != 16 + 16 * count:
        raise ValueError("unsupported OUTER text directory shape")
    text = at + text_offset + records_size
    string_base, text_count, string_bytes, kind = _words(raw, text)
    if (text_count != count or kind != 1 or string_base != 16 + 16 * count
            or text + string_base + string_bytes > limit):
        raise ValueError("invalid TEXT counts or string range")
    # Markup bytes are opaque, but their declared ranges must stay in the markup area.
    for line in range(count):
        record_offset, _, _, record_bytes = _words(raw, at + 16 + 16 * line)
        # func_001FE4D0 adds OUTER + OUTER[0] before this relative offset.
        if record_bytes and (record_bytes % 16 or record_offset + record_bytes > records_size):
            raise ValueError("OUTER markup record points outside its declared range")
    entries = []
    cursor = 0
    for line in range(count):
        offset, duplicate, length, terminated = _words(raw, text + 16 + 16 * line)
        position = text + string_base + offset
        if (offset != duplicate or offset != cursor or terminated != length + 1
                or offset + terminated > string_bytes or raw[position + length] != 0
                or b"\0" in raw[position:position + length]):
            raise ValueError("inconsistent TEXT line offset, size, or terminator")
        entries.append({"line": line, "offset": position, "size": length})
        cursor += terminated
    if cursor != string_bytes:
        raise ValueError("TEXT strings do not exactly fill their declared range")
    return {"group": group, "offset": at, "line_count": count, "lines": entries}


def _layout(raw: bytes, kind: str) -> dict:
    if kind not in ("auto", "bank", "outer"):
        raise ValueError("text kind must be auto, bank, or outer")
    base, count, size, directory = _words(raw, 0)
    if kind == "auto":
        # A bank has a directory of groups; an OUTER has a directory of markup.
        # Try the fully checked nested representation before the bare one.
        try:
            return _layout(raw, "bank")
        except ValueError:
            return _layout(raw, "outer")
    if kind == "outer":
        groups = [_outer(raw, 0, len(raw), 0)]
    else:
        if (directory != 16 or not 0 < count <= 65535 or base != directory + 16 * count
                or base + size > len(raw)):
            raise ValueError("invalid message-bank directory or payload length")
        groups = []
        cursor = 0
        for group in range(count):
            offset, units, length, padded = _words(raw, directory + 16 * group)
            if (offset != cursor or offset % 16 or units != offset // 16
                    or not 0 < length <= padded or padded % 16 or offset + padded > size):
                raise ValueError("invalid message-bank group extent")
            groups.append(_outer(raw, base + offset, base + offset + length, group))
            cursor += padded
        if cursor != size:
            raise ValueError("message-bank groups do not fill their declared payload")
    return {"kind": kind, "groups": groups}


def _input(tree: Path, name: str) -> Path:
    result = tree / name
    if not result.resolve().is_relative_to(tree.resolve()) or not result.is_file():
        raise ValueError("text input is absent or escapes its tree")
    return result


def _records(raw: bytes, group: dict, entry: dict) -> list[tuple[int, int]]:
    """Return (record address, byte anchor), never treat opaque words as pointers."""
    at = group["offset"]
    relative, _, _, size = _words(raw, at + 16 + 16 * entry["line"])
    base = at + _words(raw, at)[0] + relative
    return [(base + i, _words(raw, base + i)[2]) for i in range(0, size, 16)]


def _segments(text: bytes, records: list[tuple[int, int]]) -> list[str] | None:
    anchors = [position for _, position in records]
    if not anchors or anchors != sorted(anchors) or anchors[-1] > len(text):
        return None
    bounds = [0] + anchors + [len(text)]
    return [text[a:b].decode("latin-1") for a, b in zip(bounds, bounds[1:])]


def _encode(value: str) -> bytes:
    try:
        encoded = value.encode("latin-1")
    except UnicodeEncodeError as exc:
        raise ValueError("text must contain only native Latin-1 byte values") from exc
    if b"\0" in encoded:
        raise ValueError("edited text must contain no NUL")
    return encoded


def _renderer_limits(text: bytes, anchors: list[int], *, options: bool) -> None:
    """Mirror the bounded scratch writes in the readable decompilation.

    FC7B0 copies runs into char[0x80]; FE070 additionally accumulates markup
    spans into a zeroed 0x80-byte global. One byte must remain for NUL. The
    renderer itself flushes a 512px staging strip (CC1E0), not a line limit.
    """
    if anchors:
        boundaries = [0] + anchors + [len(text)]
        if any(len(part) > 127 for a, b in zip(boundaries, boundaries[1:])
               for part in text[a:b].split(b"\n")):
            raise ValueError("translation exceeds 127 bytes between a style anchor/newline: "
                             "func_001FE070 writes a 0x80-byte scratch buffer including NUL")
    boundaries = [0] + anchors + [len(text)]
    # FCBD0 can retrieve the same raw TEXT and bypass OUTER styles entirely;
    # validate its full runs as well as the individually flushed style spans.
    spans = [(0, len(text))] + list(zip(boundaries, boundaries[1:]))
    for start, end in spans:
        i = start
        while i < end:
            c = text[i]
            if c < 32 or c == 0x80 or 0xA0 <= c < 0xE0 or c >= 0xF0:
                i += 1
                continue
            length = 0
            while i < end and text[i] >= 32:
                # Native 0x81 consumes the following byte and emits one space.
                if text[i] == 0x81 and i + 1 >= end:
                    raise ValueError("native 0x81 escape is truncated by the string end or a style anchor (func_001FC7B0 consumes two bytes)")
                i += 2 if text[i] == 0x81 else 1
                length += 1
            if length > 127:
                raise ValueError("translation exceeds 127 rendered bytes before a control/newline: "
                                 "func_001FC7B0 copies into char buf[0x80] including NUL")
    if options:
        start = text.find(b"[")
        if (start >= 0 and start + 2 < len(text) and text[start + 1] in b"789"
                and text[start + 2] in b"0123456789ABCDEFabcdef"):
            if start + 4 > len(text):
                raise ValueError("options glyph escape must occupy four native bytes (func_001FCBD0)")
            if len(text) - 3 > 127:
                raise ValueError("options translation exceeds 127 bytes after glyph-escape substitution: "
                                 "func_001FCBD0 concatenates into Buf128")


def _line_edit(raw: bytes, group: dict, entry: dict, line: dict, *, options: bool):
    if (not isinstance(line, dict) or set(line) not in ({"line", "text"}, {"line", "text", "segments"})
            or line["line"] != entry["line"] or not isinstance(line["text"], str)):
        raise ValueError("table line identity changed")
    original = raw[entry["offset"]:entry["offset"] + entry["size"]]
    text = _encode(line["text"])
    records = _records(raw, group, entry)
    segments = _segments(original, records)
    anchors = [position for _, position in records]
    if "segments" in line:
        values = line["segments"]
        if segments is None or not isinstance(values, list) or len(values) != len(segments) or any(
                not isinstance(value, str) for value in values):
            raise ValueError("style segment count changed or original style anchors are unsupported")
        if values != segments:
            encoded = [_encode(value) for value in values]
            joined = b"".join(encoded)
            if text not in (original, joined):
                raise ValueError("conflicting text and segments edits; edit segments or keep text equal to their concatenation")
            text = joined
            anchors = []
            cursor = 0
            for segment in encoded[:-1]:
                cursor += len(segment)
                anchors.append(cursor)
    if len(text) != len(original) and records and anchors == [position for _, position in records]:
        # Without edited segments, an interior style anchor has no unique new
        # position. Never guess from a diff or silently style the wrong words.
        if segments is None or "segments" not in line or line["segments"] == segments:
            raise ValueError("length-changing styled translation must edit segments so every style anchor can relocate")
    if text != original or anchors != [position for _, position in records]:
        if records and (anchors != sorted(anchors) or anchors[-1] > len(text)):
            raise ValueError("cannot edit text with unsupported native style anchors")
        _renderer_limits(text, anchors, options=options)
    return text, list(zip((at for at, _ in records), anchors))


def _rebuild_outer(raw: bytes, group: dict, limit: int, texts: list[bytes], anchors: dict[int, int]) -> bytes:
    at = group["offset"]
    text_offset, count, records_size, _ = _words(raw, at)
    text_at = at + text_offset + records_size
    string_base, _, old_bytes, _ = _words(raw, text_at)
    start = text_at + string_base
    rebuilt = bytearray(raw[at:start])
    cursor = 0
    for index, value in enumerate(texts):
        if cursor + len(value) + 1 >= 1 << 32:
            raise ValueError("TEXT string storage exceeds native u32 offsets")
        struct.pack_into("<4I", rebuilt, text_at - at + 16 + index * 16,
                         cursor, cursor, len(value), len(value) + 1)
        cursor += len(value) + 1
    struct.pack_into("<I", rebuilt, text_at - at + 8, cursor)
    for record, position in anchors.items():
        struct.pack_into("<I", rebuilt, record - at + 8, position)
    rebuilt.extend(b"".join(value + b"\0" for value in texts))
    rebuilt.extend(raw[start + old_bytes:limit])
    return bytes(rebuilt)


def unpack_table(source: Path, out_dir: Path, *, kind="auto") -> dict:
    """Export a bank or bare OUTER table to an editable, ordered JSON document."""
    source, output = Path(source).resolve(), safe_output(out_dir)
    if output.exists() and (not output.is_dir() or any(output.iterdir())):
        raise ValueError("table unpack destination must be new or empty")
    if source.is_relative_to(output):
        raise ValueError("table unpack destination contains its input")
    raw = source.read_bytes()
    layout = _layout(raw, kind)
    table = {"encoding": "latin-1", "groups": []}
    for group in layout["groups"]:
        lines = []
        for entry in group["lines"]:
            text = raw[entry["offset"]:entry["offset"] + entry["size"]]
            line = {"line": entry["line"], "text": text.decode("latin-1")}
            segments = _segments(text, _records(raw, group, entry))
            if segments is not None:
                line["segments"] = segments
            lines.append(line)
        table["groups"].append({"group": group["group"], "lines": lines})
    output.mkdir(parents=True, exist_ok=True)
    (output / "original.bin").write_bytes(raw)
    (output / "table.json").write_text(json.dumps(table, indent=2, ensure_ascii=True) + "\n")
    manifest = {"schema": SCHEMA, "source": str(source), "layout": layout,
                "original_sha256": hashlib.sha256(raw).hexdigest(),
                "limits": "Existing line/group identities; longer or shorter Latin-1 byte strings relocate TEXT/bank directories. "
                          "For styled lines edit segments to relocate style anchors. Engine scratch runs allow 127 bytes plus NUL "
                          "(src/func_001FC7B0.c, src/func_001FE070.c); options substitution uses Buf128 "
                          "(src/func_001FCBD0.c). Native font coverage and on-screen fit are not Unicode/layout guarantees."}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def pack_table(tree: Path, out_file: Path) -> dict:
    """Relocate native string directories, style anchors and bank group extents."""
    tree = Path(tree).resolve()
    original_path, manifest_path, table_path = [_input(tree, name) for name in
                                               ("original.bin", "manifest.json", "table.json")]
    raw = original_path.read_bytes()
    manifest = json.loads(manifest_path.read_text())
    if (not isinstance(manifest, dict) or manifest.get("schema") != SCHEMA
            or manifest.get("original_sha256") != hashlib.sha256(raw).hexdigest()):
        raise ValueError("unsupported table manifest or changed original template")
    if not isinstance(manifest.get("layout"), dict):
        raise ValueError("table layout must be an object")
    layout = _layout(raw, manifest["layout"]["kind"])
    if layout != manifest["layout"]:
        raise ValueError("table layout changed; edit table.json text fields only")
    edited = json.loads(table_path.read_text())
    if (not isinstance(edited, dict) or set(edited) != {"encoding", "groups"} or edited["encoding"] != "latin-1"
            or not isinstance(edited["groups"], list) or len(edited["groups"]) != len(layout["groups"])):
        raise ValueError("table encoding or group count changed")
    output = safe_output(out_file)
    for path in (original_path, manifest_path, table_path, Path(manifest["source"])):
        if path.exists() and (output == path.resolve() or output.exists() and output.samefile(path)):
            raise ValueError("table output aliases an input")
    changed, bodies = [], []
    for group, changes in zip(layout["groups"], edited["groups"]):
        if (not isinstance(changes, dict) or set(changes) != {"group", "lines"} or changes["group"] != group["group"]
                or not isinstance(changes["lines"], list) or len(changes["lines"]) != group["line_count"]):
            raise ValueError("table group identity or line count changed")
        texts, anchors = [], {}
        for entry, line in zip(group["lines"], changes["lines"]):
            text, locations = _line_edit(raw, group, entry, line,
                                         options=layout["kind"] == "bank" and group["group"] == 8)
            texts.append(text)
            for record, position in locations:
                if record in anchors and anchors[record] != position:
                    raise ValueError("shared native style record has conflicting translated anchor positions")
                anchors[record] = position
            start, end = entry["offset"], entry["offset"] + entry["size"]
            if text != raw[start:end] or any(_words(raw, at)[2] != value for at, value in locations):
                changed.append({"group": group["group"], "line": entry["line"]})
        if layout["kind"] == "bank":
            offset, _, length, padded = _words(raw, 16 + 16 * group["group"])
            limit = group["offset"] + length
            padding = raw[limit:group["offset"] + padded]
        else:
            limit, padding = len(raw), b""
        bodies.append((_rebuild_outer(raw, group, limit, texts, anchors), padding))
    if not changed:
        rebuilt = raw
    elif layout["kind"] == "outer":
        rebuilt = bodies[0][0]
    else:
        base, count, old_size, _ = _words(raw, 0)
        header, payload = bytearray(raw[:base]), bytearray()
        for index, (body, padding) in enumerate(bodies):
            offset, length = len(payload), len(body)
            body += padding
            body += bytes(-len(body) % 16)
            if offset + len(body) >= 1 << 32:
                raise ValueError("message bank exceeds native u32 offsets")
            struct.pack_into("<4I", header, 16 + index * 16, offset, offset // 16, length, len(body))
            payload.extend(body)
        struct.pack_into("<I", header, 8, len(payload))
        rebuilt = bytes(header) + bytes(payload) + raw[base + old_size:]
    # Resident archive leaves are addressed by 16-byte DMA units. Preserve the
    # source alignment when a bare OUTER or opaque trailing bytes change size.
    if changed and len(raw) % 16 == 0:
        rebuilt += bytes(-len(rebuilt) % 16)
    _layout(rebuilt, layout["kind"])
    output.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".table-", dir=output.parent)
    try:
        with os.fdopen(fd, "wb") as stream:
            stream.write(rebuilt)
        os.replace(temporary, output)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    return {"output": str(output), "sha256": hashlib.sha256(rebuilt).hexdigest(),
            "changed_lines": changed, "byte_identical": rebuilt == raw, "size": len(rebuilt)}
