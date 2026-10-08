"""Deterministic BYO-disc mod packs: base-dependent deltas and declared new work.

No original templates, native manifests or executable copies enter the pack.
Only recognized build instructions can affect the freshly compiled executable.
"""
from __future__ import annotations

import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import tempfile
import zipfile

from . import archive, iso, streams

SCHEMA = "extermination-modpack-v1"
SERIAL = "SCUS-97112"
MAX_PACK = 1 << 30
MAX_MANIFEST = 4 << 20
MAX_CHANGES = 4096
MAX_OUTPUT = 1 << 30
INSTRUCTIONS = {"texture-upgrades.json", "stream-edits.json"}


def digest(raw: bytes) -> str:
    return hashlib.sha256(raw).hexdigest()


def _json(raw: bytes):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            if key in result:
                raise ValueError(f"duplicate JSON key: {key}")
            result[key] = value
        return result
    return json.loads(raw, object_pairs_hook=unique)


def _canonical(value) -> bytes:
    return (json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=True) + "\n").encode()


def _keys(value, keys, label):
    if not isinstance(value, dict) or set(value) != set(keys):
        raise ValueError(f"invalid {label} fields")


def _sha(value):
    if not isinstance(value, str) or not re.fullmatch(r"[0-9a-f]{64}", value):
        raise ValueError("invalid SHA-256 identity")


def _relative(value):
    if (not isinstance(value, str) or not value or "\\" in value
            or value != str(PurePosixPath(value)) or PurePosixPath(value).is_absolute()
            or any(p in (".", "..") for p in value.split("/"))):
        raise ValueError("unsafe or noncanonical relative mod path")
    return value


def _read(image: Path, offset: int, size: int) -> bytes:
    with image.open("rb") as stream:
        stream.seek(offset)
        raw = stream.read(size)
    if len(raw) != size:
        raise ValueError("original disc changed or was truncated")
    return raw


class BaseDisc:
    """Resolve corrected native leaves directly from the user's original ISO."""
    def __init__(self, image: Path):
        self.image = Path(image).resolve()
        self.info = iso.inventory(self.image)
        self.iso_files = {row["path"]: row for row in self.info["files"]}
        for name in ("DATA/DATA.DAT", "DATA/INDEX.IDX", "SCUS_971.12"):
            if name not in self.iso_files:
                raise ValueError(f"base disc is missing {name}")
        data, index = self.iso_files["DATA/DATA.DAT"], self.iso_files["DATA/INDEX.IDX"]
        self.index = _read(self.image, index["offset"], index["size"])
        self.regions, self.layout = archive.parse_index(self.index, data["size"])
        self.targets = {}
        for region in self.regions:
            for file in region["files"]:
                self.targets["archive/" + file["path"]] = dict(file,
                    disc_offset=data["offset"] + region["offset"] + file["offset"])
        for span in self.layout:
            if span["kind"] == "gap":
                self.targets["archive/" + span["path"]] = dict(span, disc_offset=data["offset"] + span["offset"])
        for name, row in self.iso_files.items():
            self.targets["iso/" + name] = dict(row, disc_offset=row["offset"])

    def read(self, target):
        row = self.targets[target]
        return _read(self.image, row["disc_offset"], row["size"])

    def hash(self, target):
        row = self.targets[target]
        if "sha256" not in row:
            row["sha256"] = digest(self.read(target))
        return row["sha256"]

    def editable(self, target):
        if target not in self.targets:
            raise ValueError(f"unknown original loose target: {target}")
        if (target in ("iso/DATA/DATA.DAT", "iso/DATA/INDEX.IDX", "iso/SCUS_971.12")
                or target.startswith("iso/OVERLAY/")):
            raise ValueError(f"{target}: edit archive leaves or recognized build instructions; source outputs cannot be replaced")


def _tree_file(tree, target):
    _relative(target)
    relative = "iso/files/" + target[4:] if target.startswith("iso/") else target
    return archive._input(tree, relative)


