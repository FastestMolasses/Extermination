"""Compile canonical code, apply verified stream cue edits, then pack assets."""
from pathlib import Path
import json

from . import archive, iso, source_build, streams


def build_disc(tree: Path, out: Path, *, stream_bundles=(), **options) -> dict:
    tree, out = Path(tree).resolve(), archive.safe_output(out)
    if out == tree or out.is_relative_to(tree) or tree.is_relative_to(out):
        raise ValueError("build-disc output must be separate from its input tree")
    # Fail malformed bundles before starting the compiler. Validate again
    # against its fresh linked/packaged result before changing any cue data.
    inspected = [streams.inspect_bundle(p, tree / "iso/files/SCUS_971.12") for p in stream_bundles]
    if len({p["kind"] for p in inspected}) != len(inspected):
        raise ValueError("only one bundle per stream kind can be applied")
    for bundle in inspected:
        original = tree / "iso/files/STREAM" / bundle["stream_file"]
        if archive.sha256_file(original) != bundle["original_stream_sha256"]:
            raise ValueError("stream bundle was decoded from a different loose-disc stream")
    built = source_build.build_sources(tree, out / "source", **options)
    out.mkdir(parents=True, exist_ok=True)
    packed = archive.pack_archive(tree / "archive", out / "DATA.DAT", out / "INDEX.IDX")
    overrides = {key: Path(path) for key, path in built["overrides"].items()}
    overrides.update({"DATA/DATA.DAT": out / "DATA.DAT", "DATA/INDEX.IDX": out / "INDEX.IDX"})
    cue_patch, resized = None, set()
    if stream_bundles:
        cue_patch = streams.apply_cue_patches(overrides["SCUS_971.12"], stream_bundles, out / "cue-boot/SCUS_971.12")
        overrides["SCUS_971.12"] = Path(cue_patch["output"])
        overrides.update({key: Path(path) for key, path in cue_patch["overrides"].items()})
        resized = set(cue_patch["overrides"])
    result = iso.pack(tree / "iso", out / "Extermination.iso", overrides=overrides, resized_streams=resized)
    receipt = {"source": built, "archive": packed, "cue_patch": cue_patch,
               "iso": {"image_path": str(out / "Extermination.iso"),
                       **{key: value for key, value in result.items() if key != "files"}}}
    (out / "build-disc.json").write_text(json.dumps(receipt, indent=2) + "\n")
    return receipt
