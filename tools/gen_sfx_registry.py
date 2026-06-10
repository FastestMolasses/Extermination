#!/usr/bin/env python3
"""gen_sfx_registry.py — emit the native port's SFX registry (sfx.txt).

Resolves engine SOUND IDS to the locally-decoded WAV files through
extract/audio_decoded/soundmap.json (the `audio_export.py soundmap`
output) and writes the port's `assets/sfx/sfx.txt` registry: one
`<id-hex> <abs-wav-path>` line per sound (em_sfx.c parses exactly that;
'#' lines are comments).

Soundmap schema (commit 99cf57d):
  - `sounds` keys are UPPERCASE zero-padded hex: '0x162', '0x3FB',
    '0x7D8' (a naive lowercase/`%x` lookup finds nothing).
  - GLOBAL ids carry the trigger-script decode inline:
        {"record": [g,b,sg,si], "bank": ..., "events": [...]}
  - AREA-TABLED / region-dependent ids instead carry "variants", each
    with the regions ("chunk04.n0") and the area list ("2.1") it is
    loaded for. Resolution order here: a variant listing the requested
    area wins; otherwise fall back to the area's region from
    `area_scene_map` (its best-coverage container bank).
  - Every event lists the WAV (relative to the soundmap's directory)
    and the ENGINE-EXACT playback rate. The registry keeps the FIRST
    event's WAV — the port's em_sfx is single-sample per id until the
    multi-event (layer/delay) trigger scripts land.

DOOR PAIR IDS (FINDINGS.md "DOOR SCRIPTS DECODED" s23): a door's open
sound is NOT a constant — `func_001BBD60` patches the op-0x0B sub-6
script record with `D_0024DB80[link >> 8][side]`, the halfword
[front_id, back_id] pair table indexed by the placement LINK halfword's
high byte (ids 0x3FB..0x40E). `--scene office` recomputes this from the
user's own local data: the AREA02 state-1 placement table (door records,
tools/placements.py) for the link bytes + the boot ELF for the pair
table. It also prints the matching `doorsfx <front> <back>` scene.txt
line the port's em_door consumes.

Usage (macOS arm64, plain python3 — no deps):
  tools/gen_sfx_registry.py --scene office \
      -o ../extermination-port/assets/sfx/sfx.txt
  tools/gen_sfx_registry.py 0x162 0x7D8 --area 2.1   # ad-hoc id list

Everything read here is the user's own locally-extracted material; the
output registry is likewise local-only (the port gitignores assets/).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOUNDMAP = ROOT / "extract" / "audio_decoded" / "soundmap.json"
BOOT_ELF = ROOT / "elf" / "SCUS_971.12.elf"

# D_0024DB80 — door sound [front_id, back_id] halfword pair table
# (FINDINGS s23), 13 used entries (selectors 0..12).
DOOR_PAIR_VADDR = 0x24DB80
DOOR_PAIR_COUNT = 16

# Office scene = AREA02 sub-state 1 (placement table 0x828170), engine
# area key 2.1 in the soundmap's area tables.
SCENES = {
    "office": {
        "area": "2.1",
        "overlay": "AREA02.BIN",
        "table_vaddr": 0x828170,
        # Gameplay ids already wired in the port (em_sfx.h):
        # - weapon draw 0x162 / handling-foley 0x163 (holster + reload
        #   start) / fire 0x164/0x165 / reload MAG ACTION 0x168 / dry
        #   0x169 — the reload pair 0x163+0x168 was live-pinned s29
        #   (FINDINGS "GAMEPLAY SOUND IDS PINNED LIVE"), retiring the
        #   old 0xF002 -> 0x166 placeholder alias.
        # - FOOTSTEPS (s37 "FOOTSTEP SURFACE TABLE" — replaces the s29
        #   fixed-pair reading): per step, surface_id = BLOCK(attr) +
        #   GAIT_SUB(gait) + rand5() and gear_id = 0x138 + rand5()
        #   (independent draw; rand5 = (rand() & 7), 5..7 folded to
        #   0..2). The office floor is attr 0 -> block 0x10, so the
        #   FULL variant sets ship: walk (gait 2, +5) 0x15..0x19, run
        #   (gait 3, +0xA) 0x1A..0x1E, gear/cloth 0x138..0x13C.
        # - canonical enemy death 0x7D8.
        # - KNIFE/MELEE (s36 decode, FINDINGS "KNIFE/MELEE DECODED"):
        #   swing/impact sounds 0x17D/0x17E/0x17F (light hits 1-3; the
        #   heavy stab shares 0x17F), fired at each attack's impact
        #   gate, vol 300; 0x179 = the armed-stance SQUARE attachment-0
        #   toggle-ON sound (func_0017A970 — the s29 "unidentified
        #   action", snd_0436).
        # - FIRE-CHAIN TAIL (s29 live, FINDINGS "GAMEPLAY SOUND IDS
        #   PINNED LIVE"): wall impact/ricochet 0x189 ~2 frames after
        #   the shot (snd_0421) + shell casing 0x16A ~0.7 s after each
        #   shot (2-event, snd_0347). SURFACE-VARIANT FLAG: the
        #   soundmap's 0x188/0x18A/0x18B neighbors (snd_0423/0422/0420,
        #   consecutive tones of the same program) look like the
        #   per-surface impact family, but no surface->id mapping is
        #   pinned in FINDINGS — only the observed 0x189 ships; the
        #   port plays it for every wall (em_sfx.h flag).
        "ids": [0x162, 0x163, 0x164, 0x165, 0x168, 0x169, 0x7D8,
                0x015, 0x016, 0x017, 0x018, 0x019,   # walk (gait 2)
                0x01A, 0x01B, 0x01C, 0x01D, 0x01E,   # run (gait 3)
                0x138, 0x139, 0x13A, 0x13B, 0x13C,   # gear/cloth
                0x17D, 0x17E, 0x17F, 0x179,
                0x189, 0x16A],
    },
}


def load_soundmap() -> dict:
    with open(SOUNDMAP) as f:
        return json.load(f)


def sound_key(sid: int) -> str:
    """soundmap `sounds` key format: uppercase, >= 3 hex digits."""
    return f"0x{sid:03X}"


def resolve(sm: dict, sid: int, area: str):
    """id -> (wav_rel, rate_hz, label, n_events, how) or None."""
    entry = sm["sounds"].get(sound_key(sid))
    if entry is None:
        return None
    label = entry.get("label", "")
    if "events" in entry:                       # global (fixed) id
        ev = entry["events"]
        return ev[0]["wav"], ev[0]["rate"], label, len(ev), "global"
    # area-tabled / region-dependent: prefer an exact area listing
    for var in entry.get("variants", []):
        if area in var.get("areas", []):
            ev = var["events"]
            return (ev[0]["wav"], ev[0]["rate"], label, len(ev),
                    f"area {area} ({var['regions'][0]})")
    # fall back to the area's best-coverage region bank
    region = sm["area_scene_map"].get(area, {}).get("region")
    if region:
        for var in entry.get("variants", []):
            if region in var.get("regions", []):
                ev = var["events"]
                return (ev[0]["wav"], ev[0]["rate"], label, len(ev),
                        f"region {region} (area-map fallback)")
    return None


def read_door_pairs() -> list[tuple[int, int]]:
    """D_0024DB80 [front, back] pairs from the user's local boot ELF."""
    data = BOOT_ELF.read_bytes()
    e_phoff, = struct.unpack_from("<I", data, 0x1C)
    e_phnum, = struct.unpack_from("<H", data, 0x2C)
    for i in range(e_phnum):
        (p_type, p_offset, p_vaddr, _pa, p_filesz, _ms, _fl,
         _al) = struct.unpack_from("<8I", data, e_phoff + i * 32)
        if p_type == 1 and p_vaddr <= DOOR_PAIR_VADDR < p_vaddr + p_filesz:
            off = DOOR_PAIR_VADDR - p_vaddr + p_offset
            hw = struct.unpack_from(f"<{DOOR_PAIR_COUNT * 2}H", data, off)
            return [(hw[2 * k], hw[2 * k + 1])
                    for k in range(DOOR_PAIR_COUNT)]
    raise RuntimeError("pair-table vaddr not covered by any LOAD segment")


