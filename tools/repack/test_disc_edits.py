"""Minimal cue instructions reconstruct complete local tables without shipping them."""
from __future__ import annotations

import json
from pathlib import Path
import shutil
import tempfile
import unittest

from tools.repack import archive, disc_edits, iso, streams
from tools.repack.test_streams import fixture


def tree_fixture(base: Path) -> Path:
    source = base / 'source'
    source.mkdir()
    fixture(source)
    tree = base / 'tree'
    files = tree / 'iso/files'
    (files / 'STREAM').mkdir(parents=True)
    shutil.copyfile(source / 'boot.elf', files / 'SCUS_971.12')
    for profile in streams.PROFILES.values():
        shutil.copyfile(source / profile['filename'], files / 'STREAM' / profile['filename'])
    manifest = dict(schema=iso.SCHEMA, sector_size=2048, files=[dict(path=str(p.relative_to(files)),
                    size=p.stat().st_size, sha256=archive.sha256_file(p)) for p in sorted(files.rglob('*')) if p.is_file()])
    (tree / 'iso/manifest.json').write_text(json.dumps(manifest))
    return tree


def edit_tree(tree: Path, *, kind='music', cue=1, length=16384):
    profile = streams.PROFILES[kind]
    native = tree / 'iso/files/STREAM' / profile['filename']
    raw = native.read_bytes()
    old = streams._rows((tree / 'iso/files/SCUS_971.12').read_bytes(), kind)[cue]
    fragment = (raw[old[1]:old[1] + old[2]] * ((length + old[2] - 1) // old[2]))[:length]
    native.write_bytes(raw[:old[1]] + fragment + raw[old[1] + old[2]:])
    path = tree / disc_edits.METADATA
    meta = json.loads(path.read_text()) if path.exists() else dict(schema=disc_edits.SCHEMA, lengths={})
    meta['lengths'].setdefault(kind, {})[str(cue)] = length
    path.write_text(json.dumps(meta))
    return meta


class DiscEditTests(unittest.TestCase):
    def setUp(self):
        base = archive.ROOT / 'build/repack'
        base.mkdir(parents=True, exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix='disc-edits-', dir=base)
        self.addCleanup(temporary.cleanup)
        self.base = Path(temporary.name)
        self.tree = tree_fixture(self.base)

    def test_absent_metadata_is_noop_without_output_creation(self):
        output = self.base / 'unused'
        self.assertEqual(disc_edits.materialize_stream_bundles(self.tree, output), [])
        self.assertFalse(output.exists())

    def test_music_voice_lengths_relocate_all_later_rows_and_preserve_loop_flags(self):
        edit_tree(self.tree, kind='music', length=16384)
        meta = edit_tree(self.tree, kind='voice', cue=143, length=8192)
        original = self.tree / 'iso/files/SCUS_971.12'
        bundles = disc_edits.materialize_stream_bundles(self.tree, self.base / 'bundles')
        self.assertEqual(len(bundles), 2)
        for bundle in bundles:
            report = streams.inspect_bundle(bundle, original)
            rows = streams._rows(original.read_bytes(), report['kind'])
            changes = disc_edits.lengths_from_bundle(bundle, original)
            self.assertEqual(changes, {report['kind']: meta['lengths'][report['kind']]})
            self.assertEqual([r[3] for r in rows], [r[3] for r in report['rows']])
            self.assertEqual(report['rows'][-1][1] + report['rows'][-1][2], report['stream_size'])
            self.assertEqual(archive.sha256_file(bundle / report['stream_file']), report['stream_sha256'])
        patched = self.base / 'patched.elf'
        streams.apply_cue_patches(original, bundles, patched)
        self.assertEqual(streams._rows(patched.read_bytes(), 'music')[2][:2], [8, 16384])
        self.assertEqual(streams._rows(patched.read_bytes(), 'voice')[144][1],
                         streams._rows(original.read_bytes(), 'voice')[144][1] + 6144)

    def test_metadata_rejects_extra_fields_invalid_ids_lengths_and_noop_rows(self):
        edit_tree(self.tree)
        metadata = self.tree / disc_edits.METADATA
        valid = json.loads(metadata.read_text())
        invalid = [dict(valid, rows=[]), dict(valid, schema='unknown')]
        invalid += [dict(schema=disc_edits.SCHEMA, lengths={kind: changes}) for kind, changes in (
            ('unknown', {'1': 2048}), ('music', {}), ('music', {'0': 2048}), ('music', {'01': 2048}),
            ('music', {'68': 2048}), ('music', {'1': True}), ('music', {'1': 0}),
            ('music', {'1': 2049}), ('music', {'1': 65536}), ('music', {'1': 0x80000000}))]
        for value in invalid:
            with self.subTest(value=value):
                metadata.write_text(json.dumps(value))
                with self.assertRaises(ValueError):
                    disc_edits.validate_tree(self.tree)
        metadata.write_text('{"schema":"extermination-stream-edits-v1","lengths":{},"lengths":{}}')
        with self.assertRaisesRegex(ValueError, 'duplicate'):
            disc_edits.validate_tree(self.tree)

    def test_wrong_size_and_undeclared_other_stream_resize_rejected(self):
        edit_tree(self.tree)
        native = self.tree / 'iso/files/STREAM/MUSIC.DAT'
        raw = native.read_bytes()
        native.write_bytes(raw[:-2048])
        with self.assertRaisesRegex(ValueError, 'cover'):
            disc_edits.validate_tree(self.tree)
        native.write_bytes(raw)
        other = self.tree / 'iso/files/STREAM/VOICE.DAT'
        other.write_bytes(other.read_bytes() + bytes(2048))
        with self.assertRaisesRegex(ValueError, 'lacks cue'):
            disc_edits.validate_tree(self.tree)

    def test_changed_original_boot_or_manifest_identity_rejected(self):
        edit_tree(self.tree)
        boot = self.tree / 'iso/files/SCUS_971.12'
        raw = bytearray(boot.read_bytes()); raw[100] ^= 1; boot.write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'canonical'):
            disc_edits.validate_tree(self.tree)
        raw[100] ^= 1; boot.write_bytes(raw)
        path = self.tree / 'iso/manifest.json'
        metadata = json.loads(path.read_text())
        metadata['files'].append(metadata['files'][0])
        path.write_text(json.dumps(metadata))
        with self.assertRaisesRegex(ValueError, 'duplicate'):
            disc_edits.validate_tree(self.tree)

    def test_aliases_rejected_without_changing_inputs(self):
        metadata = self.tree / disc_edits.METADATA
        metadata.symlink_to(self.base / 'missing')
        with self.assertRaisesRegex(ValueError, 'symlinks'):
            disc_edits.validate_tree(self.tree)
        metadata.unlink(); edit_tree(self.tree)
        before = archive.sha256_file(self.tree / 'iso/files/STREAM/MUSIC.DAT')
        with self.assertRaisesRegex(ValueError, 'separate'):
            disc_edits.materialize_stream_bundles(self.tree, self.tree / 'bundles')
        alias = self.base / 'alias'; alias.symlink_to(self.base / 'future')
        with self.assertRaisesRegex(ValueError, 'symlink'):
            disc_edits.materialize_stream_bundles(self.tree, alias)
        self.assertEqual(archive.sha256_file(self.tree / 'iso/files/STREAM/MUSIC.DAT'), before)

    def test_resume_requires_exact_materialized_hashes_and_metadata(self):
        edit_tree(self.tree)
        output = self.base / 'bundles'
        bundles = disc_edits.materialize_stream_bundles(self.tree, output)
        self.assertEqual(disc_edits.materialize_stream_bundles(self.tree, output, resume=True), bundles)
        native = bundles[0] / 'MUSIC.DAT'
        raw = bytearray(native.read_bytes()); raw[10] ^= 1; native.write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'verified cue'):
            disc_edits.materialize_stream_bundles(self.tree, output, resume=True)


if __name__ == '__main__':
    unittest.main()
