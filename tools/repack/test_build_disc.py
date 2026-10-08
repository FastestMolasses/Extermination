"""Build orchestration must patch only the newly built boot and paired streams."""
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
from tools.repack import build_disc

class BuildDiscTests(unittest.TestCase):
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

if __name__ == '__main__':
    unittest.main()