def _validate_tree(base, tree):
    """Reject altered templates rather than accidentally distributing/ignoring them."""
    manifest = _json(archive._input(tree, "iso/manifest.json").read_bytes())
    if {k: v for k, v in manifest.items() if k not in ("spans", "source_image")} != base.info:
        raise ValueError("edited tree ISO manifest does not describe this original disc")
    for span in manifest["spans"]:
        raw = archive._input(tree / "iso", span["path"]).read_bytes()
        if digest(raw) != span["sha256"] or raw != _read(base.image, span["offset"], span["size"]):
            raise ValueError("ISO metadata templates must remain original")
    native = _json(archive._input(tree, "archive/manifest.json").read_bytes())
    if (native.get("schema") != archive.SCHEMA or native.get("index_template") != "index.template.bin"
            or native.get("data_size") != base.iso_files["DATA/DATA.DAT"]["size"]
            or native.get("index_sha256") != digest(base.index)
            or archive._without_hashes(native.get("regions")) != base.regions
            or archive._without_hashes(native.get("data_layout")) != base.layout
            or archive._input(tree, "archive/index.template.bin").read_bytes() != base.index):
        raise ValueError("archive templates must describe the original disc")
    additions = archive._additions(tree / "archive", base.regions, base.index)[0]
    expected = {"iso/manifest.json", "archive/manifest.json", "archive/index.template.bin", "archive/edits.json", *INSTRUCTIONS}
    expected.update("iso/files/" + name for name in base.iso_files)
    expected.update(target for target in base.targets if target.startswith("archive/"))
    expected.update("iso/" + span["path"] for span in manifest["spans"])
    expected.update("archive/" + row["path"] for values in additions.values() for row in values)
    extras = [str(p.relative_to(tree)) for p in tree.rglob("*") if (p.is_file() or p.is_symlink()) and str(p.relative_to(tree)) not in expected]
    if extras:
        raise ValueError("unrecognized loose-tree files (keep editor projects outside it): " + ", ".join(sorted(extras)))
    return additions


