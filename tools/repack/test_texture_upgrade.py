"""Bounded title layout, PNG cardinality and fresh executable patch proofs."""
from pathlib import Path
import json
import os
import shutil
import tempfile
import unittest
from unittest.mock import Mock

from tools.repack import png, texture_upgrade as upgrade
from tools.repack.archive import ROOT


class TextureUpgradeTests(unittest.TestCase):
    def test_runtime_probe_reads_exact_function_and_rejects_mismatch(self):
        params=upgrade._parameters()
        for name in upgrade.NAMES[:4]: params[name]['palette_size']=16
        params['new-game-selected']=dict(width=512,height=256,palette_size=256)
        layout=upgrade._layout(params)
        tokens=tuple(layout[name]['tex0'] for name in upgrade.NAMES)
        plan=dict(active=True,profile=upgrade.PROFILE,tex0s=dict(zip(upgrade.NAMES,tokens)))
        game=Mock()
        game.read.return_value=upgrade._compositor(tokens)
        result=upgrade.probe_runtime(game,plan)
        game.read.assert_called_once_with(upgrade.FUNCTION,upgrade.FUNCTION_SIZE)
        self.assertEqual(result['views']['new-game-selected']['width'],512)
        self.assertEqual(result['views']['new-game-selected']['height'],256)
        self.assertEqual(result['views']['new-game-selected']['psm'],19)
        game.read.return_value=b'wrong disc'
        with self.assertRaisesRegex(RuntimeError,'loaded title compositor'):
            upgrade.probe_runtime(game,plan)

    def test_owned_gs_capacity_and_parameter_whitelist(self):
        parameters = upgrade._parameters()
        layout = upgrade._layout(parameters)
        self.assertEqual(set(layout), set(upgrade.NAMES))
        parameters['new-game-selected'] = dict(width=512, height=256, palette_size=256)
        with self.assertRaisesRegex(ValueError, 'owned GS arena'):
            upgrade._layout(parameters)
        for name in upgrade.NAMES[:4]:
            parameters[name]['palette_size'] = 16
        result = upgrade._layout(parameters)
        self.assertEqual(result['new-game-selected']['psm'], 0x13)
        self.assertEqual(result['background-top-left']['psm'], 0x14)
        parameters['new-game-selected']['patch_address'] = 0
        with self.assertRaisesRegex(ValueError, 'unknown fields'):
            upgrade._layout(parameters)

    def test_palette_conversion_requires_explicit_loss(self):
        rgba = b''.join(bytes((i*7, i*3, i*5, 255)) for i in range(32))
        with self.assertRaisesRegex(ValueError, 'exceeding palette16'):
            upgrade._indexed(rgba, 16, 'exact')
        indices, palette, decoded = upgrade._indexed(rgba, 16, 'median-cut')
        self.assertEqual(len(indices), 32)
        self.assertEqual(len(palette), 16)
        self.assertLessEqual(len(set(decoded[i:i+4] for i in range(0, len(decoded), 4))), 16)
        _, expanded, expanded_rgba = upgrade._indexed(decoded, 256, 'exact')
        self.assertEqual(len(expanded), 256)
        self.assertEqual(expanded_rgba, decoded)

    def test_gs_alpha_and_dimension_refusals(self):
        with self.assertRaisesRegex(ValueError, 'alpha'):
            upgrade._indexed(bytes((1,2,3,127)), 16, 'exact')
        parameters = upgrade._parameters()
        for width in (0, 129, 2048):
            parameters['new-game-selected']['width'] = width
            with self.assertRaisesRegex(ValueError, 'power-of-two'):
                upgrade._layout(parameters)
        for width,height in ((1024,256),(256,1024)):
            parameters['new-game-selected']=dict(width=width,height=height,palette_size=16)
            with self.assertRaisesRegex(ValueError,'14-bit UV'):
                upgrade._layout(parameters)
        parameters['new-game-selected'] = dict(width=64,height=32,palette_size=16)
        layout = upgrade._layout(parameters)
        self.assertEqual(layout['new-game-selected']['tbw'],2)
        self.assertGreaterEqual(layout['load-game']['tbp']-layout['new-game-selected']['tbp'],32)

    def test_empty_descriptor_is_noop(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build/repack') as tmp:
            self.assertFalse(upgrade.validate_tree(Path(tmp))['active'])
            (Path(tmp)/upgrade.DESCRIPTOR).symlink_to(Path(tmp)/'absent.json')
            with self.assertRaisesRegex(ValueError, 'symlink'):
                upgrade.validate_tree(Path(tmp))

    def test_outputs_reject_symlink_before_resolution(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build/repack') as tmp:
            root = Path(tmp)
            source, target, alias = root/'source.elf', root/'untouched.bin', root/'alias.elf'
            source.write_bytes(b'fresh'); target.write_bytes(b'preserve'); alias.symlink_to(target)
            with self.assertRaisesRegex(ValueError, 'symlink'):
                upgrade._atomic(alias,b'bad')
            with self.assertRaisesRegex(ValueError, 'symlink'):
                upgrade.apply_boot_patches(source,root,alias)
            self.assertEqual(target.read_bytes(),b'preserve')

    def test_duplicate_json_fields_are_rejected(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'build/repack') as tmp:
            path = Path(tmp)/'spec.json'
            path.write_text('{"profile":"title-menu-v1","profile":"different"}')
            with self.assertRaisesRegex(ValueError, 'duplicate'):
                upgrade._json(path)

    def test_authored_compositor_abi_and_branches(self):
        code = upgrade._compositor(upgrade.TOKENS)
        self.assertEqual(len(code), upgrade.FUNCTION_SIZE)
        for initial in (0,1,2,255):
            for selector in range(3):
                calls, memory, abi = upgrade._trace(code, selector, initial, with_abi=True)
                if initial not in (0,1):
                    self.assertEqual(calls, [])
                else:
                    textures = [value for target,args,_ in calls
                                for value in (args if target == 0x1ABF90 else (args[6],) if target == 0x207E40 else ())]
                    self.assertEqual(textures, list(upgrade.textures.TITLE)+list(upgrade.textures.MENU[selector]))
                    self.assertEqual(sum(target == 0x1FB9F0 for target,_,_ in calls), int(initial == 0))
                self.assertEqual(memory[0x20000A], 1 if initial == 0 else initial)
                for register in (*range(16,24),28,30):
                    self.assertEqual(abi[register], (0x12340000+register,0x56780000+register))
                self.assertEqual(abi[29][0], 0x100000)
                self.assertEqual(abi[31][0], 0xFFFFFFFC)


@unittest.skipUnless(os.environ.get('EM_TEST_FULL') == '1', 'local original-file proof')
class RealTitleUpgradeTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT / 'build/repack')
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.tree = self.base / 'tree'
        for relative in (upgrade.LEAF, 'iso/files/SCUS_971.12'):
            target = self.tree / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            source = ROOT / 'build/repack/loose' / relative
            if not source.is_file():
                self.skipTest('original loose disc is unavailable')
            shutil.copyfile(source, target)
        self.boot = self.tree / 'iso/files/SCUS_971.12'

    def test_noop_native_and_function_trace(self):
        original = (self.tree / upgrade.LEAF).read_bytes()
        upgrade.export_views(self.tree, self.base / 'views')
        self.assertTrue(upgrade.upgrade_tree(self.tree, self.base / 'views/spec.json')['unchanged'])
        self.assertEqual(original, (self.tree / upgrade.LEAF).read_bytes())
        raw = self.boot.read_bytes()
        self.assertEqual(upgrade._patch_elf(raw, upgrade.TOKENS), (raw, []))
        offset = upgrade._elf_offset(raw, upgrade.FUNCTION, upgrade.FUNCTION_SIZE)
        original, authored = raw[offset:offset+upgrade.FUNCTION_SIZE], upgrade._compositor(upgrade.TOKENS)
        for initial in (0,1,2,255):
            for selector in range(3):
                left = upgrade._trace(original,selector,initial,with_abi=True)
                right = upgrade._trace(authored,selector,initial,with_abi=True)
                self.assertEqual([(target,args) for target,args,_ in left[0]], [(target,args) for target,args,_ in right[0]])
                self.assertEqual(left[1:], right[1:])
        receipt = upgrade.apply_boot_patches(self.boot, self.tree, self.base / 'noop.elf')
        self.assertTrue(receipt['byte_identical'])

    def test_real_twofold_texture_and_both_palette_directions(self):
        from export_startup import upload_module, texture
        old_local, old_covered = upload_module(self.tree / upgrade.LEAF)
        old_views = {n:texture(old_local,old_covered,t) for n,t in zip(upgrade.NAMES,upgrade.TOKENS)}
        upgrade.export_views(self.tree, self.base / 'views')
        source = self.base / 'views/new-game-selected.png'
        w, h, rgba = png.read(source)
        doubled = b''.join(rgba[(y//2*w+x//2)*4:(y//2*w+x//2)*4+4]
                           for y in range(h*2) for x in range(w*2))
        png.write(self.base / 'views/double.png', w*2, h*2, doubled)
        views = {n: dict(png=n+'.png', palette_size=16) for n in upgrade.NAMES[:4]}
        views['new-game-selected'] = dict(png='double.png', palette_size=256)
        spec = self.base / 'views/upgrade.json'
        spec.write_text(json.dumps(dict(profile=upgrade.PROFILE, views=views)))
        receipt = upgrade.upgrade_tree(self.tree, spec, quantize='median-cut')
        self.assertEqual(set(receipt['quantized']), set(upgrade.NAMES[:4]))
        info = upgrade.validate_tree(self.tree)
        with self.assertRaisesRegex(ValueError, 'shared title TEX0'):
            upgrade._patch_immediates(self.boot.read_bytes(), tuple(info['tex0s'][n] for n in upgrade.NAMES))
        native = self.tree / upgrade.LEAF
        local, covered = upload_module(native)
        for name, token in info['tex0s'].items():
            width, height, decoded = texture(local, covered, token)
            # Established forward extractor reads native bottom-up; PNG is display orientation.
            flipped = b''.join(decoded[y*width*4:(y+1)*width*4] for y in range(height-1,-1,-1))
            meta = json.loads((self.tree / upgrade.DESCRIPTOR).read_text())
            self.assertEqual(upgrade._hash(flipped), meta['decoded_sha256'][name])
            if name == 'new-game-selected':
                self.assertEqual((width,height,flipped), (w*2,h*2,doubled))
            elif name not in views:
                self.assertEqual((width,height,decoded), old_views[name])
        result = upgrade.apply_boot_patches(self.boot, self.tree, self.base / 'upgraded.elf')
        self.assertFalse(result['byte_identical'])
        raw = self.boot.read_bytes(); off = upgrade._elf_offset(raw, upgrade.FUNCTION, upgrade.FUNCTION_SIZE)
        self.assertTrue(all(off <= p < off+upgrade.FUNCTION_SIZE for p in result['changed_byte_offsets']))
        forged = json.loads((self.tree / upgrade.DESCRIPTOR).read_text())
        forged['patches'] = []
        (self.tree / upgrade.DESCRIPTOR).write_text(json.dumps(forged))
        with self.assertRaisesRegex(ValueError, 'schema'):
            upgrade.validate_tree(self.tree)


if __name__ == '__main__':
    unittest.main()
