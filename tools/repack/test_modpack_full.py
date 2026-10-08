"""Opt-in two-build proof: direct edits equal independently applied mod packs.

Requires EM_TEST_MODPACK_BUILD=1, EM_TEST_ISO, the canonical build/repack/loose
tree and the locally generated title/model examples. Runs two real fresh source
builds; never launches a native game or emulator. All modified clone files are
atomically replaced so hardlinks never write through to the canonical tree.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import tempfile
import time
import unittest

from tools.repack import archive, build_disc, iso, modpack
from tools.repack.test_full import udf_hashes


def atomic_write(path: Path, raw: bytes):
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary = tempfile.mkstemp(prefix='.modpack-proof-', dir=path.parent)
    try:
        with os.fdopen(descriptor, 'wb') as output:
            output.write(raw)
        os.replace(temporary, path)
    finally:
        Path(temporary).unlink(missing_ok=True)


def tree_hashes(tree: Path) -> dict[str, str]:
    paths = sorted(tree.rglob('*'))
    if any(path.is_symlink() for path in paths):
        raise ValueError('canonical proof tree must contain regular files, not symlinks')
    return {str(path.relative_to(tree)): archive.sha256_file(path) for path in paths if path.is_file()}


def identity(values: dict) -> str:
    return hashlib.sha256(json.dumps(values, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


@unittest.skipUnless(os.environ.get('EM_TEST_MODPACK_BUILD') == '1',
                     'set EM_TEST_MODPACK_BUILD=1 for two fresh BYO-disc source builds')
class FullModpackBuildTests(unittest.TestCase):
    def test_two_packs_equal_direct_edit_with_fresh_sources_and_both_namespaces(self):
        self.assertTrue(os.environ.get('EM_TEST_ISO'), 'EM_TEST_ISO must name the original BYO disc')
        image = Path(os.environ['EM_TEST_ISO']).resolve()
        canonical = archive.ROOT / 'build/repack/loose'
        root = archive.safe_output(archive.ROOT / 'build/repack/modpack-proof')
        root.mkdir(parents=True, exist_ok=True)
        run = root / ('run-' + str(time.time_ns()))
        run.mkdir()
        started = time.monotonic()
        receipt = dict(schema='extermination-modpack-build-proof-v1', status='running', run=str(run),
                       source_iso=str(image), stages=[], packs=[], cleanup=[])

        def save():
            receipt['elapsed_seconds'] = round(time.monotonic() - started, 3)
            raw = (json.dumps(receipt, indent=2) + '\n').encode()
            atomic_write(run / 'receipt.json', raw)
            atomic_write(root / 'receipt.json', raw)

        def stage(name):
            receipt['stages'].append(dict(name=name, elapsed_seconds=round(time.monotonic() - started, 3)))
            save()
            print('Modpack full proof: ' + name, flush=True)

        def remove(path):
            path = Path(path)
            self.assertTrue(path.is_relative_to(run))
            self.assertFalse(path.is_symlink())
            if path.is_dir():
                shutil.rmtree(path)
            elif path.exists():
                path.unlink()
            receipt['cleanup'].append(str(path.relative_to(run)))

        def source_report(report):
            source = report['source']
            self.assertEqual(source['status'], 'complete')
            self.assertEqual(len(source['stages']), 44)
            self.assertTrue(all(row['exit_code'] == 0 for row in source['stages']))
            self.assertEqual(len(source['overlays']), 19)
            return dict(status=source['status'], stages=[dict(name=r['name'], exit_code=r['exit_code'])
                        for r in source['stages']], fresh_boot_objects=source['fresh_boot_objects'],
                        source_snapshot_sha256=source['source_snapshot_sha256'],
                        boot_sha256=source['boot']['sha256'],
                        overlay_sha256={r['area']: r['sha256'] for r in source['overlays']},
                        boot_provenance=source['boot_provenance'])

        before = None
        try:
            stage('validate original and canonical tree')
            base = modpack.BaseDisc(image)
            receipt['source_iso_sha256'] = base.info['image_sha256']
            original_files = {row['path']: row['sha256'] for row in base.info['files']}
            self.assertEqual(len(original_files), 43)
            before = tree_hashes(canonical)
            receipt['canonical_file_count'] = len(before)
            receipt['canonical_tree_sha256'] = identity(before)
            edits = {
                'title': ('archive/chunk01/transient00.bin', archive.ROOT / 'build/repack/texture-mod/title-edited.bin'),
                'model': ('archive/chunk28/f00_id3b.bin', archive.ROOT / 'build/repack/model-edit/player-edited.bin'),
            }
            receipt['leaves'] = {}
            for name, (target, edited) in edits.items():
                self.assertTrue(edited.is_file(), f'missing local {name} edit')
                self.assertEqual(before[target], base.hash(target), 'canonical leaf must remain original')
                self.assertNotEqual(archive.sha256_file(edited), before[target])
                receipt['leaves'][target] = dict(original_sha256=before[target], edited_sha256=archive.sha256_file(edited))
            title_old = base.read(edits['title'][0])
            title_new = edits['title'][1].read_bytes()
            self.assertEqual(len(title_old), len(title_new))
            self.assertEqual(sum(a != b for a, b in zip(title_old, title_new)), 43)

            stage('make independent title and model packs')
            clones, packs = {}, []
            for name, (target, edited) in edits.items():
                clone = run / (name + '-tree')
                shutil.copytree(canonical, clone, copy_function=os.link)
                atomic_write(clone / target, edited.read_bytes())
                clones[name] = clone
                package = run / (name + '.emmod')
                result = modpack.make_modpack(image, clone, package)
                self.assertEqual(result['changes'], 1)
                self.assertEqual(result['delta_files'], 1)
                self.assertEqual(result['authored_files'], 0)
                receipt['packs'].append(result)
                packs.append(package)
                save()
            stage('verify both packs and reject a target conflict')
            receipt['verification'] = modpack.verify_modpack(image, packs)
            self.assertTrue(receipt['verification']['verified'])
            with self.assertRaisesRegex(ValueError, 'conflict'):
                modpack.verify_modpack(image, [packs[0], packs[0]])
            receipt['conflict_rejected'] = True

            direct_tree = run / 'direct-tree'
            shutil.copytree(canonical, direct_tree, copy_function=os.link)
            for target, edited in edits.values():
                atomic_write(direct_tree / target, edited.read_bytes())
            direct = run / 'direct'
            stage('fresh direct-edit source build (1 of 2)')
            direct_report = build_disc.build_disc(direct_tree, direct)
            receipt['direct_source'] = source_report(direct_report)
            direct_info = iso.inventory(direct / 'Extermination.iso')
            direct_files = {row['path']: row['sha256'] for row in direct_info['files']}
            self.assertEqual(set(direct_files), set(original_files))
            self.assertEqual(direct_info['image_sha256'], direct_report['iso']['image_sha256'])
            self.assertEqual(receipt['direct_source']['boot_sha256'], original_files['SCUS_971.12'])
            receipt['direct_iso_sha256'] = direct_info['image_sha256']
            receipt['direct_files'] = direct_files
            stage('record direct hashes and remove direct large artifacts before apply')
            for path in (direct / 'Extermination.iso', direct / 'DATA.DAT', direct / 'INDEX.IDX',
                         direct / 'source/workspace', direct_tree, *clones.values()):
                remove(path)
            save()

            stage('apply both packs with fresh source build (2 of 2)')
            installation = modpack.apply_modpack(image, packs, run / 'applied')
            receipt['installation'] = installation
            applied_receipt = json.loads(Path(installation['build_receipt']).read_text())
            receipt['applied_source'] = source_report(applied_receipt)
            applied_iso = Path(installation['image_path'])
            applied_info = iso.inventory(applied_iso)
            applied_files = {row['path']: row['sha256'] for row in applied_info['files']}
            self.assertEqual(applied_info['image_sha256'], receipt['direct_iso_sha256'])
            self.assertEqual(installation['image_sha256'], receipt['direct_iso_sha256'])
            self.assertEqual(applied_files, direct_files)
            self.assertEqual(receipt['direct_source']['source_snapshot_sha256'],
                             receipt['applied_source']['source_snapshot_sha256'])
            self.assertEqual(receipt['applied_source']['boot_sha256'], original_files['SCUS_971.12'])
            stage('independent UDF oracle for all 43 applied files')
            udf = udf_hashes(applied_iso, list(applied_files))
            self.assertEqual(udf, applied_files)
            receipt['files'] = [dict(path=name, original_sha256=original_files[name],
                               direct_sha256=direct_files[name], applied_iso_sha256=applied_files[name],
                               applied_udf_sha256=udf[name]) for name in sorted(applied_files)]
            self.assertEqual([r['path'] for r in receipt['files'] if r['original_sha256'] != r['direct_sha256']],
                             ['DATA/DATA.DAT'])
            receipt.update(applied_iso_sha256=applied_info['image_sha256'], exact_iso_match=True,
                           all_43_iso_and_udf_files_match=True)
            for path in (run / 'applied/disc/source/workspace', run / 'applied/disc/DATA.DAT',
                         run / 'applied/disc/INDEX.IDX'):
                remove(path)
            stage('verify canonical hardlink sources remained unchanged')
            self.assertEqual(tree_hashes(canonical), before)
            receipt['canonical_unchanged'] = True
            receipt['status'] = 'complete'
            save()
            print(f'Modpack full proof passed: {root / "receipt.json"}', flush=True)
        except BaseException as error:
            receipt['status'] = 'failed'
            receipt['error'] = f'{type(error).__name__}: {error}'
            if before is not None:
                receipt['canonical_unchanged'] = tree_hashes(canonical) == before
            save()
            raise


if __name__ == '__main__':
    unittest.main()
