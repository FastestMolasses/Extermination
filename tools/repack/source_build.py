"""Build fresh boot/overlay executables in an isolated, ignored workspace.

The established matching-decomp pipeline still assembles explicitly unmatched
functions and data from the user's original disc. Its provenance audit is saved
alongside the build, so a byte-identical result is never described as all C.
"""
from __future__ import annotations

from contextlib import contextmanager
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile
import time

from .archive import ROOT, safe_output, sha256_file

MAIN_ROOT = ROOT.parent / "Extermination"
BUILD_LOCK = MAIN_ROOT / "build" / ".decomp_build.lock"
IMAGE = "exterm-permuter"
OVERLAYS = ("AREA00", "AREA01", "AREA02", "AREA03", "AREA04", "AREA06", "AREA07", "AREA08",
            "AREA11", "AREA13", "AREA14", "AREA15", "AREA16", "AREA17", "AREA18", "AREA19",
            "AREA20", "AREA21", "AREA22")


@contextmanager
def build_lock(path: Path = BUILD_LOCK, progress=print):
    """Cooperate with the main decomp build; release only the lock we acquired."""
    waiting = False
    while True:
        try:
            path.mkdir()
            break
        except FileExistsError:
            if not waiting:
                progress("source-build: waiting for the shared decomp build lock")
                waiting = True
            time.sleep(10)
    try:
        yield
    finally:
        path.rmdir()


def load_segment(raw: bytes) -> tuple[int, int, int]:
    """Return the single nonempty ELF32 little-endian PT_LOAD span."""
    if len(raw) < 52 or raw[:6] != b"\x7fELF\x01\x01":
        raise ValueError("expected ELF32 little-endian executable")
    offset, stride, count = struct.unpack_from("<I", raw, 28)[0], _u16(raw, 42), _u16(raw, 44)
    if stride < 32 or offset + stride * count > len(raw):
        raise ValueError("ELF program header table is out of bounds")
    spans = []
    for number in range(count):
        kind, begin, address, _physical, size, memory_size = struct.unpack_from("<6I", raw, offset + number * stride)
        if kind == 1 and size:
            if begin + size > len(raw) or memory_size < size:
                raise ValueError("ELF load span is out of bounds")
            spans.append((begin, address, size))
    if len(spans) != 1:
        raise ValueError("expected exactly one nonempty ELF load segment")
    return spans[0]


def _u16(raw: bytes, at: int) -> int:
    return struct.unpack_from("<H", raw, at)[0]


def package_boot(original: Path, linked: Path, output: Path) -> dict:
    """Combine the disc ELF envelope with fresh linked LOAD bytes, then verify.

    A mismatch fails instead of substituting original executable content.
    Headers, nonloaded sections and their padding have no C compilation source;
    keeping their original envelope also preserves the disc file byte identity.
    """
    original, linked, output = Path(original), Path(linked), safe_output(output)
    for source in (original, linked):
        if output == source.resolve() or (output.exists() and output.samefile(source)):
            raise ValueError("boot output aliases an input")
    old, fresh = original.read_bytes(), linked.read_bytes()
    old_offset, old_address, old_size = load_segment(old)
    new_offset, new_address, new_size = load_segment(fresh)
    if old_address != new_address or new_size < old_size:
        raise ValueError("linked boot LOAD address/size does not match the original envelope")
    # This linker emits one additional 0x80-byte zero block after the pinned
    # original load range. Accept that measured alignment quirk only; reject
    # larger or nonzero tails rather than silently discarding executable data.
    padding = new_size - old_size
    if padding not in (0, 0x80) or any(fresh[new_offset + old_size:new_offset + new_size]):
        raise ValueError("linked boot has unsupported data beyond the original LOAD range")
    payload = fresh[new_offset:new_offset + old_size]
    if payload != old[old_offset:old_offset + old_size]:
        raise ValueError("fresh linked boot LOAD differs from the original; no fallback copy was written")
    packaged = old[:old_offset] + payload + old[old_offset + old_size:]
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(packaged)
    return dict(path=str(output), sha256=hashlib.sha256(packaged).hexdigest(), byte_identical=packaged == old,
                linked_load_bytes=old_size, linked_segment_bytes=new_size,
                discarded_linker_zero_padding_bytes=padding, preserved_envelope_bytes=len(old) - old_size,
                linked_load_sha256=hashlib.sha256(payload).hexdigest(), load_address=old_address)


