"""Editable PNGs back to bounded native GS texture-upload packets.

Unknown palettes are never guessed: the default PNG stores PSMT8 indices as
gray values. Explicit TEX0 views (or audited startup presets) additionally
expose RGBA textures and CSM1 palettes. All source bytes outside actual edits
survive, including packet headers, unused palette entries and alignment.
"""
from __future__ import annotations

from array import array
from functools import lru_cache
import hashlib
import json
import os
from pathlib import Path
import struct
import sys
import tempfile

from .archive import safe_output
from . import png

TOOLS = Path(__file__).resolve().parents[1]
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))
from extract_textures import psmct32_word, psmt8_byte, psmt4_nibble  # noqa: E402
from export_startup import TITLE, MENU, LOGOS, palette_address  # noqa: E402

SCHEMA = "extermination-textures-v1"


def _hash(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _uploads(raw: bytes) -> list[dict]:
    uploads = []
    for pos in range(0, len(raw) - 80, 16):
        if struct.unpack_from("<Q", raw, pos + 8)[0] != 0x50:
            continue
        if any(struct.unpack_from("<Q", raw, pos + 16 * i + 8)[0] != reg
               for i, reg in ((1, 0x51), (2, 0x52), (3, 0x53))):
            continue
        tag = pos + 64
        if (raw[tag + 7] >> 2) & 3 != 2:
            continue
        bb, origin, shape, direction = (struct.unpack_from("<Q", raw, pos + 16 * i)[0]
                                        for i in range(4))
        width, height = shape & 0xfff, (shape >> 32) & 0xfff
        dbp, dbw, dpsm = (bb >> 32) & 0x3fff, (bb >> 48) & 63, (bb >> 56) & 63
        size = width * height * 4
        count = (struct.unpack_from("<H", raw, tag)[0] & 0x7fff) * 16
        if (not width or not height or width > 1024 or height > 1024 or width % 64 or height % 32
                or dpsm != 0 or dbw != width // 64 or origin != 0 or direction != 0):
            raise ValueError("unsupported GS transfer: requires page-aligned PSMCT32, matching DBW, origin zero and host-to-local")
        if count != size or tag + 16 + size > len(raw):
            raise ValueError("GS IMAGE payload is truncated or its size disagrees with TRXREG")
        entry = dict(number=len(uploads), bitblt_offset=pos, payload_offset=tag + 16,
                     payload_size=size, width=width, height=height, dbp=dbp, dbw=dbw)
        if uploads and entry["payload_offset"] < uploads[-1]["payload_offset"] + uploads[-1]["payload_size"]:
            raise ValueError("overlapping GS IMAGE payloads")
        uploads.append(entry)
    return uploads


def inspect(source: Path) -> dict:
    raw = Path(source).read_bytes()
    return dict(schema=SCHEMA, source_size=len(raw), source_sha256=_hash(raw), uploads=_uploads(raw))


@lru_cache(maxsize=4)
def _permutation(width: int, height: int, dbw: int) -> array:
    """Map each logical PSMT8 pixel to its source transfer byte."""
    words = {psmct32_word(x, y, dbw): (y * width + x) * 4
             for y in range(height) for x in range(width)}
    mapping = array("I")
    for y in range(height * 2):
        for x in range(width * 2):
            address = psmt8_byte(x, y, dbw)
            if address // 4 not in words:
                raise ValueError("PSMT8 view does not fully cover the transfer")
            mapping.append(words[address // 4] + address % 4)
    if len(set(mapping)) != width * height * 4:
        raise ValueError("transfer swizzle is not a complete byte permutation")
    return mapping


def _words(uploads: list[dict]) -> dict[int, int]:
    result = {}
    for upload in uploads:
        width, height = upload["width"], upload["height"]
        for y in range(height):
            for x in range(width):
                address = upload["dbp"] * 64 + psmct32_word(x, y, upload["dbw"])
                if address >= 1024 * 1024:
                    raise ValueError("GS transfer exceeds 4 MiB local memory")
                result[address] = upload["payload_offset"] + (y * width + x) * 4
    return result


def _native(words: dict[int, int], address: int) -> int:
    if address // 4 not in words:
        raise ValueError("TEX0 texture or palette reads outside supplied uploads")
    return words[address // 4] + address % 4


def _fields(tex0: int) -> dict:
    if not 0 <= tex0 < 1 << 64:
        raise ValueError("TEX0 must be a u64")
    psm, bw = (tex0 >> 20) & 63, (tex0 >> 14) & 63
    width, height = 1 << ((tex0 >> 26) & 15), 1 << ((tex0 >> 30) & 15)
    if (psm not in (0x13, 0x14) or (tex0 >> 51) & 0x3ff or not bw or bw % 2
            or width > 1024 or height > 1024):
        raise ValueError("TEX0 requires PSMT8/PSMT4, even TBW and PSMCT32 CSM1/CSA0 palette")
    return dict(tbp=tex0 & 16383, tbw=bw, psm=psm, cbp=(tex0 >> 37) & 16383,
                width=width, height=height)


def _view_map(words: dict[int, int], tex0: int, flip_y: bool):
    fields = _fields(tex0)
    result = []
    for y in range(fields["height"]):
        row = fields["height"] - 1 - y if flip_y else y
        for x in range(fields["width"]):
            if fields["psm"] == 0x13:
                address = fields["tbp"] * 256 + psmt8_byte(x, row, fields["tbw"] // 2)
                result.append((_native(words, address), 255, 0))
            else:
                nibble = fields["tbp"] * 512 + psmt4_nibble(x, row, fields["tbw"] // 2)
                shift = (nibble & 1) * 4
                result.append((_native(words, nibble // 2), 15 << shift, shift))
    return fields, result


def _palette_map(words: dict[int, int], cbp: int, psm: int) -> list[int]:
    return [_native(words, palette_address(cbp, i, psm)) for i in range(256 if psm == 0x13 else 16)]


def _palette(raw: bytes, mapping: list[int]) -> list[bytes]:
    return [bytes(raw[at:at + 3]) + bytes((min(255, raw[at + 3] * 2),)) for at in mapping]


def _rgba(raw: bytes, mapping, palette: list[bytes]) -> bytes:
    return b"".join(palette[(raw[at] & mask) >> shift] for at, mask, shift in mapping)


def _input(tree: Path, relative: str) -> Path:
    path = Path(relative)
    if path.is_absolute() or ".." in path.parts:
        raise ValueError("unsafe texture manifest path")
    target = (tree / path).resolve()
    if not target.is_relative_to(tree.resolve()) or not target.is_file():
        raise ValueError(f"texture input is absent or escapes its tree: {relative}")
    return target


def unpack(source: Path, out_dir: Path, *, tex0: list[int] | None = None, preset: str | None = None) -> dict:
    """Export index sheets and optional true-color views without guessing palettes."""
    source, out_dir = Path(source), safe_output(out_dir)
    if out_dir.exists() and any(out_dir.iterdir()):
        raise ValueError("texture unpack destination must be empty")
    raw = source.read_bytes()
    uploads = _uploads(raw)
    if not uploads:
        raise ValueError("no supported bounded GS texture upload found")
    tokens = list(tex0 or [])
    if preset == "title":
        tokens = list(TITLE) + [token for row in MENU for token in row] + tokens
    elif preset == "warning":
        tokens = list(LOGOS[0][1]) + tokens
    elif preset == "logos":
        tokens = list(LOGOS[1][1]) + list(LOGOS[2][1]) + tokens
    elif preset is not None:
        raise ValueError("texture preset must be title, warning or logos")
    tokens = list(dict.fromkeys(tokens))
    words = _words(uploads) if tokens else None
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "template.bin").write_bytes(raw)
    manifest = dict(schema=SCHEMA, source_size=len(raw), source_sha256=_hash(raw), source_path=str(source.resolve()),
                    preset=preset, uploads=uploads, views=[], palettes=[])
    for upload in uploads:
        mapping = _permutation(upload["width"], upload["height"], upload["dbw"])
        relative = f"upload{upload['number']:03d}.indices.png"
        indices = bytes(raw[upload["payload_offset"] + at] for at in mapping)
        png.write(out_dir / relative, upload["width"] * 2, upload["height"] * 2, indices, mode="L")
    palette_names = {}
    for token in tokens:
        fields, mapping = _view_map(words, token, bool(preset))
        key = (fields["cbp"], fields["psm"])
        if key not in palette_names:
            name = f"palette{len(palette_names):03d}.png"
            palette_names[key] = name
            pm = _palette_map(words, *key)
            palette = _palette(raw, pm)
            width, height = (16, 16) if fields["psm"] == 0x13 else (8, 2)
            png.write(out_dir / name, width, height, b"".join(palette))
            manifest["palettes"].append(dict(path=name, cbp=key[0], psm=key[1], width=width, height=height))
        palette = _palette(raw, _palette_map(words, *key))
        relative = f"texture{len(manifest['views']):03d}.png"
        png.write(out_dir / relative, fields["width"], fields["height"], _rgba(raw, mapping, palette))
        manifest["views"].append(dict(path=relative, tex0=token, palette=palette_names[key], flip_y=bool(preset), **fields))
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def _relayout_uploads(raw: bytes, uploads: list[dict], sheets: list[tuple[int, int, bytes]]) -> bytes:
    """Rebuild the audited contiguous CNT/END, VIF DIRECT, GIF IMAGE chain.

    This resizes physical transfer canvases. It does not invent or rewrite
    logical TEX0 consumers in executable constants or other archive leaves.
    """
    packets, cursor, ranges = [], 0, []
    for number, (upload, (width, height, indices)) in enumerate(zip(uploads, sheets)):
        start = upload["bitblt_offset"] - 48
        count = upload["payload_size"] // 16
        dma_kind = 6 if number == len(uploads) - 1 else 1
        expected = (dma_kind << 28 | (count + 7), 0, 0, 0, 0, 0, 0x10000000, 0x50000000 | (count + 6))
        if (start != cursor or struct.unpack_from("<8I", raw, start) != expected
                or struct.unpack_from("<QQ", raw, start + 32) != (0x1000000000000004, 14)):
            raise ValueError("upload resize requires the audited contiguous CNT/END + VIF DIRECT + four-register GIF envelope; unknown pointers cannot relocate")
        cursor = upload["payload_offset"] + upload["payload_size"]
        if width % 128 or height % 64 or not 128 <= width <= 2048 or not 64 <= height <= 2048:
            raise ValueError("indexed upload canvas must have width a multiple of 128 and height a multiple of 64, within 2048: page-aligned PSMCT32 transfer geometry")
        tw, th = width // 2, height // 2
        size = tw * th * 4
        count = size // 16
        if count > 0x7FFF:
            raise ValueError("upload exceeds the GIF IMAGE 15-bit NLOOP limit (32767 quadwords); packet splitting is not supported")
        if count + 7 > 0xFFFF or count + 6 > 0xFFFF:
            raise ValueError("upload exceeds the DMA QWC/VIF DIRECT 16-bit count limit")
        base, end = upload["dbp"] * 256, upload["dbp"] * 256 + size
        if end > 4 * 1024 * 1024:
            raise ValueError("resized upload exceeds the GS 4 MiB local-memory limit")
        ranges.append((base, end))
        header = bytearray(raw[start:upload["payload_offset"]])
        struct.pack_into("<H", header, 0, count + 7)
        struct.pack_into("<H", header, 28, count + 6)
        bb = struct.unpack_from("<Q", header, 48)[0]
        struct.pack_into("<Q", header, 48, (bb & ~(63 << 48)) | (tw // 64) << 48)
        shape = struct.unpack_from("<Q", header, 80)[0]
        struct.pack_into("<Q", header, 80, (shape & ~(0xFFF | 0xFFF << 32)) | tw | th << 32)
        image = struct.unpack_from("<H", header, 112)[0]
        struct.pack_into("<H", header, 112, (image & 0x8000) | count)
        payload = bytearray(size)
        for value, native in zip(indices, _permutation(tw, th, tw // 64)):
            payload[native] = value
        packets.append(bytes(header) + payload)
    if any(raw[cursor:]):
        raise ValueError("upload resize cannot relocate unknown nonzero data after the DMA chain")
    for i, (base, end) in enumerate(ranges):
        for j, (other_base, other_end) in enumerate(ranges[:i]):
            if max(base, other_base) < min(end, other_end):
                old_a, old_b = uploads[i], uploads[j]
                old_start = max(old_a["dbp"], old_b["dbp"]) * 256
                old_end = min(old_a["dbp"] * 256 + old_a["payload_size"],
                              old_b["dbp"] * 256 + old_b["payload_size"])
                if max(base, other_base) < old_start or min(end, other_end) > old_end:
                    raise ValueError("resized upload introduces a GS memory overlap with another transfer in this leaf")
    # Payloads are page multiples, so preserving the exact old padding also
    # preserves the native leaf's sector/DMA alignment after growing/shrinking.
    rebuilt = b"".join(packets) + raw[cursor:]
    parsed = _uploads(rebuilt)
    if len(parsed) != len(uploads):
        raise ValueError("resized DMA chain failed transfer-count verification")
    for entry, (width, height, indices) in zip(parsed, sheets):
        mapping = _permutation(entry["width"], entry["height"], entry["dbw"])
        if bytes(rebuilt[entry["payload_offset"] + at] for at in mapping) != indices:
            raise ValueError("resized upload failed native index verification")
    return rebuilt


def _atomic_native(output: Path, rebuilt: bytes, inputs: list[Path]) -> None:
    for source in inputs:
        if source.exists() and (output == source.resolve() or output.exists() and output.samefile(source)):
            raise ValueError("texture output aliases an input")
    output.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".texture-", dir=output.parent)
    try:
        with os.fdopen(fd, "wb") as dst:
            dst.write(rebuilt)
        os.replace(temporary, output)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def _resize_from_pngs(tree: Path, output: Path, manifest: dict, raw: bytes, inputs: list[Path]):
    sheets, changes, resized = [], [], []
    for upload in manifest["uploads"]:
        name = f"upload{upload['number']:03d}.indices.png"
        path = _input(tree, name)
        inputs.append(path)
        width, height, rgba = png.read(path)
        if any(r != g or r != b or a != 255 for r, g, b, a in zip(rgba[::4], rgba[1::4], rgba[2::4], rgba[3::4])):
            raise ValueError(f"{name}: unknown-CLUT sheets store opaque gray index values, not colors")
        indices = rgba[::4]
        sheets.append((width, height, indices))
        original_shape = (upload["width"] * 2, upload["height"] * 2)
        if (width, height) != original_shape:
            resized.append(dict(upload=upload["number"], before=list(original_shape), after=[width, height]))
            changes.append(name)
        else:
            mapping = _permutation(upload["width"], upload["height"], upload["dbw"])
            before = bytes(raw[upload["payload_offset"] + at] for at in mapping)
            if indices != before:
                changes.append(name)
    if not resized:
        return None
    if manifest["views"] or manifest["palettes"] or manifest.get("preset"):
        raise ValueError("physical upload-canvas resize requires an index-only unpack; logical TEX0 views/palettes have external references. "
                         "Startup TEX0 constants live in func_001AB9D0/001ABC60/001ABE10/001AC7F0; actor GS residency crosses leaves")
    rebuilt = _relayout_uploads(raw, manifest["uploads"], sheets)
    _atomic_native(output, rebuilt, inputs)
    return dict(path=str(output), sha256=_hash(rebuilt), unchanged=False, size=len(rebuilt),
                original_size=len(raw), resized_uploads=resized, changed_pngs=changes,
                quantization="indices", quantized_pixels=0, maximum_channel_error=0,
                scope="physical transfer canvases only; external TEX0/model/ELF references are unchanged and game residency is not validated")


def pack(tree: Path, out_native: Path, *, quantize: str = "exact", resize_uploads: bool = False) -> dict:
    """Apply PNG edits to native bytes; reject conflicting overlapping views.

    ``exact`` requires colors already present in the edited palette and PS2-
    representable alpha. ``nearest`` uses nearest RGBA palette color and reports
    quantized pixels/error. Unchanged PNG pixels retain their original indices.
    ``resize_uploads`` permits index-only physical transfer canvas changes in
    audited DMA chains; it does not change logical texture/palette sizes or
    certify the game's cross-leaf GS residency.
    """
    tree, output = Path(tree), safe_output(out_native)
    if quantize not in ("exact", "nearest"):
        raise ValueError("quantize must be exact or nearest")
    if Path(out_native).is_symlink() or output.is_relative_to(tree.resolve()):
        raise ValueError("texture output must be separate from its input tree and not a symlink")
    manifest = json.loads(_input(tree, "manifest.json").read_text())
    raw = _input(tree, "template.bin").read_bytes()
    if (manifest.get("schema") != SCHEMA or _hash(raw) != manifest["source_sha256"] or
            len(raw) != manifest["source_size"] or _uploads(raw) != manifest["uploads"]):
        raise ValueError("texture template or upload manifest changed")
    inputs = [_input(tree, "manifest.json"), _input(tree, "template.bin")]
    original = Path(manifest["source_path"])
    if original.exists():
        inputs.append(original)
    if resize_uploads:
        resized = _resize_from_pngs(tree, output, manifest, raw, inputs)
        if resized is not None:
            return resized
    patches, changed_pngs = {}, []
    quantized_pixels, maximum_error = 0, 0

    def apply(at: int, value: int, mask: int = 255):
        previous_mask, previous_value = patches.get(at, (0, 0))
        if (previous_value ^ value) & previous_mask & mask:
            raise ValueError("conflicting texture/palette PNG edits address the same native bits")
        patches[at] = (previous_mask | mask, (previous_value & ~mask) | (value & mask))

    def pixels(relative: str, width: int, height: int) -> bytes:
        path = _input(tree, relative)
        inputs.append(path)
        w, h, rgba = png.read(path)
        if (w, h) != (width, height):
            if relative.endswith(".indices.png"):
                raise ValueError(f"{relative}: dimensions changed; enable resize_uploads for audited physical transfer-canvas relayout")
            if relative.startswith("palette"):
                raise ValueError(f"{relative}: palette size change needs all TEX0 PSM/CBP references and GS allocations; "
                                 "actor cross-leaf residency and executable references are not closed")
            raise ValueError(f"{relative}: logical texture dimensions require relocating all TEX0 references; "
                             "startup constants are in func_001AB9D0/001ABC60/001ABE10/001AC7F0 and actor material/GS references cross archive leaves. "
                             "The asset-only input does not contain their complete reference set")
        return rgba

    for upload in manifest["uploads"]:
        name = f"upload{upload['number']:03d}.indices.png"
        rgba = pixels(name, upload["width"] * 2, upload["height"] * 2)
        mapping = _permutation(upload["width"], upload["height"], upload["dbw"])
        changed = False
        for index, at in enumerate(mapping):
            r, g, b, alpha = rgba[index * 4:index * 4 + 4]
            if r != g or r != b or alpha != 255:
                raise ValueError(f"{name}: unknown-CLUT sheets store opaque gray index values, not colors")
            at += upload["payload_offset"]
            if raw[at] != r:
                apply(at, r)
                changed = True
        if changed:
            changed_pngs.append(name)
    words = _words(manifest["uploads"]) if manifest["views"] or manifest["palettes"] else None
    for entry in manifest["palettes"]:
        mapping = _palette_map(words, entry["cbp"], entry["psm"])
        want = pixels(entry["path"], entry["width"], entry["height"])
        before = b"".join(_palette(raw, mapping))
        if want == before:
            continue
        changed_pngs.append(entry["path"])
        for i, at in enumerate(mapping):
            color, old = want[i * 4:i * 4 + 4], before[i * 4:i * 4 + 4]
            for channel in range(3):
                if color[channel] != old[channel]:
                    apply(at + channel, color[channel])
            if color[3] != old[3]:
                alpha = min(128, (color[3] + 1) // 2)
                expanded = min(255, alpha * 2)
                if expanded != color[3]:
                    if quantize == "exact":
                        raise ValueError("palette alpha must be even in 0..254 or 255; use nearest to quantize")
                    quantized_pixels += 1
                    maximum_error = max(maximum_error, abs(expanded - color[3]))
                apply(at + 3, alpha)
    palette_raw = bytearray(raw)
    for at, (mask, value) in patches.items():
        palette_raw[at] = (palette_raw[at] & ~mask) | value
    for view in manifest["views"]:
        fields, mapping = _view_map(words, view["tex0"], view["flip_y"])
        want = pixels(view["path"], fields["width"], fields["height"])
        pm = _palette_map(words, fields["cbp"], fields["psm"])
        before = _rgba(raw, mapping, _palette(raw, pm))
        if want == before:
            continue
        changed_pngs.append(view["path"])
        palette = _palette(palette_raw, pm)
        lookup = {}
        for i, color in reversed(list(enumerate(palette))):
            lookup[color] = i
        nearest_cache = {}
        for i, (at, mask, shift) in enumerate(mapping):
            color = want[i * 4:i * 4 + 4]
            if color == before[i * 4:i * 4 + 4]:
                continue
            old_index = (raw[at] & mask) >> shift
            if palette[old_index] == color:
                index = old_index
            elif color in lookup:
                index = lookup[color]
            elif quantize == "exact":
                raise ValueError(f"{view['path']}: edited RGBA color is absent from its palette; edit palette PNG or use nearest")
            else:
                if color not in nearest_cache:
                    nearest_cache[color] = min(range(len(palette)), key=lambda j:
                        sum((a - b) ** 2 for a, b in zip(color, palette[j])))
                index = nearest_cache[color]
                quantized_pixels += 1
                maximum_error = max(maximum_error, max(abs(a - b) for a, b in zip(color, palette[index])))
            apply(at, index << shift, mask)
    rebuilt = bytearray(raw)
    for at, (mask, value) in patches.items():
        rebuilt[at] = (rebuilt[at] & ~mask) | value
    _atomic_native(output, rebuilt, inputs)
    return dict(path=str(output), sha256=_hash(rebuilt), unchanged=rebuilt == raw,
                size=len(rebuilt), changed_native_bytes=sum(a != b for a, b in zip(raw, rebuilt)),
                changed_pngs=changed_pngs, quantization=quantize, quantized_pixels=quantized_pixels,
                maximum_channel_error=maximum_error)
