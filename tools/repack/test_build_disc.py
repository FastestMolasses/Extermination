"""Build orchestration must patch only the newly built boot and paired streams."""
from pathlib import Path
import json
import shutil
import tempfile
import unittest
from unittest.mock import patch
from tools.repack import build_disc
from tools.repack.test_disc_edits import tree_fixture, edit_tree

class BuildDiscTests(unittest.TestCase):
    def workspace(self):
        base = build_disc.archive.ROOT / 'build/repack'
        base.mkdir(parents=True, exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix='build-disc-test-', dir=base)
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        return root, tree_fixture(root), root / 'out'

    def test_cue_patch_uses_fresh_build_and_grants_only_paired_stream_resize(self):
        base = build_disc.archive.ROOT / 'build/repack'
        base.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=base) as d:
            root = Path(d); tree = root/'tree'; out = root/'out'; bundle = root/'bundle'
            original = tree/'iso/files/STREAM/MUSIC.DAT'
            original.parent.mkdir(parents=True); original.write_bytes(b'synthetic source stream')
            fresh = out/'source/boot'; native = bundle/'MUSIC.DAT'
            calls=[]
            def compile(*args, **kwargs):
                calls.append('compile')
                return {'overrides': {'SCUS_971.12': str(fresh)}}
            def cue(source, bundles, output):
                self.assertEqual(source, fresh); self.assertEqual(bundles, [bundle])
                calls.append('cue')
                return {'output': str(output), 'overrides': {'STREAM/MUSIC.DAT': str(native)}}
            def pack(source, output, **kwargs):
                self.assertEqual(kwargs['overrides']['SCUS_971.12'], out/'cue-boot/SCUS_971.12')
                self.assertEqual(kwargs['overrides']['STREAM/MUSIC.DAT'], native)
                self.assertEqual(kwargs['resized_streams'], {'STREAM/MUSIC.DAT'})
                calls.append('iso')
                return {'unchanged': False, 'files': []}
            with patch.object(build_disc.streams,'inspect_bundle',return_value={'kind':'music',
                    'stream_file':'MUSIC.DAT', 'original_stream_sha256':build_disc.archive.sha256_file(original)}) as inspect, \
                 patch.object(build_disc.source_build,'build_sources',side_effect=compile), \
                 patch.object(build_disc.archive,'pack_archive',return_value={}), \
                 patch.object(build_disc.streams,'apply_cue_patches',side_effect=cue), \
                 patch.object(build_disc.iso,'pack',side_effect=pack):
                build_disc.build_disc(tree,out,stream_bundles=[bundle])
            inspect.assert_called_once_with(bundle,tree/'iso/files/SCUS_971.12')
            self.assertEqual(calls,['compile','cue','iso'])

    def test_bad_bundle_rejected_before_compilation(self):
        base=build_disc.archive.ROOT/'build/repack'
        with patch.object(build_disc.streams,'inspect_bundle',side_effect=ValueError('bad bundle')), \
             patch.object(build_disc.source_build,'build_sources') as compile:
            with self.assertRaisesRegex(ValueError,'bad bundle'):
                build_disc.build_disc(base/'tree',base/'output',stream_bundles=[base/'bundle'])
            compile.assert_not_called()

    def test_tree_stream_instructions_use_edited_bytes_and_canonical_fresh_boot(self):
        root, tree, out = self.workspace()
        edit_tree(tree, length=16384)
        native = tree / 'iso/files/STREAM/MUSIC.DAT'
        expected = build_disc.archive.sha256_file(native)
        fresh = out / 'source/SCUS_971.12'

        def compile(*_args, **_kwargs):
            fresh.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(tree / 'iso/files/SCUS_971.12', fresh)
            return {'overrides': {'SCUS_971.12': str(fresh)}}

        def pack(_tree, _output, **options):
            self.assertEqual(options['resized_streams'], {'STREAM/MUSIC.DAT'})
            self.assertEqual(build_disc.archive.sha256_file(options['overrides']['STREAM/MUSIC.DAT']), expected)
            boot = options['overrides']['SCUS_971.12']
            self.assertEqual(boot, out / 'cue-boot/SCUS_971.12')
            rows = build_disc.streams._rows(boot.read_bytes(), 'music')
            self.assertEqual(rows[1][2], 16384)
            self.assertEqual(rows[2][1], 16384)
            return {'files': [], 'unchanged': False}

        with patch.object(build_disc.source_build, 'build_sources', side_effect=compile), \
                patch.object(build_disc.archive, 'pack_archive', return_value={}), \
                patch.object(build_disc.iso, 'pack', side_effect=pack):
            result = build_disc.build_disc(tree, out)
            # Resume keeps the exact verified local copies and still rebuilds through
            # the normal source-build resume entry point.
            again = build_disc.build_disc(tree, out, resume=True)
        self.assertEqual(result['cue_patch'], again['cue_patch'])
        self.assertNotIn('texture_patch', result)

    def test_duplicate_explicit_and_tree_kind_fails_before_compile(self):
        root, tree, out = self.workspace()
        edit_tree(tree)
        bundle = build_disc.disc_edits.materialize_stream_bundles(tree, root / 'external')[0]
        with patch.object(build_disc.source_build, 'build_sources') as compile:
            with self.assertRaisesRegex(ValueError, 'one bundle'):
                build_disc.build_disc(tree, out, stream_bundles=[bundle])
            compile.assert_not_called()

    def test_bad_tree_stream_or_texture_metadata_fails_before_compile(self):
        _root, tree, out = self.workspace()
        edit_tree(tree)
        meta = tree / 'stream-edits.json'
        value = json.loads(meta.read_text())
        value['lengths']['music']['1'] += 2048
        meta.write_text(json.dumps(value))
        with patch.object(build_disc.source_build, 'build_sources') as compile:
            with self.assertRaisesRegex(ValueError, 'cover'):
                build_disc.build_disc(tree, out)
            compile.assert_not_called()
        meta.unlink()
        with patch.object(build_disc.texture_upgrade, 'validate_tree', side_effect=ValueError('bad texture')), \
                patch.object(build_disc.source_build, 'build_sources') as compile:
            with self.assertRaisesRegex(ValueError, 'bad texture'):
                build_disc.build_disc(tree, out)
            compile.assert_not_called()

    def test_independent_texture_and_stream_patches_combine_against_same_canonical_boot(self):
        _root, tree, out = self.workspace()
        edit_tree(tree)
        original = (tree / 'iso/files/SCUS_971.12').read_bytes()
        fresh = out / 'source/SCUS_971.12'

        def compile(*_args, **_kwargs):
            fresh.parent.mkdir(parents=True)
            fresh.write_bytes(original)
            return {'overrides': {'SCUS_971.12': str(fresh)}}

        def texture(source, source_tree, output):
            self.assertEqual((source, source_tree), (fresh, tree))
            self.assertEqual(source.read_bytes(), original)
            raw = bytearray(original); raw[100] ^= 1
            output.parent.mkdir(parents=True); output.write_bytes(raw)
            return dict(output=str(output), sha256=build_disc.archive.sha256_file(output),
                        original_elf_sha256=build_disc.archive.sha256_file(source))

        def pack(_tree, _output, **options):
            boot = options['overrides']['SCUS_971.12']
            self.assertEqual(boot, out / 'mod-boot/SCUS_971.12')
            self.assertEqual(boot.read_bytes()[100], original[100] ^ 1)
            self.assertEqual(build_disc.streams._rows(boot.read_bytes(), 'music')[1][2], 16384)
            self.assertEqual(options['resized_streams'], {'STREAM/MUSIC.DAT'})
            return {'files': [], 'unchanged': False}

        with patch.object(build_disc.source_build, 'build_sources', side_effect=compile), \
                patch.object(build_disc.archive, 'pack_archive', return_value={}), \
                patch.object(build_disc.texture_upgrade, 'validate_tree', return_value={'active': True}), \
                patch.object(build_disc.texture_upgrade, 'apply_boot_patches', side_effect=texture), \
                patch.object(build_disc.iso, 'pack', side_effect=pack):
            result = build_disc.build_disc(tree, out)
        self.assertIn('texture_patch', result)
        self.assertIn(100, result['combined_boot']['changed_byte_offsets'])

    def test_combination_rejects_overlap_tamper_and_output_alias(self):
        root, _tree, _out = self.workspace()
        fresh = root / 'canonical'; fresh.write_bytes(bytes(16))
        paths, patches = [], []
        for index, at in enumerate((2, 2)):
            path = root / f'patched{index}'
            raw = bytearray(16); raw[at] = 1; path.write_bytes(raw)
            paths.append(path)
            patches.append(dict(output=str(path), sha256=build_disc.archive.sha256_file(path),
                                original_elf_sha256=build_disc.archive.sha256_file(fresh)))
        output = root / 'combined'
        with self.assertRaisesRegex(ValueError, 'overlap'):
            build_disc._combine_boot_patches(fresh, patches, output)
        self.assertFalse(output.exists())
        paths[1].write_bytes(bytes(16))
        with self.assertRaisesRegex(ValueError, 'receipt'):
            build_disc._combine_boot_patches(fresh, patches, output)
        with self.assertRaisesRegex(ValueError, 'aliases'):
            build_disc._combine_boot_patches(fresh, patches[:1], fresh)
        self.assertEqual(fresh.read_bytes(), bytes(16))

if __name__ == '__main__':
    unittest.main()
