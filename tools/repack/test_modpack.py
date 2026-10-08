"""Synthetic distribution/adversarial proofs; no original asset fixtures."""
from pathlib import Path
import copy
import contextlib
import io
import json
import os
import shutil
import tempfile
import unittest
from unittest.mock import patch
import zipfile

from tools.repack import archive, build_disc, delta, iso, modpack
from tools.repack.test_archive import synthetic_archive
from tools.repack.test_iso import synthetic_iso


class ModPackTests(unittest.TestCase):
    def setUp(self):
        archive.OUTPUT_ROOT.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix='modpack-test-', dir=archive.OUTPUT_ROOT)
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        seed = synthetic_iso(self.root / 'seed.iso')
        temporary = self.root / 'seed-tree'
        iso.unpack(seed, temporary)
        data, index = synthetic_archive(self.root / 'native')
        self.image = self.root / 'original.iso'
        iso.pack(temporary, self.image, overrides={'DATA/DATA.DAT': data, 'DATA/INDEX.IDX': index})
        self.tree = self.root / 'edited'
        modpack.unpack_disc(self.image, self.tree)
        self.base = modpack.BaseDisc(self.image)
        self.a = 'archive/chunk00/f00_id43.bin'
        self.b = 'archive/chunk01/f00_id05.bin'

    def edit(self, target, value=0xF4):
        path = modpack._tree_file(self.tree, target)
        raw = bytearray(path.read_bytes()); raw[31] = value
        path.write_bytes(raw)

    def make(self, name='one.emmod', **kwargs):
        return Path(modpack.make_modpack(self.image, self.tree, self.root / name, **kwargs)['pack'])

    def mutated_pack(self, original, edit):
        manifest, payloads, _ = modpack._load(original)
        edit(manifest, payloads)
        path = self.root / 'hostile.emmod'
        path.write_bytes(modpack._zip_bytes(manifest, payloads))
        return path

    def test_delta_payload_and_raw_zip_header_join_is_scanned(self):
        # These are all authored fixture bytes. The copied run physically spans
        # an INSERT payload and the next member's local ZIP header.
        prefix = bytes(range(31))
        encoded = delta.encode(b'', prefix)
        manifest = {'schema': modpack.SCHEMA,
                    'base': {'serial': modpack.SERIAL, 'iso_sha256': '0' * 64},
                    'changes': [{'encoding': 'delta-v1', 'payload': 'changes/0000.delta'}],
                    'instructions': {}}
        payloads = {'changes/0000.delta': encoded}
        raw = modpack._zip_bytes(manifest, payloads)
        with zipfile.ZipFile(io.BytesIO(raw)) as package:
            member = package.getinfo('changes/0000.delta')
            end = member.header_offset + 30 + len(member.filename.encode()) + member.file_size
        original = prefix + raw[end:end + 34]
        self.assertEqual(len(original), 65)
        self.assertIn(original, raw)
        self.image.write_bytes(original)
        with self.assertRaisesRegex(ValueError, '65 original bytes'):
            modpack._scan(self.base, manifest, payloads, raw)

    def test_deterministic_delta_and_exact_iso_after_apply_through_build_disc(self):
        self.edit(self.a)
        pack = self.make()
        self.assertEqual(pack.read_bytes(), self.make('two.emmod').read_bytes())
        info = modpack.verify_modpack(self.image, [pack])
        self.assertEqual(info['changes'], 1)
        m, payloads, _ = modpack._load(pack)
        self.assertEqual(m['changes'][0]['encoding'], 'delta-v1')
        self.assertLess(pack.stat().st_size, 2000)
        direct = self.root / 'direct'; direct.mkdir()
        archive.pack_archive(self.tree/'archive', direct/'DATA.DAT', direct/'INDEX.IDX')
        expected = iso.pack(self.tree/'iso', direct/'image.iso', overrides={
            'DATA/DATA.DAT':direct/'DATA.DAT', 'DATA/INDEX.IDX':direct/'INDEX.IDX'})
        def source(tree, out, **kwargs):
            # Real-disc tests exercise the actual compiler. This isolates pack
            # composition while still using the real archive/ISO/build driver.
            boot = out/'boot'; boot.parent.mkdir(parents=True)
            shutil.copyfile(tree/'iso/files/SCUS_971.12', boot)
            return {'overrides':{'SCUS_971.12':str(boot)}}
        with patch.object(build_disc.source_build, 'build_sources', side_effect=source) as compile:
            result = modpack.apply_modpack(self.image, [pack], self.root/'installed')
        self.assertEqual(result['image_sha256'], expected['image_sha256'])
        self.assertEqual(Path(result['image_path']).read_bytes(), (direct/'image.iso').read_bytes())
        compile.assert_called_once()

    def test_two_independent_packs_stack_but_same_file_conflicts_before_writes(self):
        original = modpack._tree_file(self.tree, self.a).read_bytes()
        self.edit(self.a); first = self.make()
        modpack._tree_file(self.tree, self.a).write_bytes(original)
        self.edit(self.b); second = self.make('second.emmod')
        self.assertEqual(modpack.verify_modpack(self.image, [first, second])['changes'], 2)
        loaded = modpack._load_set(self.base, [first, second])
        tree = self.root/'stack'; modpack.unpack_disc(self.image, tree)
        modpack._install(self.base, loaded, tree)
        for target in (self.a, self.b):
            self.assertEqual(modpack._tree_file(tree, target).read_bytes()[31], 0xF4)
        output = self.root/'conflict'
        with patch.object(build_disc.source_build, 'build_sources') as compile:
            with self.assertRaisesRegex(ValueError, 'conflicts.*'):
                modpack.apply_modpack(self.image, [first, first], output)
            compile.assert_not_called()
        self.assertFalse(output.exists())

    def test_declared_new_entries_stack_by_role_and_recompute_ordinals(self):
        source = self.root/'new.bin'; source.write_bytes(bytes(range(160)))
        added = archive.add_entry(self.tree/'archive', 'chunk01', 0xF0, source)
        with self.assertRaisesRegex(ValueError, 'requires --authored'):
            self.make()
        first = self.make(authored=['archive/'+added['path']])
        # A separate mod starts at the same original ordinal, different role.
        (self.tree/'archive'/added['path']).unlink(); (self.tree/'archive/edits.json').unlink()
        added2 = archive.add_entry(self.tree/'archive', 'chunk01', 0xF1, source)
        second = self.make('second.emmod', authored=['archive/'+added2['path']])
        tree = self.root/'installed'; modpack.unpack_disc(self.image, tree)
        modpack._install(self.base, modpack._load_set(self.base, [first,second]), tree)
        entries = json.loads((tree/'archive/edits.json').read_text())['additions']
        self.assertEqual([e['id'] for e in entries], [0xF0,0xF1])
        self.assertNotEqual(entries[0]['path'].split('_')[0], entries[1]['path'].split('_')[0])

    def test_full_declaration_does_not_allow_65_original_bytes_from_untouched_file(self):
        source = self.root/'copied.bin'
        source.write_bytes(self.base.read(self.b)[:65])
        added = archive.add_entry(self.tree/'archive', 'chunk01', 0xF0, source)
        with self.assertRaisesRegex(ValueError, '65 original bytes'):
            self.make(authored=['archive/'+added['path']])
        self.assertFalse((self.root/'one.emmod').exists())

    def test_declared_full_replacement_and_wrong_original_are_checked(self):
        path = modpack._tree_file(self.tree, self.b)
        path.write_bytes(b'Wholly authored synthetic replacement, no borrowed payload.')
        pack = self.make(authored=[self.b])
        self.assertEqual(modpack.verify_modpack(self.image,[pack])['changes'],1)
        m, payloads, _ = modpack._load(pack)
        self.assertTrue(m['changes'][0]['created_from_scratch'])
        hostile = self.mutated_pack(pack, lambda m,p:m['changes'][0].update(created_from_scratch=False))
        with self.assertRaisesRegex(ValueError,'declaration'):
            modpack.verify_modpack(self.image,[hostile])
        hostile = self.mutated_pack(pack, lambda m,p:m['base'].update(iso_sha256='0'*64))
        with self.assertRaisesRegex(ValueError,'different original'):
            modpack.verify_modpack(self.image,[hostile])

    def test_hashes_unknown_paths_hidden_members_and_archive_trailers_fail(self):
        self.edit(self.a); pack=self.make()
        for change, message in ((lambda m,p:m['changes'][0].update(result_sha256='0'*64),'hash/size'),
                                (lambda m,p:m['changes'][0].update(target='archive/../other'),'unsafe'),
                                (lambda m,p:p.update({'hidden.bin':b'forbidden'}),'unreferenced')):
            with self.subTest(message=message):
                hostile=self.mutated_pack(pack,change)
                with self.assertRaisesRegex(ValueError,message):modpack.verify_modpack(self.image,[hostile])
        extra=self.root/'extra.emmod';extra.write_bytes(pack.read_bytes()+b'original-data-smuggling-channel')
        with self.assertRaisesRegex(ValueError,'noncanonical'):
            modpack.verify_modpack(self.image,[extra])
        extra.write_bytes(b'not a ZIP')
        with self.assertRaisesRegex(ValueError,'invalid mod-pack ZIP'):
            modpack.verify_modpack(self.image,[extra])

    def test_disjoint_stream_instruction_sets_merge_and_pack_disc_refuses_them(self):
        # The native cue-layout validators have independent real/synthetic
        # tests; here exercise pack ownership, merging and CLI routing.
        self.edit(self.a); first=self.make()
        second=self.mutated_pack(first,lambda m,p:m['changes'][0].update(target=self.b))
        first_m, first_p, _=modpack._load(first)
        second_m, second_p, _=modpack._load(second)
        for manifest,kind in ((first_m,'music'),(second_m,'voice')):
            manifest['instructions']={'stream-edits.json':{
                'schema':'extermination-stream-edits-v1','lengths':{kind:{'1':4096}}}}
        with patch.object(modpack,'_validate_pack',side_effect=[(first_m,first_p,'a'),(second_m,second_p,'b')]):
            loaded=modpack._load_set(self.base,[first,second])
        # Only instruction composition is isolated; no bypass exists in CLI.
        for manifest,_,_ in loaded: manifest['changes']=[]
        with patch.object(modpack,'_preflight_instructions'):
            modpack._install(self.base,loaded,self.tree)
        merged=json.loads((self.tree/'stream-edits.json').read_text())
        self.assertEqual(set(merged['lengths']),{'music','voice'})
        from tools.repack.__main__ import main
        output=self.root/'wrong-driver'
        with contextlib.redirect_stderr(io.StringIO()) as errors:
            with self.assertRaises(SystemExit) as failed:
                main(['pack-disc','--tree',str(self.tree),'--out',str(output)])
        self.assertNotEqual(failed.exception.code,0)
        self.assertIn('use build-disc',errors.getvalue())
        self.assertFalse(output.exists())

    def test_original_templates_executables_unknown_files_and_resize_rejected(self):
        self.edit(self.a)
        target=modpack._tree_file(self.tree,'iso/SCUS_971.12');before=target.read_bytes()
        target.write_bytes(b'Z'+before[1:])
        with self.assertRaisesRegex(ValueError,'source outputs'):
            self.make()
        target.write_bytes(before)
        target=modpack._tree_file(self.tree,'iso/SYSTEM.CNF');before=target.read_bytes()
        target.write_bytes(before+b'new')
        with self.assertRaisesRegex(ValueError,'unsupported ISO file resize'):
            self.make()
        target.write_bytes(before)
        (self.tree/'unrecognized.bin').write_bytes(b'unsupported loose entry')
        with self.assertRaisesRegex(ValueError,'unrecognized loose-tree'):
            self.make()

    def test_output_aliases_symlinks_and_bundle_inputs_are_refused(self):
        self.edit(self.a)
        alias=self.root/'alias.emmod';alias.hardlink_to(modpack._tree_file(self.tree,self.a))
        with self.assertRaisesRegex(ValueError,'aliases an input'):
            modpack.make_modpack(self.image,self.tree,alias)
        alias.unlink();alias.symlink_to(self.root/'missing')
        with self.assertRaisesRegex(ValueError,'symlink'):
            modpack.make_modpack(self.image,self.tree,alias)
        bundle=self.root/'bundle';bundle.mkdir();(bundle/'cue-edits.json').write_text('{}')
        with self.assertRaisesRegex(ValueError,'bundle input'):
            modpack.make_modpack(self.image,self.tree,bundle/'result.emmod',stream_bundles=[bundle])

    def test_full_and_delta_content_join_cannot_hide_a_copied_run(self):
        # Test the actual pack channel selection with an original synthetic
        # string split across two legitimate-looking different member types.
        run=bytes(range(65)); self.image.write_bytes(run)
        base=b'base'; edited=run[32:]
        encoded=delta.encode(base,edited,prefer_xor=False)
        m={'schema':modpack.SCHEMA,'base':{'serial':modpack.SERIAL,'iso_sha256':'0'*64},
           'changes':[{'encoding':'full','payload':'changes/0000.full'},
                      {'encoding':'delta-v1','payload':'changes/0001.delta'}],'instructions':{}}
        payloads={'changes/0000.full':run[:32],'changes/0001.delta':encoded}
        raw=modpack._zip_bytes(m,payloads)
        with self.assertRaisesRegex(ValueError,'65 original bytes'):
            modpack._scan(self.base,m,payloads,raw)


if __name__=='__main__': unittest.main()
