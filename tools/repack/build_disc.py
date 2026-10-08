"""Compile canonical code, apply verified stream cue edits, then pack assets."""
from pathlib import Path
import hashlib
import json
import os
import tempfile

from . import archive, disc_edits, iso, source_build, streams, texture_upgrade


def _combine_boot_patches(fresh: Path, patches: list[dict], output: Path) -> dict:
    """Merge independently verified patches; even equal overlapping edits fail."""
    if Path(output).is_symlink():
        raise ValueError("combined boot output must not be a symlink")
    output = archive.safe_output(output)
    sources = [Path(fresh), *(Path(p['output']) for p in patches)]
    archive._distinct_outputs([output], sources)
    original = Path(fresh).read_bytes()
    digest = hashlib.sha256(original).hexdigest()
    rebuilt, changed = bytearray(original), set()
    for patch in patches:
        path = Path(patch['output'])
        raw = path.read_bytes()
        if (len(raw) != len(original) or patch['original_elf_sha256'] != digest
                or hashlib.sha256(raw).hexdigest() != patch['sha256']):
            raise ValueError("boot patch does not match its canonical source or receipt")
        offsets = {i for i, (old, new) in enumerate(zip(original, raw)) if old != new}
        if offsets & changed:
            raise ValueError("stream and texture boot patches overlap")
        for offset in offsets:
            rebuilt[offset] = raw[offset]
        changed.update(offsets)
    output.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix='.mod-boot-', dir=output.parent)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(rebuilt)
        os.replace(temporary, output)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    return dict(output=str(output), sha256=archive.sha256_file(output), original_elf_sha256=digest,
                byte_identical=not changed, changed_byte_offsets=sorted(changed))


def build_disc(tree: Path, out: Path, *, stream_bundles=(), **options) -> dict:
    tree, out = Path(tree).resolve(), archive.safe_output(out)
    if out == tree or out.is_relative_to(tree) or tree.is_relative_to(out):
        raise ValueError("build-disc output must be separate from its input tree")
    # Fail malformed bundles before starting the compiler. Validate again
    # against its fresh linked/packaged result before changing any cue data.
    stream_bundles = list(stream_bundles)
    inspected = [streams.inspect_bundle(p, tree / "iso/files/SCUS_971.12") for p in stream_bundles]
    tree_streams = disc_edits.validate_tree(tree)
    all_profiles = [*inspected, *tree_streams]
    if len({p["kind"] for p in all_profiles}) != len(all_profiles):
        raise ValueError("only one bundle per stream kind can be applied")
    for bundle in inspected:
        original = tree / "iso/files/STREAM" / bundle["stream_file"]
        if archive.sha256_file(original) != bundle["original_stream_sha256"]:
            raise ValueError("stream bundle was decoded from a different loose-disc stream")
    texture_plan = texture_upgrade.validate_tree(tree)
    local_bundles = disc_edits.materialize_stream_bundles(tree, out / 'stream-bundles', resume=options.get('resume', False))
    if len(local_bundles) != len(tree_streams):
        raise ValueError('tree stream instructions changed after build preflight')
    for bundle, plan in zip(local_bundles, tree_streams):
        local = streams.inspect_bundle(bundle, tree / 'iso/files/SCUS_971.12')
        if any(local.get(key) != value for key, value in plan.items() if key != 'stream_path'):
            raise ValueError('tree stream changed after build preflight')
    stream_bundles.extend(local_bundles)
    built = source_build.build_sources(tree, out / "source", **options)
    out.mkdir(parents=True, exist_ok=True)
    packed = archive.pack_archive(tree / "archive", out / "DATA.DAT", out / "INDEX.IDX")
    overrides = {key: Path(path) for key, path in built["overrides"].items()}
    overrides.update({"DATA/DATA.DAT": out / "DATA.DAT", "DATA/INDEX.IDX": out / "INDEX.IDX"})
    canonical_boot = overrides['SCUS_971.12']
    cue_patch, texture_patch, combined_boot, resized = None, None, None, set()
    if stream_bundles:
        cue_patch = streams.apply_cue_patches(canonical_boot, stream_bundles, out / "cue-boot/SCUS_971.12")
        overrides["SCUS_971.12"] = Path(cue_patch["output"])
        overrides.update({key: Path(path) for key, path in cue_patch["overrides"].items()})
        resized = set(cue_patch["overrides"])
    if texture_plan['active']:
        if texture_upgrade.validate_tree(tree) != texture_plan:
            raise ValueError('texture upgrades changed after build preflight')
        texture_patch = texture_upgrade.apply_boot_patches(canonical_boot, tree, out / 'texture-boot/SCUS_971.12')
        combined_boot = _combine_boot_patches(canonical_boot, [p for p in (cue_patch, texture_patch) if p],
                                             out / 'mod-boot/SCUS_971.12')
        overrides['SCUS_971.12'] = Path(combined_boot['output'])
    result = iso.pack(tree / "iso", out / "Extermination.iso", overrides=overrides, resized_streams=resized)
    receipt = {"source": built, "archive": packed, "cue_patch": cue_patch,
               "iso": {"image_path": str(out / "Extermination.iso"),
                       **{key: value for key, value in result.items() if key != "files"}}}
    if texture_patch is not None:
        receipt.update(texture_patch=texture_patch, combined_boot=combined_boot)
    (out / "build-disc.json").write_text(json.dumps(receipt, indent=2) + "\n")
    return receipt