def _zip_bytes(manifest, payloads):
    output = io.BytesIO()
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_STORED, allowZip64=False) as package:
        members = {"manifest.json": _canonical(manifest), **payloads}
        for name, raw in sorted(members.items()):
            info = zipfile.ZipInfo(name, (1980, 1, 1, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o600) << 16
            package.writestr(info, raw)
    return output.getvalue()


def _load(pack):
    try:
        return _load_checked(pack)
    except (zipfile.BadZipFile, zipfile.LargeZipFile, UnicodeError) as exc:
        raise ValueError("invalid mod-pack ZIP or text encoding") from exc


def _load_checked(pack):
    pack = Path(pack).resolve()
    if pack.stat().st_size > MAX_PACK:
        raise ValueError("mod pack exceeds the 1 GiB distribution limit")
    raw = pack.read_bytes()
    with zipfile.ZipFile(io.BytesIO(raw)) as package:
        members, infos = {}, package.infolist()
        if len(infos) > MAX_CHANGES + 1 or package.comment:
            raise ValueError("too many ZIP members or unsupported archive comment")
        total = 0
        for info in infos:
            _relative(info.filename)
            if (info.filename in members or info.is_dir() or info.compress_type != zipfile.ZIP_STORED
                    or info.extra or info.comment or info.flag_bits & 1):
                raise ValueError("duplicate, compressed, decorated or unsupported ZIP member")
            total += info.file_size
            if total > MAX_PACK or info.file_size > MAX_PACK:
                raise ValueError("unpacked mod payload exceeds its limit")
            members[info.filename] = package.read(info)
    encoded = members.pop("manifest.json", None)
    if encoded is None or len(encoded) > MAX_MANIFEST:
        raise ValueError("missing or oversized mod manifest")
    manifest = _json(encoded)
    _keys(manifest, ("schema", "base", "changes", "instructions"), "mod manifest")
    if manifest["schema"] != SCHEMA:
        raise ValueError("unsupported mod-pack schema")
    _keys(manifest["base"], ("serial", "iso_sha256"), "base identity")
    if manifest["base"]["serial"] != SERIAL:
        raise ValueError("unsupported disc serial")
    _sha(manifest["base"]["iso_sha256"])
    if not isinstance(manifest["changes"], list) or not 1 <= len(manifest["changes"]) <= MAX_CHANGES:
        raise ValueError("mod pack must contain 1..4096 changed files")
    if not isinstance(manifest["instructions"], dict) or set(manifest["instructions"]) - INSTRUCTIONS:
        raise ValueError("unknown executable/build instructions")
    used, targets = set(), set()
    for number, change in enumerate(manifest["changes"]):
        _keys(change, ("target", "base_sha256", "result_sha256", "result_size", "encoding",
                       "payload", "payload_sha256", "created_from_scratch"), "file change")
        target = _relative(change["target"])
        if target in targets:
            raise ValueError(f"duplicate touched target: {target}")
        targets.add(target)
        if change["base_sha256"] is not None:
            _sha(change["base_sha256"])
        _sha(change["result_sha256"]); _sha(change["payload_sha256"])
        if type(change["result_size"]) is not int or not 0 <= change["result_size"] < MAX_OUTPUT:
            raise ValueError("invalid result size (must be below 1 GiB)")
        encoding = change["encoding"]
        if encoding not in ("delta-v1", "full") or type(change["created_from_scratch"]) is not bool:
            raise ValueError("unknown content encoding or authorship declaration")
        if encoding == "full" and not change["created_from_scratch"]:
            raise ValueError("FULL content requires an explicit created-from-scratch declaration")
        if encoding == "delta-v1" and (change["base_sha256"] is None or change["created_from_scratch"]):
            raise ValueError("delta requires original bytes and cannot claim FULL authorship")
        expected = f"changes/{number:04d}." + ("delta" if encoding == "delta-v1" else "full")
        if change["payload"] != expected or expected not in members:
            raise ValueError("missing or noncanonical payload member")
        used.add(expected)
        if digest(members[expected]) != change["payload_sha256"]:
            raise ValueError("mod payload hash mismatch")
    if set(members) != used:
        raise ValueError("unreferenced content in mod pack")
    if encoded != _canonical(manifest) or raw != _zip_bytes(manifest, members):
        raise ValueError("noncanonical mod container: comments, hidden bytes and alternate encodings are forbidden")
    return manifest, members, raw


def _content_channels(manifest, payloads, raw):
    """Include ZIP framing and all content except the typed delta instructions."""
    from . import delta
    excluded, residuals = [], []
    with zipfile.ZipFile(io.BytesIO(raw)) as package:
        for change in manifest["changes"]:
            if change["encoding"] == "delta-v1":
                info = package.getinfo(change["payload"])
                start = info.header_offset + 30 + len(info.filename.encode())
                # Keep residual bytes in their real ZIP positions. Excluding
                # a whole member would miss a run crossing its payload/header
                # boundary. Only the validated instruction fields are exempt.
                cursor = 0
                for low, high in delta.distributable_payload_ranges(payloads[change["payload"]]):
                    excluded.append((start + cursor, start + low))
                    cursor = high
                excluded.append((start + cursor, start + info.file_size))
                residuals.extend(delta.distributable_payloads(payloads[change["payload"]]))
    channels, cursor = [], 0
    for start, end in sorted(excluded):
        channels.append(raw[cursor:start]); cursor = end
    channels.append(raw[cursor:])
    # Also check content in logical member order: splitting an original run
    # across FULL and delta payload members must not evade the scan.
    semantic = [_canonical(manifest)]
    for change in manifest["changes"]:
        data = payloads[change["payload"]]
        semantic.extend(delta.distributable_payloads(data) if change["encoding"] == "delta-v1" else [data])
    return channels + residuals + semantic


def _scan(base, manifest, payloads, raw):
    from .original_scan import scan_original_runs
    match = scan_original_runs(base.image, _content_channels(manifest, payloads, raw))
    if match is not None:
        raise ValueError("mod pack contains a run of at least 65 original bytes outside delta instructions; "
                         "use base-dependent deltas or genuinely new content")


def _addition(target):
    match = re.fullmatch(r"add/(chunk[0-9]{2}(?:\.n[0-9]+)?)/([0-9a-f]{2})", target)
    if match is None:
        raise ValueError("new targets must identify an archive resident region and role ID")
    return match[1], int(match[2], 16)


def _reconstruct(base, change, payload):
    from . import delta
    target = change["target"]
    if target.startswith("add/"):
        region, ident = _addition(target)
        original = next((row for row in base.regions if row["label"] == region), None)
        if (original is None or not original["entries"] or any(row["id"] == ident for row in original["entries"])
                or change["base_sha256"] is not None or change["encoding"] != "full" or not change["result_size"]):
            raise ValueError("invalid new resident role or missing authored FULL content")
    else:
        base.editable(target)
        if change["base_sha256"] != base.hash(target):
            raise ValueError(f"original loose-file hash mismatch: {target}")
    rebuilt = delta.apply(base.read(target), payload) if change["encoding"] == "delta-v1" else payload
    if len(rebuilt) != change["result_size"] or digest(rebuilt) != change["result_sha256"]:
        raise ValueError(f"reconstructed result hash/size mismatch: {target}")
    return rebuilt


def _validate_pack(base, pack):
    manifest, payloads, raw = _load(pack)
    if manifest["base"]["iso_sha256"] != base.info["image_sha256"]:
        raise ValueError("mod pack requires a different original ISO SHA-256")
    for change in manifest["changes"]:
        _reconstruct(base, change, payloads[change["payload"]])
    _scan(base, manifest, payloads, raw)
    return manifest, payloads, digest(raw)


def _preflight_instructions(tree):
    from . import disc_edits
    disc_edits.validate_tree(tree)
    if (tree / "texture-upgrades.json").exists():
        from . import texture_upgrade
        texture_upgrade.validate_tree(tree)


def _preflight_layout(base, tree):
    """Reject invalid native/ISO size changes before paying for compilation."""
    _preflight_instructions(tree)
    from . import disc_edits
    kinds = {p["kind"] for p in disc_edits.validate_tree(tree)}
    for name, row in base.iso_files.items():
        if name in ("DATA/DATA.DAT", "DATA/INDEX.IDX"):
            continue
        current = _tree_file(tree, "iso/" + name)
        if current.stat().st_size == row["size"]:
            continue
        kind = next((kind for kind, profile in streams.PROFILES.items() if name == "STREAM/" + profile["filename"]), None)
        if kind not in kinds:
            raise ValueError(f"unsupported ISO file resize or missing paired stream instructions: {name}")
    with tempfile.TemporaryDirectory(prefix="mod-layout-", dir=archive.OUTPUT_ROOT) as temporary:
        output = Path(temporary)
        archive.pack_archive(tree / "archive", output / "DATA.DAT", output / "INDEX.IDX")


def make_modpack(image, tree, out, *, authored=(), stream_bundles=()):
    from . import delta, disc_edits
    if Path(out).is_symlink():
        raise ValueError("mod pack output must not be a symlink")
    tree, out = Path(tree).resolve(), archive.safe_output(out)
    base = BaseDisc(image)
    archive._distinct_outputs([out], [base.image, *(p for p in tree.rglob("*") if p.is_file())])
    for bundle in stream_bundles:
        bundle = Path(bundle).resolve()
        if out.is_relative_to(bundle):
            raise ValueError("mod pack output overlaps a stream bundle input")
        archive._distinct_outputs([out], [p for p in bundle.rglob("*") if p.is_file()])
    if out == base.image or (out.exists() and out.samefile(base.image)) or out.is_relative_to(tree):
        raise ValueError("mod pack output must be separate from original disc and edited tree")
    additions = _validate_tree(base, tree)
    _preflight_layout(base, tree)
    authored, consumed = {_relative(value) for value in authored}, set()
    changes, payloads = [], {}
    instructions = {name: _json(archive._input(tree, name).read_bytes()) for name in sorted(INSTRUCTIONS) if (tree / name).exists()}
    overrides = {}
    for bundle in stream_bundles:
        item = streams.inspect_bundle(bundle, tree / "iso/files/SCUS_971.12")
        target = "iso/STREAM/" + item["stream_file"]
        if target in overrides or item["kind"] in instructions.get("stream-edits.json", {}).get("lengths", {}):
            raise ValueError("duplicate stream edit source")
        if archive.sha256_file(_tree_file(tree, target)) != base.hash(target):
            raise ValueError("explicit bundle requires its original loose stream; do not combine two edit sources")
        if item["original_stream_sha256"] != base.hash(target):
            raise ValueError("stream bundle does not belong to the original disc")
        overrides[target] = Path(item["stream_path"])
        lengths = disc_edits.lengths_from_bundle(Path(bundle), tree / "iso/files/SCUS_971.12")
        if any(lengths.values()):
            metadata = instructions.setdefault("stream-edits.json", {"schema": "extermination-stream-edits-v1", "lengths": {}})
            metadata["lengths"].update(lengths)

    def add(target, source, old_hash, authored_path):
        rebuilt = source.read_bytes()
        if len(rebuilt) >= MAX_OUTPUT:
            raise ValueError("mod result must be below 1 GiB")
        full = authored_path in authored
        if full:
            consumed.add(authored_path)
        if target.startswith("add/") and not full:
            raise ValueError(f"new entry {authored_path} requires --authored and created-from-scratch content")
        payload = rebuilt if full else delta.encode(base.read(target), rebuilt)
        encoding = "full" if full else "delta-v1"
        member = f"changes/{len(changes):04d}." + ("full" if full else "delta")
        payloads[member] = payload
        changes.append(dict(target=target, base_sha256=old_hash, result_sha256=digest(rebuilt),
                            result_size=len(rebuilt), encoding=encoding, payload=member,
                            payload_sha256=digest(payload), created_from_scratch=full))

    for target in sorted(base.targets):
        source = overrides.get(target, _tree_file(tree, target))
        original_hash = base.hash(target)
        if archive.sha256_file(source) != original_hash:
            base.editable(target)
            add(target, source, original_hash, target)
    for region, values in additions.items():
        for value in values:
            target = f"add/{region}/{value['id']:02x}"
            relative = "archive/" + value["path"]
            add(target, archive._input(tree, relative), None, relative)
    if authored != consumed:
        raise ValueError("unused or unchanged --authored paths: " + ", ".join(sorted(authored - consumed)))
    if not changes:
        raise ValueError("edited tree has no distributable changes")
    if len(changes) > MAX_CHANGES:
        raise ValueError("too many changed files for one mod pack")
    manifest = dict(schema=SCHEMA, base=dict(serial=SERIAL, iso_sha256=base.info["image_sha256"]),
                    changes=changes, instructions=instructions)
    raw = _zip_bytes(manifest, payloads)
    if len(raw) > MAX_PACK:
        raise ValueError("mod pack exceeds the 1 GiB distribution limit")
    _scan(base, manifest, payloads, raw)
    out.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=".modpack-", dir=out.parent)
    try:
        with os.fdopen(handle, "wb") as stream:
            stream.write(raw)
        _validate_pack(base, Path(temporary))
        os.replace(temporary, out)
    finally:
        Path(temporary).unlink(missing_ok=True)
    return dict(pack=str(out), sha256=archive.sha256_file(out), size=len(raw), changes=len(changes),
                delta_files=sum(c["encoding"] == "delta-v1" for c in changes),
                authored_files=sum(c["encoding"] == "full" for c in changes), original_run_limit=64)