def office_door_pairs(scene: dict):
    """Door placements' link bytes -> their D_0024DB80 pairs.

    Returns (pair_ids_in_order, per_door_info, doorsfx_pair)."""
    sys.path.insert(0, str(ROOT / "tools"))
    import placements  # noqa: E402  (repo tool, same conventions)

    overlay = ROOT / "extract" / "OVERLAY" / scene["overlay"]
    table = placements.parse_table(overlay.read_bytes(),
                                   scene["table_vaddr"])
    pairs = read_door_pairs()
    door_ids: list[int] = []
    info: list[str] = []
    selectors: list[int] = []
    for e in table:
        # class-5 door actors (behavior func_001BC350) with a live link
        if (e.spawn_class & 0xFF) != 0x85 or e.link == 0xFFFF:
            continue
        sel = (e.link >> 8) & 0xFF
        front, back = pairs[sel]
        selectors.append(sel)
        info.append(f"door uid {e.uid_byte:#04x} link {e.link:#06x} -> "
                    f"selector {sel} -> pair [front 0x{front:03X}, "
                    f"back 0x{back:03X}]")
        for sid in (front, back):
            if sid and sid not in door_ids:
                door_ids.append(sid)
    if not door_ids:
        raise RuntimeError("no door placements with links found")
    if len(set(selectors)) > 1:
        info.append("WARNING: doors use DIFFERENT sound-set selectors; "
                    "the port's single global doorsfx line carries only "
                    "the first pair")
    doorsfx = pairs[selectors[0]]
    return door_ids, info, doorsfx


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("ids", nargs="*",
                    help="sound ids (hex like 0x162, or decimal)")
    ap.add_argument("--scene", choices=sorted(SCENES),
                    help="scene preset: its id set, area, and the "
                         "door-pair computation")
    ap.add_argument("--area", default=None,
                    help="area key A.S for area-tabled ids "
                         "(default: the scene preset's, e.g. 2.1)")
    ap.add_argument("-o", "--out", default=None,
                    help="write the registry here (default: stdout)")
    args = ap.parse_args(argv)

    ids = [int(t, 16) if t.lower().startswith("0x") else int(t)
           for t in args.ids]
    door_info: list[str] = []
    doorsfx = None
    area = args.area

    if args.scene:
        scene = SCENES[args.scene]
        area = area or scene["area"]
        ids = scene["ids"] + [i for i in ids if i not in scene["ids"]]
        door_ids, door_info, doorsfx = office_door_pairs(scene)
        ids += [i for i in door_ids if i not in ids]
    if not ids:
        ap.error("no ids (pass ids and/or --scene)")
    if not area:
        ap.error("--area required for area-tabled ids (or use --scene)")

    sm = load_soundmap()
    wav_root = SOUNDMAP.parent

    lines = [f"# sfx.txt — generated by tools/gen_sfx_registry.py "
             f"(scene {args.scene or '-'}, area {area})",
             "# <id-hex> <wav-path>; WAVs are this user's own local "
             "decodes (extract/audio_decoded/)."]
    for note in door_info:
        lines.append(f"# {note}")
    if doorsfx:
        lines.append(f"# scene.txt line for the port's em_door: "
                     f"doorsfx 0x{doorsfx[0]:03X} 0x{doorsfx[1]:03X}")

    missing = 0
    for sid in ids:
        r = resolve(sm, sid, area)
        if r is None:
            lines.append(f"# 0x{sid:03X} UNRESOLVED for area {area}")
            print(f"UNRESOLVED: 0x{sid:03X} (area {area})",
                  file=sys.stderr)
            missing += 1
            continue
        wav_rel, rate, label, n_ev, how = r
        wav = (wav_root / wav_rel).resolve()
        if not wav.is_file():
            lines.append(f"# 0x{sid:03X} WAV MISSING: {wav}")
            print(f"WAV MISSING: 0x{sid:03X} -> {wav}", file=sys.stderr)
            missing += 1
            continue
        lines.append(f"# 0x{sid:03X} {label or '(unlabeled)'} — "
                     f"{rate} Hz, {n_ev} event(s), {how}")
        lines.append(f"0x{sid:03X} {wav}")

    text = "\n".join(lines) + "\n"
    if args.out:
        Path(args.out).parent.mkdir(parents=True, exist_ok=True)
        Path(args.out).write_text(text)
        print(f"wrote {args.out} ({len(ids)} id(s), {missing} "
              f"unresolved)", file=sys.stderr)
    else:
        sys.stdout.write(text)
    return 1 if missing else 0


if __name__ == "__main__":
    raise SystemExit(main())
