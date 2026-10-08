"""Native Python CLI; generated material stays in this checkout's build/repack."""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from . import archive, iso

def output_path(value: Path) -> Path:
    return archive.safe_output(value)


def iso_summary(result: dict, destination: Path) -> dict:
    return {"image_path": str(destination), **{
        key: value for key, value in result.items() if key != "files"
    }}


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    p = commands.add_parser("inventory", help="list and classify every ISO file")
    p.add_argument("--iso", type=Path, required=True)
    p.add_argument("--out", type=Path, help="optional JSON receipt under build/repack")
    p = commands.add_parser("unpack", help="unpack DATA.DAT and INDEX.IDX")
    p.add_argument("--data", type=Path, required=True)
    p.add_argument("--index", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p = commands.add_parser("pack", help="rebuild DATA.DAT and INDEX.IDX")
    p.add_argument("--tree", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p = commands.add_parser("unpack-iso", help="unpack ISO files and exact layout")
    p.add_argument("--iso", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p = commands.add_parser("pack-iso", help="rebuild a lossless ISO tree")
    p.add_argument("--tree", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--data", type=Path)
    p.add_argument("--index", type=Path)
    p = commands.add_parser("unpack-disc", help="unpack ISO and editable archive together")
    p.add_argument("--iso", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p = commands.add_parser("pack-disc", help="build archive, then ISO, from an unpacked disc")
    p.add_argument("--tree", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True, help="output directory")
    p = commands.add_parser("build-disc", help="compile boot and overlays, rebuild assets, and pack a disc")
    p.add_argument("--tree", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True, help="output directory including isolated build workspace")
    p.add_argument("--toolchain-root", type=Path, help="checkout containing the local compiler tools")
    p.add_argument("--require-original", action="store_true", help="fail unless the whole image matches the unpacked reference")
    p.add_argument("--resume", action="store_true", help="resume a failed overlay link after validating its existing fresh build")
    args = parser.parse_args(argv)
    try:
        if args.command == "inventory":
            result = iso.inventory(args.iso)
            report = json.dumps(result, indent=2) + "\n"
            if args.out:
                dest = output_path(args.out)
                if dest == args.iso.resolve() or (dest.exists() and dest.samefile(args.iso)):
                    raise ValueError("inventory output aliases the source ISO")
                dest.parent.mkdir(parents=True, exist_ok=True)
                dest.write_text(report)
                print(f"Inventory written to {dest}")
            else:
                print(report, end="")
        elif args.command == "unpack":
            archive.unpack_archive(args.data, args.index, output_path(args.out))
            print(f"Archive unpacked to {args.out}")
        elif args.command == "pack":
            out = output_path(args.out)
            out.mkdir(parents=True, exist_ok=True)
            result = archive.pack_archive(args.tree, out / "DATA.DAT", out / "INDEX.IDX")
            print(json.dumps(result, indent=2))
        elif args.command == "unpack-iso":
            iso.unpack(args.iso, output_path(args.out))
            print(f"ISO unpacked to {args.out}")
        elif args.command == "pack-iso":
            overrides = {}
            if args.data:
                overrides["DATA/DATA.DAT"] = args.data
            if args.index:
                overrides["DATA/INDEX.IDX"] = args.index
            result = iso.pack(args.tree, output_path(args.out), overrides=overrides)
            print(json.dumps(iso_summary(result, args.out), indent=2))
        elif args.command == "unpack-disc":
            out = output_path(args.out)
            # Both unpackers reject existing populated destinations, so an edited
            # tree cannot silently be replaced by another extraction.
            if out.exists() and any(out.iterdir()):
                raise ValueError("unpack-disc destination must be empty")
            iso.unpack(args.iso, out / "iso")
            files = out / "iso" / "files" / "DATA"
            archive.unpack_archive(files / "DATA.DAT", files / "INDEX.IDX", out / "archive")
            print(f"Disc unpacked to {out}; edit {out / 'archive'}")
        elif args.command == "pack-disc":
            out = output_path(args.out)
            tree = args.tree.resolve()
            if out == tree or out.is_relative_to(tree) or tree.is_relative_to(out):
                raise ValueError("pack-disc output directory must be separate from its input tree")
            out.mkdir(parents=True, exist_ok=True)
            packed = archive.pack_archive(tree / "archive", out / "DATA.DAT", out / "INDEX.IDX")
            disc = iso.pack(tree / "iso", out / "Extermination.iso", overrides={
                "DATA/DATA.DAT": out / "DATA.DAT", "DATA/INDEX.IDX": out / "INDEX.IDX",
            })
            print(json.dumps({"archive": packed, "iso": iso_summary(disc, out / "Extermination.iso")}, indent=2))
        elif args.command == "build-disc":
            from . import source_build
            out, tree = output_path(args.out), args.tree.resolve()
            if out == tree or out.is_relative_to(tree) or tree.is_relative_to(out):
                raise ValueError("build-disc output must be separate from its input tree")
            options = {"progress": lambda message: print(message, file=sys.stderr, flush=True)}
            if args.toolchain_root:
                options["toolchain_root"] = args.toolchain_root
            if args.resume:
                options["resume"] = True
            built = source_build.build_sources(tree, out / "source", **options)
            out.mkdir(parents=True, exist_ok=True)
            packed = archive.pack_archive(tree / "archive", out / "DATA.DAT", out / "INDEX.IDX")
            overrides = {key: Path(path) for key, path in built["overrides"].items()}
            overrides.update({"DATA/DATA.DAT": out / "DATA.DAT", "DATA/INDEX.IDX": out / "INDEX.IDX"})
            result = iso.pack(tree / "iso", out / "Extermination.iso", overrides=overrides)
            receipt = {"source": built, "archive": packed, "iso": iso_summary(result, out / "Extermination.iso")}
            (out / "build-disc.json").write_text(json.dumps(receipt, indent=2) + "\n")
            print(json.dumps(receipt, indent=2))
            if args.require_original and not result["unchanged"]:
                raise ValueError(f"source-built disc differs from the reference; see {out / 'build-disc.json'}")
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, f"repack: {exc}\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