def _load_set(base, packs):
    loaded, owners, conflicts = [], {}, []
    for pack in packs:
        manifest, payloads, identity = _validate_pack(base, pack)
        name = str(Path(pack).resolve())
        instruction_targets = []
        for instruction, value in manifest["instructions"].items():
            if instruction == "stream-edits.json" and isinstance(value, dict) and isinstance(value.get("lengths"), dict):
                instruction_targets.extend("stream-cues/" + kind for kind in value["lengths"])
            else:
                instruction_targets.append(instruction)
        for target in [c["target"] for c in manifest["changes"]] + instruction_targets:
            if target in owners:
                conflicts.append(f"{target}: {owners[target]} <> {name}")
            else:
                owners[target] = name
        loaded.append((manifest, payloads, dict(path=name, sha256=identity)))
    if conflicts:
        raise ValueError("mod-pack conflicts (no changes applied):\n" + "\n".join(conflicts))
    return loaded


def _write_atomic(path, raw):
    path = archive.safe_output(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".mod-apply-", dir=path.parent)
    try:
        with os.fdopen(fd, "wb") as stream:
            stream.write(raw)
        os.replace(temporary, path)
    finally:
        Path(temporary).unlink(missing_ok=True)


def _install(base, loaded, tree):
    for manifest, payloads, _name in loaded:
        for change in manifest["changes"]:
            target = change["target"]
            raw = _reconstruct(base, change, payloads[change["payload"]])
            if target.startswith("add/"):
                region, ident = _addition(target)
                # add_entry owns ordinal selection, slack validation and rollback.
                temporary = tree / ".new-entry"
                _write_atomic(temporary, raw)
                try:
                    archive.add_entry(tree / "archive", region, ident, temporary)
                finally:
                    temporary.unlink(missing_ok=True)
            else:
                _write_atomic(_tree_file(tree, target), raw)
        for name, value in manifest["instructions"].items():
            if name == "stream-edits.json" and (tree / name).exists():
                previous = _json((tree / name).read_bytes())
                _keys(value, ("schema", "lengths"), "stream instructions")
                if not isinstance(value["lengths"], dict) or set(previous["lengths"]) & set(value["lengths"]):
                    raise ValueError("overlapping stream instructions")
                value = dict(value, lengths={**previous["lengths"], **value["lengths"]})
            _write_atomic(tree / name, _canonical(value))
    _preflight_instructions(tree)


