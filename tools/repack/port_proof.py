"""Run existing native-port exporters and its hidden title fixture in isolation.

No port source, executable, or shared assets are changed. The receipt separates
copied support assets from exports made from the supplied disc. This is a title
compatibility/visual proof, not a claim that the port has loaded every mod.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import time

from .archive import ROOT, safe_output, sha256_file
from .iso import inventory


def _snapshot(root: Path) -> dict:
    return {str(path.relative_to(root)): sha256_file(path)
            for path in sorted(root.rglob("*")) if path.is_file()}


def _bmp_rgb(path: Path) -> tuple[int, int, bytes]:
    data = path.read_bytes()
    if data[:2] != b"BM" or len(data) < 54:
        raise ValueError("port capture is not a BMP")
    offset = struct.unpack_from("<I", data, 10)[0]
    width, height, planes, bits, compression = struct.unpack_from("<iiHHI", data, 18)
    if width <= 0 or not height or planes != 1 or bits not in (24, 32) or compression:
        raise ValueError("unsupported port BMP capture layout")
    stride = (width * (bits // 8) + 3) & ~3
    if offset + abs(height) * stride > len(data):
        raise ValueError("truncated port BMP")
    pixels = bytearray()
    for y in range(abs(height)):
        row = abs(height) - 1 - y if height > 0 else y
        for x in range(width):
            at = offset + row * stride + x * (bits // 8)
            pixels.extend(data[at:at + 3][::-1])
    return width, abs(height), bytes(pixels)


def _magenta(rgb: bytes) -> int:
    return sum(r > 100 and b > 100 and g < 80 for r, g, b in zip(rgb[::3], rgb[1::3], rgb[2::3]))


def _logged_run(receipt: dict, output: Path, name, command, *, cwd=ROOT, env=None, timeout=180):
    print(f"port-proof: {name}", flush=True)
    child_env = dict(os.environ if env is None else env)
    child_env["PYTHONDONTWRITEBYTECODE"] = "1"
    log = output / "logs" / (name + ".log")
    if log.exists():
        raise ValueError("port proof command log already exists; preserve previous attempts")
    started = time.monotonic()
    item = {"name": name, "argv": [str(arg) for arg in command], "cwd": str(cwd), "log": str(log)}
    receipt["commands"].append(item)
    def save():
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    save()
    try:
        with log.open("w") as stream:
            result = subprocess.run(item["argv"], cwd=cwd, env=child_env, stdout=stream, stderr=subprocess.STDOUT, timeout=timeout)
        item["returncode"] = result.returncode
    except subprocess.TimeoutExpired:
        item["timed_out"] = True
        raise RuntimeError(f"{name} exceeded {timeout}s; see {log}")
    finally:
        item["seconds"] = round(time.monotonic() - started, 3); save()
    if result.returncode:
        raise RuntimeError(f"existing port exporter/fixture {name} failed ({result.returncode}); see {log}")


def _native_capture(receipt: dict, output: Path, executable: Path, *, attempt="native-title"):
    capture = output / "capture"
    if any(capture.iterdir()):
        raise ValueError("port proof capture directory is occupied; preserve previous captures")
    env = {key: value for key, value in os.environ.items() if not key.startswith("EM_")}
    env.update(EM_HEADLESS="1", EM_STARTUP_TEST="skip", EM_STARTUP_CAPTURE_DIR=str(capture))
    _logged_run(receipt, output, attempt, [executable], cwd=output, env=env, timeout=120)
    log = (output / "logs" / (attempt + ".log")).read_text()
    if "startup test: PASS (logos, movie, title navigation/clamps)" not in log:
        raise RuntimeError("native title fixture did not report PASS")
    width, height, rgb = _bmp_rgb(capture / "title_0.bmp")
    receipt["live_title"] = {"width": width, "height": height, "sha256": sha256_file(capture / "title_0.bmp"),
                             "magenta_pixels": _magenta(rgb)}
    receipt["visible_palette_edit"] = bool(
        receipt["title_export_differs_from_copied_baseline"]
        and receipt["title_export_magenta_pixels"] > receipt["baseline_title_magenta_pixels"]
        and _magenta(rgb) / (width * height) > receipt["baseline_title_magenta_pixels"] / (512 * 448))
    # Preserve the native capture; sips only changes its lossless file encoding.
    _logged_run(receipt, output, "capture-png", ["/usr/bin/sips", "-s", "format", "png", capture / "title_0.bmp", "--out", capture / "title.png"])
    receipt["screenshot"] = str(capture / "title.png")
    if not receipt["visible_palette_edit"]:
        raise RuntimeError("native title capture did not show the expected palette edit; inspect captured image and receipt")
    receipt["success"] = True


def capture_title(image: Path, out_dir: Path, *, port: Path | None = None, python: Path | None = None) -> dict:
    """Export the supplied ISO and capture the existing port's title fixture."""
    image, output = Path(image).resolve(), safe_output(out_dir)
    port = Path(port or ROOT.parent / "extermination-port").resolve()
    python = Path(python or (ROOT / ".venv/bin/python" if (ROOT / ".venv/bin/python").is_file() else sys.executable))
    if output.exists() and (not output.is_dir() or any(output.iterdir())):
        raise ValueError("port proof output must be new or empty")
    if image.is_relative_to(output) or port.is_relative_to(output):
        raise ValueError("port proof output contains an input")
    executable, source_assets = port / "build/extermination", port / "assets"
    if not executable.is_file() or not source_assets.is_dir():
        raise ValueError("existing native-port executable/assets are required; this helper never builds or modifies the port")
    for path in source_assets.rglob("*"):
        if path.is_symlink():
            raise ValueError("port proof requires ordinary support assets, not shared writable symlinks")
    scripts = [ROOT / "tools/extract_data.py", ROOT / "tools/export_startup.py"] + [
        port / "tools" / name for name in ("export_movie.py", "export_streams.py", "export_module_loader.py", "export_disc_textures.py")]
    font_elf = port.parent / "Extermination/config/SCUS_971.12"
    protected = {str(path): sha256_file(path) for path in [executable, *scripts, font_elf]}
    support = _snapshot(source_assets)
    baseline_title = (source_assets / "startup/title_0.emui").read_bytes()
    output.mkdir(parents=True, exist_ok=True)
    logs, assets, capture = output / "logs", output / "assets", output / "capture"
    logs.mkdir(); capture.mkdir()
    receipt = {"source_iso": str(image), "port": str(port), "scope": "native port startup/title only",
               "copied_support_assets": support, "commands": [], "protected_inputs_before": protected,
               "headless": True, "native_save_directory": str(output / "data/save"),
               "reexported_assets": [], "success": False}

    def save():
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

    def run(name, command, **kwargs):
        return _logged_run(receipt, output, name, command, **kwargs)

    try:
        info = inventory(image)
        receipt["source_iso_sha256"] = info["image_sha256"]
        inputs = output / "disc"
        with image.open("rb") as stream:
            for entry in info["files"]:
                if entry["path"] not in ("DATA/DATA.DAT", "DATA/INDEX.IDX", "SCUS_971.12"):
                    continue
                target = inputs / entry["path"]
                target.parent.mkdir(parents=True, exist_ok=True)
                stream.seek(entry["offset"])
                remaining = entry["size"]
                with target.open("wb") as destination:
                    while remaining:
                        block = stream.read(min(4 * 1024 * 1024, remaining))
                        if not block:
                            raise ValueError("truncated ISO input during port staging")
                        destination.write(block); remaining -= len(block)
                if sha256_file(target) != entry["sha256"]:
                    raise ValueError("staged port input differs from ISO inventory")
        boot = inputs / "SCUS_971.12"
        receipt["boot_sha256"] = sha256_file(boot)
        if protected[str(font_elf)] != receipt["boot_sha256"]:
            raise ValueError("port font export requires its pinned main-checkout ELF to match the supplied disc boot; no original-ELF substitution is allowed")
        shutil.copytree(source_assets, assets)
        if _snapshot(assets) != support:
            raise ValueError("copied port support assets differ from their source")
        extract = output / "extract"
        run("legacy-extract", [python, scripts[0], "extract", "--disc", inputs, "--out", extract])
        run("startup", [python, scripts[1], "--extract", extract, "--out", assets / "startup", "--png"])
        run("movie", [python, scripts[2], "--iso", image, "--out", assets / "startup/intro.mov"])
        run("streams", [python, scripts[3], "--iso", image, "--elf", boot, "--out", assets / "streams"])
        run("module-loader", [python, scripts[4], "--iso", image, "--elf", boot, "--out", assets / "module_loader/modules.emml"])
        run("font", [python, scripts[5], "--iso", image, "--extract", extract, "--assets", assets,
                     "--scratch", output / "disc-textures", "--only", "font"])
        receipt["font_elf_input"] = {"path": str(font_elf), "sha256": protected[str(font_elf)],
                                     "matches_iso_boot": protected[str(font_elf)] == receipt["boot_sha256"],
                                     "reason": "Existing font exporter has no --elf argument and reads its pinned main-repository ELF"}
        exported = [assets / "startup" / f"{name}_{index}.{extension}"
                    for name in ("logo", "title") for index in range(3) for extension in ("emui", "png")]
        exported += [assets / "startup/intro.mov", assets / "startup/manifest.json", assets / "streams/streams.emst",
                    assets / "streams/streams.json", assets / "module_loader/modules.emml",
                    assets / "module_loader/modules.json", assets / "font.emfn"]
        if any(not path.is_file() for path in exported):
            raise RuntimeError("an existing exporter reported success without creating all expected startup/metadata files")
        receipt["reexported_assets"] = {str(p.relative_to(assets)): sha256_file(p) for p in exported}
        title = (assets / "startup/title_0.emui").read_bytes()
        receipt["title_export_differs_from_copied_baseline"] = title != baseline_title
        receipt["title_export_magenta_pixels"] = _magenta(bytes(c for i, c in enumerate(title[36:]) if i % 4 != 3))
        receipt["baseline_title_magenta_pixels"] = _magenta(bytes(c for i, c in enumerate(baseline_title[36:]) if i % 4 != 3))
        _native_capture(receipt, output, executable)
    finally:
        receipt["protected_inputs_unchanged"] = all(sha256_file(Path(path)) == digest for path, digest in protected.items())
        receipt["shared_assets_unchanged"] = _snapshot(source_assets) == support
        save()
        if not receipt["protected_inputs_unchanged"] or not receipt["shared_assets_unchanged"]:
            raise RuntimeError("shared port inputs changed during proof; inspect receipt")
    return receipt


