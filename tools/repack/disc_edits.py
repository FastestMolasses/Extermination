"""Minimal tree-level cue edits; full native tables are reconstructed locally."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import re
import shutil

from . import archive, iso, streams

SCHEMA = 'extermination-stream-edits-v1'
METADATA = 'stream-edits.json'


def _input(tree: Path, relative: str) -> Path:
    path = tree / relative
    if any(p.is_symlink() for p in (path, *path.parents) if p != tree.parent):
        raise ValueError('disc edit inputs must not use symlinks')
    return archive._input(tree, relative)


def _json(path: Path):
    def pairs(items):
        result = {}
        for key, value in items:
            if key in result:
                raise ValueError('duplicate JSON key in disc edit metadata')
            result[key] = value
        return result
    try:
        return json.loads(path.read_text(), object_pairs_hook=pairs)
    except (UnicodeError, json.JSONDecodeError) as exc:
        raise ValueError('invalid disc edit JSON') from exc


def _file_records(tree: Path) -> dict:
    manifest = _json(_input(tree, 'iso/manifest.json'))
    if (not isinstance(manifest, dict) or manifest.get('schema') != iso.SCHEMA
            or manifest.get('sector_size') != streams.SECTOR or not isinstance(manifest.get('files'), list)):
        raise ValueError('disc edits require an original ISO manifest')
    files = {}
    for entry in manifest['files']:
        if (not isinstance(entry, dict) or not isinstance(entry.get('path'), str)
                or entry['path'] in files or type(entry.get('size')) is not int or entry['size'] < 0
                or not isinstance(entry.get('sha256'), str)
                or not re.fullmatch('[0-9a-f]{64}', entry['sha256'])):
            raise ValueError('invalid or duplicate original ISO file identity')
        files[entry['path']] = entry
    return files


def validate_tree(tree: Path) -> list[dict]:
    """Validate minimal cue lengths and pin current DAT hashes without writing."""
    tree = Path(tree).resolve()
    metadata = tree / METADATA
    if not metadata.exists() and not metadata.is_symlink():
        return []
    edits = _json(_input(tree, METADATA))
    if (not isinstance(edits, dict) or set(edits) != {'schema', 'lengths'} or edits['schema'] != SCHEMA
            or not isinstance(edits['lengths'], dict) or edits['lengths'].keys() - streams.PROFILES.keys()):
        raise ValueError('unsupported stream-edits schema or fields')
    if not edits['lengths']:
        return []
    files = _file_records(tree)
    boot_path = _input(tree, 'iso/files/SCUS_971.12')
    elf = boot_path.read_bytes()
    boot = files.get('SCUS_971.12')
    digest = hashlib.sha256(elf).hexdigest()
    if boot is None or boot['size'] != len(elf) or boot['sha256'] != digest:
        raise ValueError('stream edits require the unchanged canonical tree boot')
    for kind, profile in streams.PROFILES.items():
        key = 'STREAM/' + profile['filename']
        if key not in files:
            raise ValueError('stream source is absent from original ISO manifest')
        if kind not in edits['lengths']:
            current = _input(tree, 'iso/files/' + key)
            if current.stat().st_size != files[key]['size']:
                raise ValueError('resized tree stream lacks cue length instructions')
    result = []
    for kind in sorted(edits['lengths']):
        changes, profile = edits['lengths'][kind], streams.PROFILES[kind]
        if not isinstance(changes, dict) or not changes:
            raise ValueError('each stream edit kind needs changed cue lengths')
        key = 'STREAM/' + profile['filename']
        original = files.get(key)
        if original is None:
            raise ValueError('stream source is absent from original ISO manifest')
        old_rows = streams._rows(elf, kind)
        streams._validate_rows(old_rows, kind, original['size'])
        for cue, length in changes.items():
            if (not re.fullmatch('[1-9][0-9]*', cue) or not 1 <= int(cue) < profile['count']
                    or type(length) is not int or length <= 0 or length > 0x7FFFF800
                    or length % streams.SECTOR or length == old_rows[int(cue)][2]):
                raise ValueError('invalid or unchanged cue length; use existing canonical IDs and sector lengths')
        rows, cursor = [[0, 0, 0, 0]], 0
        for cue, old in enumerate(old_rows[1:], 1):
            length = changes.get(str(cue), old[2])
            rows.append([cursor // streams.SECTOR, cursor, length, old[3]])
            cursor += length
        native = _input(tree, 'iso/files/' + key)
        streams._validate_rows(rows, kind, native.stat().st_size)
        offset = streams._table_offset(elf, kind)
        result.append(dict(schema=streams.BUNDLE_SCHEMA, kind=kind, stream_file=profile['filename'],
                           stream_size=cursor, stream_sha256=archive.sha256_file(native),
                           original_stream_sha256=original['sha256'], original_elf_sha256=digest,
                           table_vram=profile['vram'], table_count=profile['count'],
                           original_table_sha256=hashlib.sha256(elf[offset:offset + profile['count'] * 16]).hexdigest(),
                           rows=rows, changed_cues=[dict(cue=int(c), original_size=old_rows[int(c)][2],
                                                       new_size=changes[c]) for c in sorted(changes, key=int)],
                           byte_identical=False, stream_path=str(native)))
    return result


def materialize_stream_bundles(tree: Path, out: Path, *, resume=False) -> list[Path]:
    """Build existing stream-bundle format below a new, separate output folder."""
    tree = Path(tree).resolve()
    plans = validate_tree(tree)
    if not plans:
        return []
    requested = Path(out)
    if requested.is_symlink():
        raise ValueError('stream bundle output must not be a symlink')
    out = archive.safe_output(requested)
    if out == tree or out.is_relative_to(tree) or tree.is_relative_to(out):
        raise ValueError('stream bundles must be separate from their input tree')
    if out.exists() and (not out.is_dir() or any(out.iterdir())):
        if not resume or not out.is_dir() or {p.name for p in out.iterdir()} != {p['kind'] for p in plans}:
            raise ValueError('materialized stream bundle output must be new or empty')
        bundles = []
        for plan in plans:
            target = out / plan['kind']
            if target.is_symlink() or not target.is_dir() or {p.name for p in target.iterdir()} != {plan['stream_file'], 'cue-edits.json'}:
                raise ValueError('resumed stream bundle contents differ from preflight')
            expected = {key: value for key, value in plan.items() if key != 'stream_path'}
            if _json(_input(target, 'cue-edits.json')) != expected:
                raise ValueError('resumed stream bundle metadata differs from tree edits')
            _input(target, plan['stream_file'])
            streams.inspect_bundle(target, tree / 'iso/files/SCUS_971.12')
            bundles.append(target)
        return bundles
    out.mkdir(parents=True, exist_ok=True)
    bundles = []
    for plan in plans:
        target = out / plan['kind']
        target.mkdir()
        native = target / plan['stream_file']
        with Path(plan['stream_path']).open('rb') as source, native.open('xb') as dest:
            shutil.copyfileobj(source, dest, archive.COPY_SIZE)
        if native.stat().st_size != plan['stream_size'] or archive.sha256_file(native) != plan['stream_sha256']:
            raise ValueError('tree stream changed during bundle materialization')
        receipt = {key: value for key, value in plan.items() if key != 'stream_path'}
        (target / 'cue-edits.json').write_text(json.dumps(receipt, indent=2) + '\n')
        streams.inspect_bundle(target, tree / 'iso/files/SCUS_971.12')
        bundles.append(target)
    return bundles


def lengths_from_bundle(bundle: Path, canonical_elf: Path) -> dict:
    """Return only changed lengths; no original cue rows enter a modpack."""
    info = streams.inspect_bundle(bundle, canonical_elf)
    rows = streams._rows(Path(canonical_elf).read_bytes(), info['kind'])
    changed = {str(cue): new[2] for cue, (old, new) in enumerate(zip(rows, info['rows']))
               if old[2] != new[2]}
    return {info['kind']: changed} if changed else {}