def _module(path: Path, name: str):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _stage_sources(workspace: Path, toolchain_root: Path) -> dict:
    listed = subprocess.check_output(["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z",
                                      "--", "src", "config", "tools/decomp", "tools/overlay", "tools/eegcc"],
                                     cwd=ROOT).split(b"\0")
    prefixes = ("src/", "config/", "tools/decomp/", "tools/overlay/", "tools/eegcc/")
    snapshot = hashlib.sha256()
    source_count = 0
    for encoded in listed:
        if not encoded:
            continue
        name = os.fsdecode(encoded)
        if not name.startswith(prefixes):
            continue
        source, destination = ROOT / name, workspace / name
        if not source.is_file():
            raise ValueError(f"tracked source is missing: {name}")
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
        snapshot.update(encoded + b"\0" + destination.read_bytes())
        source_count += 1
    # These locally installed toolchains are ignored inputs, never outputs in
    # the shared checkout. Copying makes every container write stay isolated.
    def dependency_copy(source, destination):
        # Keep tracked wrappers from this source snapshot even when the local
        # toolchain directory also contains an older copy of the wrapper.
        if Path(destination).exists():
            return str(destination)
        return shutil.copy2(source, destination)

    for name in ("bin", "eegcc", "mwccps2", "mwccps2-233", "mwccps2-24"):
        source = toolchain_root / "tools" / name
        if not source.is_dir():
            raise ValueError(f"local compiler tools are missing: {source}")
        shutil.copytree(source, workspace / "tools" / name, dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns("__pycache__", "*.pyc"), copy_function=dependency_copy)
    for name in ("mwccps2-30", "mwccps2-301"):
        source = toolchain_root / "tools" / name
        if source.is_dir():
            shutil.copytree(source, workspace / "tools" / name)
    worker = ROOT / "tools/repack/source_compile.py"
    target = workspace / "tools/repack/source_compile.py"
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(worker, target)
    snapshot.update(b"tools/repack/source_compile.py\0" + target.read_bytes())
    source_count += 1
    return dict(source_files=source_count, source_snapshot_sha256=snapshot.hexdigest(),
                git_head=subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip())


def _require_objects(paths: list[Path], label: str) -> int:
    missing = [path.name for path in paths if not path.is_file() or path.stat().st_size < 52]
    if missing:
        raise ValueError(f"{label}: {len(missing)} fresh compile objects missing; build cannot fall back silently")
    return len(paths)


def _undefined_symbols(path: Path) -> set[str]:
    """Read ELF32 symbol names only, without disassembly or external tools."""
    raw = path.read_bytes()
    if len(raw) < 52 or raw[:6] != b"\x7fELF\x01\x01":
        raise ValueError(f"invalid compiled ELF object: {path.name}")
    start = struct.unpack_from("<I", raw, 32)[0]
    stride, count = struct.unpack_from("<HH", raw, 46)
    if stride < 40 or start + stride * count > len(raw):
        raise ValueError("invalid compiled ELF section table")
    sections = [struct.unpack_from("<10I", raw, start + stride * i) for i in range(count)]
    undefined = set()
    for section in sections:
        if section[1] != 2:
            continue
        offset, size, link, entry_size = section[4], section[5], section[6], section[9]
        if entry_size < 16 or size % entry_size or offset + size > len(raw) or link >= count:
            raise ValueError("invalid compiled ELF symbol table")
        strings = sections[link]
        if strings[4] + strings[5] > len(raw):
            raise ValueError("invalid compiled ELF string table")
        names = raw[strings[4]:strings[4] + strings[5]]
        for at in range(offset, offset + size, entry_size):
            name, _value, _size, _info, _other, index = struct.unpack_from("<IIIBBH", raw, at)
            if index == 0 and name:
                end = names.find(b"\0", name)
                if name >= len(names) or end < 0:
                    raise ValueError("invalid compiled ELF symbol name")
                undefined.add(names[name:end].decode("ascii"))
    return undefined


def _corroborated_aliases(area: str, undefined: set[str], old_script: str,
                         fresh_boundaries: set[int], fresh_names: set[str]) -> dict[str, int]:
    """Keep an old encoded name only when two independent layouts agree."""
    known = {name: int(address, 16) for name, address in re.findall(
        r"^\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+)\s*;", old_script, re.MULTILINE)}
    result = {}
    for name in sorted(undefined - fresh_names):
        match = re.fullmatch(rf"func_overlay_{re.escape(area)}_([0-9A-Fa-f]{{8}})", name)
        if match:
            address = int(match.group(1), 16)
            if known.get(name) == address and address in fresh_boundaries:
                result[name] = address
    return result