def resume_title(image: Path, out_dir: Path) -> dict:
    """Retry only native graphics after checking every staged/source hash."""
    output, image = safe_output(out_dir), Path(image).resolve()
    receipt = json.loads((output / "receipt.json").read_text())
    if receipt.get("success") or str(image) != receipt["source_iso"] or sha256_file(image) != receipt["source_iso_sha256"]:
        raise ValueError("port proof is complete or its ISO input changed")
    if not isinstance(receipt.get("reexported_assets"), dict):
        raise ValueError("port proof exports were not completed")
    port = Path(receipt["port"])
    protected, support = receipt["protected_inputs_before"], receipt["copied_support_assets"]
    expected_assets = {**support, **receipt["reexported_assets"]}
    if _snapshot(output / "assets") != expected_assets:
        raise ValueError("staged port assets changed after export")
    if (any(sha256_file(Path(path)) != digest for path, digest in protected.items())
            or _snapshot(port / "assets") != support):
        raise ValueError("shared port inputs changed since staging")
    attempt = "native-title-retry-" + str(sum(item["name"].startswith("native-title") for item in receipt["commands"]))
    try:
        _native_capture(receipt, output, port / "build/extermination", attempt=attempt)
    finally:
        receipt["protected_inputs_unchanged"] = all(sha256_file(Path(path)) == digest for path, digest in protected.items())
        receipt["shared_assets_unchanged"] = _snapshot(port / "assets") == support
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
        if not receipt["protected_inputs_unchanged"] or not receipt["shared_assets_unchanged"]:
            raise RuntimeError("shared port inputs changed during proof; inspect receipt")
    return receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--iso", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--port", type=Path)
    parser.add_argument("--resume-title", action="store_true", help="verify completed exports and retry only the hidden native title fixture")
    args = parser.parse_args()
    result = resume_title(args.iso, args.out) if args.resume_title else capture_title(args.iso, args.out, port=args.port)
    print(json.dumps({key: result[key] for key in ("success", "source_iso_sha256", "screenshot", "live_title")}, indent=2))


if __name__ == "__main__":
    main()