def verify_modpack(image, packs):
    if not packs:
        raise ValueError("provide at least one mod pack")
    base = BaseDisc(image)
    loaded = _load_set(base, packs)
    # Instructions and new-entry capacity require the actual original layout.
    with tempfile.TemporaryDirectory(prefix="mod-verify-", dir=archive.OUTPUT_ROOT) as name:
        tree = Path(name)
        unpack_disc(base.image, tree)
        if _json((tree / "iso/manifest.json").read_bytes())["image_sha256"] != base.info["image_sha256"]:
            raise ValueError("original disc changed during verification")
        _install(base, loaded, tree)
        _preflight_layout(base, tree)
    return dict(verified=True, base_iso_sha256=base.info["image_sha256"], packs=len(loaded),
                changes=sum(len(m["changes"]) for m, _, _ in loaded), conflicts=[], original_run_limit=64)


def unpack_disc(image, tree):
    iso.unpack(image, tree / "iso")
    archive.unpack_archive(tree / "iso/files/DATA/DATA.DAT", tree / "iso/files/DATA/INDEX.IDX", tree / "archive")


def apply_modpack(image, packs, out, **build_options):
    from .build_disc import build_disc
    if not packs:
        raise ValueError("provide at least one mod pack")
    base, out = BaseDisc(image), archive.safe_output(out)
    if out.exists() and (not out.is_dir() or any(out.iterdir())):
        raise ValueError("mod installation output must be new or empty")
    if any(Path(p).resolve().is_relative_to(out) for p in [image, *packs]):
        raise ValueError("installation output contains an input")
    loaded = _load_set(base, packs)
    out.mkdir(parents=True, exist_ok=True)
    tree = out / "tree"
    unpack_disc(base.image, tree)
    if _json((tree / "iso/manifest.json").read_bytes())["image_sha256"] != base.info["image_sha256"]:
        raise ValueError("original disc changed during installation")
    _install(base, loaded, tree)
    _preflight_layout(base, tree)
    receipt = build_disc(tree, out / "disc", **build_options)
    result = dict(schema="extermination-mod-install-v1", base_iso_sha256=base.info["image_sha256"],
                  packs=[dict(identity, changes=len(m["changes"])) for m, _, identity in loaded],
                  image_path=receipt["iso"]["image_path"], image_sha256=receipt["iso"]["image_sha256"],
                  build_receipt=str(out / "disc/build-disc.json"), original_run_limit=64)
    (out / "installation.json").write_bytes(_canonical(result))
    return result