def _overlay_aliases(workspace: Path, area: str) -> dict[str, int]:
    """Restore renamed splat aliases in generated linker inputs only."""
    directory = workspace / "build/overlays" / area
    fill = _module(workspace / "tools/overlay/fill_overlay.py", "repack_alias_layout")
    entries = fill.collect_functions(directory / "asm/matchings" / area / "code")
    undefined = set().union(*(_undefined_symbols(path) for path in (directory / "obj").glob("*.o")))
    old_script = (ROOT / "config/overlays" / f"{area}.lds").read_text()
    aliases = _corroborated_aliases(area, undefined, old_script,
                                  {address for address, _name, _path in entries},
                                  {name for _address, name, _path in entries})
    if aliases:
        path = directory / "undefined_funcs_auto.txt"
        previous = path.read_text() if path.exists() else ""
        additions = "".join(f"{name} = 0x{address:08X};\n" for name, address in aliases.items()
                            if not re.search(rf"^\s*{re.escape(name)}\s*=", previous, re.MULTILINE))
        path.write_text(previous.rstrip() + "\n" + additions)
    return aliases


def _overlay_selection(workspace: Path, area: str, compiled: list[Path]) -> dict:
    directory = workspace / "build/overlays" / area
    absorption = json.loads((directory / "filler/_absorbed.json").read_text())
    absorbed = {name for names in absorption.values() for name in names}
    pieces = {path.stem for path in (directory / "asm/matchings" / area / "code").glob("*.s")}
    selected = pieces - absorbed
    compiled_names = {path.stem for path in compiled}
    bypassed = [dict(object=name + ".o", reason="absorbed by another compiled function" if name in absorbed
                    else "canonical splat piece was renamed; canonical filler selects original assembly")
                for name in sorted(compiled_names - selected)]
    return dict(selected_compiled_objects=len(compiled_names & selected), bypassed_compiled_objects=bypassed,
                original_assembly_pieces=len(selected - compiled_names), absorbed_pieces=len(absorbed))


def _compile_checkpoint(workspace: Path) -> dict:
    paths = [*workspace.glob("build/obj/*.o"), *workspace.glob("build/overlays/*/obj/*.o"),
             *workspace.glob("build/source-*-compile.json")]
    digest = hashlib.sha256()
    for path in sorted(paths):
        digest.update(str(path.relative_to(workspace)).encode() + b"\0" + bytes.fromhex(sha256_file(path)))
    return dict(files=len(paths), sha256=digest.hexdigest())


def _validate_resume(tree: Path, out_dir: Path, toolchain_root: Path, receipt: dict) -> dict:
    """Resume an overlay-link failure only after checking its fresh inputs."""
    workspace = out_dir / "workspace"
    stages = receipt.get("stages", [])
    if (receipt.get("status") != "failed" or not stages or stages[-1]["exit_code"] == 0
            or stages[-1]["name"] not in {f"{area}-link" for area in OVERLAYS}
            or any(stage["exit_code"] for stage in stages[:-1])):
        raise ValueError("resume requires a single overlay-link failure after successful fresh compilation")
    if not {"boot-compile", "boot-link", "boot-provenance", "overlay-compile"} <= {s["name"] for s in stages[:-1]}:
        raise ValueError("resume lacks successful fresh compilation/link receipts")
    checked = 0
    with tempfile.TemporaryDirectory(prefix="resume-check-", dir=out_dir) as name:
        expected = Path(name)
        snapshot = _stage_sources(expected, toolchain_root)
        if snapshot["source_snapshot_sha256"] != receipt["source_snapshot_sha256"]:
            raise ValueError("source snapshot changed; a fresh build is required")
        for path in expected.rglob("*"):
            if not path.is_file() or "__pycache__" in path.parts or path.suffix == ".pyc":
                continue
            relative = path.relative_to(expected)
            # Canonical linkers regenerate these output scripts in staging.
            if (relative == Path("config/SCUS_971.12.lcf")
                    or relative.parent == Path("config/overlays") and relative.suffix == ".lds"):
                continue
            staged = workspace / relative
            if not staged.is_file() or sha256_file(staged) != sha256_file(path):
                raise ValueError(f"staged source/tool input changed: {relative}")
            checked += 1
    for kind in ("boot", "overlays"):
        report = json.loads((workspace / f"build/source-{kind}-compile.json").read_text())
        if report.get("kind") != kind or report.get("failures") or report.get("units", 0) <= 0:
            raise ValueError(f"invalid successful {kind} compile receipt")
    for relative in ["SCUS_971.12", *[f"OVERLAY/{area}.BIN" for area in OVERLAYS]]:
        staged = workspace / ("config/SCUS_971.12" if relative == "SCUS_971.12" else f"extract/{relative}")
        if sha256_file(staged) != sha256_file(tree / "iso/files" / relative):
            raise ValueError("original executable input changed; fresh build required")
    boot_driver = _module(workspace / "tools/decomp/build.py", "repack_resume_boot")
    _require_objects([workspace / "build/obj" / f"{name}.o" for name in boot_driver.units()], "resumed boot")
    current = _compile_checkpoint(workspace)
    historical = receipt.get("compile_checkpoint")
    if historical and historical != current:
        raise ValueError("fresh compiler products changed since their successful checkpoint")
    return dict(checked_source_and_tool_files=checked, source_snapshot_sha256=snapshot["source_snapshot_sha256"],
                successful_compile_receipts=["boot", "overlays"], failed_stage=stages[-1]["name"],
                compile_checkpoint=current, historical_object_checkpoint_verified=bool(historical),
                recovery_note=None if historical else "Legacy failed attempt predates hash checkpoints. "
                "Fresh-object provenance comes from its originally empty workspace and successful compile receipts; "
                "this digest records present reuse identity, not a retroactive historical hash proof.")


