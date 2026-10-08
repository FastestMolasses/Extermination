"""Fixed-length editing views for decoded message-bank OUTER/TEXT tables.

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


def unpack_table(source: Path, out_dir: Path, *, kind="auto") -> dict:
    """Export a bank or bare OUTER table to an editable, ordered JSON document."""
    source, output = Path(source).resolve(), safe_output(out_dir)
    if output.exists() and (not output.is_dir() or any(output.iterdir())):
        raise ValueError("table unpack destination must be new or empty")
    if source.is_relative_to(output):
        raise ValueError("table unpack destination contains its input")
    raw = source.read_bytes()
    layout = _layout(raw, kind)
    table = {"encoding": "latin-1", "groups": [
        {"group": group["group"], "lines": [
            {"line": entry["line"], "text": raw[entry["offset"]:entry["offset"] + entry["size"]].decode("latin-1")}
            for entry in group["lines"]]} for group in layout["groups"]]}
    output.mkdir(parents=True, exist_ok=True)
    (output / "original.bin").write_bytes(raw)
    (output / "table.json").write_text(json.dumps(table, indent=2, ensure_ascii=True) + "\n")
    manifest = {"schema": SCHEMA, "source": str(source), "layout": layout,
                "original_sha256": hashlib.sha256(raw).hexdigest(),
                "limits": "Existing lines only, identical encoded byte lengths. Native glyph bytes use Latin-1 mapping; markup is preserved."}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def pack_table(tree: Path, out_file: Path) -> dict:
    """Apply validated text edits without shifting native markup or table entries."""
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
    rebuilt = bytearray(raw)
    changed = []
    for group, changes in zip(layout["groups"], edited["groups"]):
        if (not isinstance(changes, dict) or set(changes) != {"group", "lines"} or changes["group"] != group["group"]
                or not isinstance(changes["lines"], list) or len(changes["lines"]) != group["line_count"]):
            raise ValueError("table group identity or line count changed")
        for entry, line in zip(group["lines"], changes["lines"]):
            if (not isinstance(line, dict) or set(line) != {"line", "text"}
                    or line["line"] != entry["line"] or not isinstance(line["text"], str)):
                raise ValueError("table line identity changed")
            try:
                text = line["text"].encode("latin-1")
            except UnicodeEncodeError as exc:
                raise ValueError("text must contain only native Latin-1 byte values") from exc
            if len(text) != entry["size"] or b"\0" in text:
                raise ValueError("edited text must keep its original byte length and contain no NUL")
            start, end = entry["offset"], entry["offset"] + entry["size"]
            if text != raw[start:end]:
                rebuilt[start:end] = text
                changed.append({"group": group["group"], "line": entry["line"]})
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