def build_sources(tree_dir: Path, out_dir: Path, *, toolchain_root: Path = MAIN_ROOT,
                  progress=print, resume: bool = False) -> dict:
    """Freshly compile, link, prove and package boot plus all nineteen overlays.

    ``tree_dir`` is an unpack-disc tree; ``out_dir`` is a new build/repack child.
    The return value's ``overrides`` can be passed straight to iso.pack, together
    with DATA/INDEX freshly rebuilt by archive.pack_archive.
    """
    tree, out_dir, toolchain_root = Path(tree_dir).resolve(), safe_output(out_dir), Path(toolchain_root).resolve()
    if out_dir.exists() and any(out_dir.iterdir()) and not resume:
        raise ValueError("source-build output must be a new or empty directory")
    if out_dir.is_relative_to(tree) or tree.is_relative_to(out_dir):
        raise ValueError("source-build output must be separate from its loose input tree")
    original_dir = tree / "iso" / "files"
    original_boot = original_dir / "SCUS_971.12"
    originals = {area: original_dir / "OVERLAY" / f"{area}.BIN" for area in OVERLAYS}
    if not original_boot.is_file() or any(not path.is_file() for path in originals.values()):
        raise ValueError("unpack-disc tree must contain the boot ELF and all 19 overlays")
    python = ROOT / ".venv" / "bin" / "python"
    if not python.is_file() or shutil.which("container") is None:
        raise ValueError("source build requires the existing project .venv and Apple container CLI")
    out_dir.mkdir(parents=True, exist_ok=True)
    workspace, logs = out_dir / "workspace", out_dir / "logs"
    workspace.mkdir(exist_ok=resume)
    logs.mkdir(exist_ok=resume)
    receipt = dict(schema="extermination-source-build-v1", stages=[], status="running", toolchain_image=IMAGE,
                   compiler_workers=4, lock=str(BUILD_LOCK), output_dir=str(out_dir), overrides={})
    receipt_path = out_dir / "source-build.json"
    if resume:
        receipt = json.loads(receipt_path.read_text())

    def save():
        receipt_path.write_text(json.dumps(receipt, indent=2) + "\n")

    def run(name: str, command: list[str]):
        progress(f"source-build: {name}")
        began = time.monotonic()
        log_path = logs / f"{len(receipt['stages']):02d}-{name}.log"
        with log_path.open("wb") as stream:
            result = subprocess.run(command, cwd=workspace, stdout=stream, stderr=subprocess.STDOUT)
        receipt["stages"].append(dict(name=name, exit_code=result.returncode,
                                      seconds=round(time.monotonic() - began, 3), log=str(log_path)))
        save()
        if result.returncode:
            raise ValueError(f"source-build {name} failed; inspect local log {log_path}")

    def container(script: list[str]) -> list[str]:
        return ["container", "run", "--rm", "-v", f"{workspace}:/work", "-w", "/work", IMAGE, *script]

    def _fresh_boot_and_compile():
        progress("source-build: staging a fresh source and toolchain snapshot")
        receipt.update(_stage_sources(workspace, toolchain_root))
        shutil.copy2(original_boot, workspace / "config" / "SCUS_971.12")
        (workspace / "extract" / "OVERLAY").mkdir(parents=True)
        for area, source in originals.items():
            shutil.copy2(source, workspace / "extract" / "OVERLAY" / f"{area}.BIN")
        run("boot-setup", [str(python), "tools/decomp/build.py", "setup"])
        run("boot-compile", container(["python3", "tools/repack/source_compile.py", "boot", "--jobs", "4"]))
        boot_driver = _module(workspace / "tools/decomp/build.py", "repack_fresh_boot_driver")
        boot_units = boot_driver.units()
        receipt["fresh_boot_objects"] = _require_objects(
            [workspace / "build" / "obj" / f"{name}.o" for name in boot_units], "boot")
        if not boot_units:
            raise ValueError("fresh boot build selected no source units")
        # Match verify_all's single-worker fill: parallel copying through
        # the container bind mount can intermittently report EDEADLK.
        run("boot-fill", container(["python3", "tools/decomp/fill_unmatched.py", "--clean", "--jobs", "1"]))
        run("boot-link", container(["python3", "tools/decomp/link.py", "--no-fill"]))
        audit_path = out_dir / "boot-provenance.json"
        run("boot-provenance", [str(python), "tools/decomp/audit_link_provenance.py", "--output", str(audit_path)])
        audit = json.loads(audit_path.read_text())
        if any(row["route"] == "original_assembly_missing_object" and row["source"] != "missing"
               for row in audit["rows"]):
            raise ValueError("boot provenance includes missing-object assembly fallback")
        receipt["boot_provenance"] = {key: value for key, value in audit.items() if key != "rows"}
        receipt["boot"] = package_boot(original_boot, workspace / "elf" / "SCUS_971.12.elf",
                                        out_dir / "files" / "SCUS_971.12")
        receipt["overrides"]["SCUS_971.12"] = receipt["boot"]["path"]
        for area in OVERLAYS:
            run(f"{area}-splat", [str(python), "-m", "splat", "split", f"config/overlays/{area}.yaml"])
        run("overlay-compile", container(["python3", "tools/repack/source_compile.py", "overlays", "--jobs", "4"]))
        receipt["compile_checkpoint"] = _compile_checkpoint(workspace)
        save()

    def _link_overlays():
        overlay_compiler = _module(workspace / "tools/overlay/compile_overlay_src.py", "repack_fresh_overlay_driver")
        receipt["overlays"] = []
        for area in OVERLAYS:
            sources = list((workspace / "src" / "overlays" / area).glob("*.c"))
            compiled = [source for source in sources if not overlay_compiler.is_nearmiss(source)]
            count = _require_objects([workspace / "build" / "overlays" / area / "obj" / f"{source.stem}.o"
                                      for source in compiled], area)
            aliases = _overlay_aliases(workspace, area)
            run(f"{area}-link", container(["python3", "tools/overlay/link_overlay.py", area]))
            linked = workspace / "build" / "overlays" / area / f"{area}.BIN"
            if sha256_file(linked) != sha256_file(originals[area]):
                raise ValueError(f"{area}: fresh linked overlay differs from original")
            output = out_dir / "files" / "OVERLAY" / f"{area}.BIN"
            output.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(linked, output)
            receipt["overrides"][f"OVERLAY/{area}.BIN"] = str(output)
            receipt["overlays"].append(dict(area=area, path=str(output), sha256=sha256_file(output),
                byte_identical=True, fresh_source_objects=count, source_nearmiss_count=len(sources) - count,
                preserved_header_bytes=64, linked_payload_bytes=output.stat().st_size - 64,
                restored_symbol_aliases=aliases, **_overlay_selection(workspace, area, compiled)))
            save()

    if not resume:
        save()
    try:
        with build_lock(progress=progress):
            if resume:
                progress("source-build: validating preserved fresh sources, toolchains and compiler receipts")
                receipt["resume_validation"] = _validate_resume(tree, out_dir, toolchain_root, receipt)
                receipt["compile_checkpoint"] = receipt["resume_validation"]["compile_checkpoint"]
                receipt.setdefault("prior_failed_stages", []).append(receipt["stages"].pop())
                receipt["prior_error"] = receipt.pop("error", None)
                receipt["status"] = "running"
                receipt["boot"] = package_boot(original_boot, workspace / "elf/SCUS_971.12.elf",
                                                out_dir / "files/SCUS_971.12")
                save()
            else:
                _fresh_boot_and_compile()
            _link_overlays()
            receipt["status"] = "complete"
            save()
    except BaseException as exc:
        receipt["status"] = "failed"
        receipt["error"] = str(exc)
        save()
        raise
    progress("source-build: boot and 19 overlays verified byte-identical")
    return dict(receipt, receipt_path=str(receipt_path))
