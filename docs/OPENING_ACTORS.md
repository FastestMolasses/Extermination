# AREA11 opening actor assets

How the original resolves, animates and draws the actors of the AREA11
opening (the player, Roger and Roger's equipment node) and their faces, and
the two decomp exporters that rebuild those meshes and tracks. Written in
September 2026; reviewed on 2026-10-09, when the exporters were rerun and
their reported values below re-checked. This file cites addresses, resource
indices and measurements. It holds no original code, no disassembly and no
disc bytes; every exported file stays in ignored directories.

**Status of the port side (2026-10-09).** The native port no longer draws
the opening from these exports. Port chain C8b OPENING (2026-09-28, port
commit 8bc4042) put the opening's actors on their original records: script
0x828FC0's op14 spawns the two 001BB0E0 records and the player plays bank
0x98's clip 1 on its own stage (port `docs/OPENING_ORIGINAL.md`). It retired
the baked-track module `em_opening_actor`, its tests (`test-opening-actor`,
`test-opening-runtime`) and the face-asset part of
`test_opening_face_reference.py`. The faces now attach through the original
object-unit path 001CB3C0 (port `docs/FACE_ATTACH.md`) and morph through the
translated VU1 face program (port `docs/VU1_FACE_MORPH.md`). The exporters
stay in this repository because port tools import their decoders
(`OpeningClip`, `exact_mesh_sections`, `matrix_multiply`, `original_palette`)
and the port's `tools/test_disc_textures_reference.py` reruns both on a
rebuilt GS image and requires byte-identical outputs (port
`docs/DISC_TEXTURES.md`).

## Actor exporter

The opening uses its own player and Roger animations, with world coordinates
inside the bone hierarchy. An ordinary locomotion asset placed at a guessed
spawn point cannot reproduce these tracks.

`tools/export_opening_actors.py` exports the three original meshes and their
opening tracks. Run from the decomp repository after extracting the owner's
disc and capturing the original opening GS state. The port's
`tools/test_disc_textures_reference.py` compares its own rerun with the files
in the port's `assets/scene_snow/opening/`, so this is still the command that
fills that directory:

```sh
python3 tools/export_opening_actors.py \
  --gs build/startup-reference/opening_gs.bin \
  --reference-ee build/startup-reference/opening_ee.bin \
  --out ../extermination-port/assets/scene_snow/opening \
  --report build/area11_original/opening_export.json
```

The optional EE input is specifically the captured opening at source animation
frame 135 (clip clock 511). It validates whole raw meshes, every bone's local
animation cursor fields, static adjustment transforms, and resulting matrices.

| Actor | Original resolver | Extracted model | Opening animation |
|---|---|---|---|
| Player | actor `008102B0`, model `00D1C1C0` | `chunk28/f00_id3b.bin` at `0` | bank `98`, clip `1` |
| Roger | resource `D_0028A490[47]`, model `01877740` | `chunk15/f18_id94.bin` at `35000` | bank `98`, clip `2` |
| Equipment | global model table `D_0028A56C`, index `6B`, model `00D00240` | `chunk27/f01_id37.bin` at `156080` | copies Roger bone `1` |

All offsets and addresses in the table are hexadecimal. Opening placement
records at `00828F30` and `00828F5C` select actor classes 9 and 8 respectively.
`001BAC00` creates these records, `001BB0E0` updates them, and `001BAD40`
resolves their resources. The second record uses mode 5 (`001C5C90`), which
copies the parent model `47` bone1 matrix without an extra attachment offset.
The ordinary Roger actor elsewhere in the live list is a different actor and
must not be substituted for the opening instance.

The player also has seven drawable class1 children (`0018A6B0`), separate from
the class8 equipment controller. The controller has no bones and is not drawn;
that does **not** mean the equipment is absent. Original opening RAM confirms
global library indices 47/48/49/50/56/64 form the rifle, all with matrices byte
equal to player bone4. Index 106 is the holstered knife, with matrix byte equal
to bone14. The exporter verifies every whole child model and its live matrix,
then merges its unchanged local vertices onto the corresponding player bone.

Bank `98` begins at `chunk15/f12_id44.bin + 0D0800`, exactly matching runtime
pointer `014C4740`. Its three-entry directory is `10, 5100, 10740`: camera,
player, Roger. Each actor clip contains 21 nodes and lasts 646 source frames
(1,291 poses at the half-frame step).
Both have header continuation `-2`, so `anim_advance_time` holds at frame 645.

## Original animation operations

The exporter follows these original operations:

- `001C8710` initializes each channel cursor; `001C87C0` advances them at 0.5
  per engine tick. Scale-key bit15 clears all rotation increments and vector
  velocities. Analytic interpolation misses the resulting cutscene holds.
- `001CA0A0`, historically named `quat_nlerp`, performs a hemisphere-corrected
  linear blend **without normalization**. `001CA1C0` (`quat_to_mat3`) directly
  builds its matrix.
- `001C6DA0` (`anim_eval_skeleton`) applies local scale and composes parent
  matrices. The opening has identity actor/static adjustment matrices; node0
  stays identity while other nodes carry world motion. No root translation is
  removed.
- `001BB0E0` falls through from initialization into the first animation
  advance, so the first drawn pose is source 0.5. This matches the original
  capture, where actor source 135 accompanies camera source 134.5.

Exporter validation at the original source-135 capture (rerun 2026-10-09):
all key indices, quaternion key pairs, local translations/scales and their
velocities, reciprocals, and channel countdowns match exactly. Player
rotation-blend accumulation differs by at most `1.79e-7`; Roger's matches
exactly. Maximum matrix component error is `4.58e-5` for the player,
`9.16e-5` for Roger, and `5.96e-8` for the attachment. These are host
float32 versus EE/VU rounding differences of the exporter's own arithmetic,
not a claim of bit-identical matrices. Model bytes are independently compared
in full. Geometry welding uses exact attribute bytes, and textures use the
captured opening's PSMT4 texels and CLUTs without a fallback palette.

## Original face meshes and state

The face is a separate original draw, not part of the skeletal animation bank.
`001CAA00` first draws the body through `001C7420`. The latter compares each
bone with `actor+94`; that bone gets a zero basis with only its translation
retained. Consequently its body triangles collapse to a point. `001CB3C0`
then uploads the full head matrix, uploads the face weights through `001CB2C0`,
and draws the face resource through `001D3E40` and VU kernel `0023C4B0`. The
face path computes its own lighting through `001D88B0` at the head position.

| Face | Resource index (hex) | Original source | Runtime address | Bone |
|---|---|---|---|---|
| Dennis | `18` | `chunk03/f16_id18.bin + 0` | `011749C0` | 7 |
| Roger | `88` | `chunk15/f18_id94.bin + 86000` | `018C8740` | 7 |

Whole-model comparisons and the live `D_0028A490` table independently prove
these mappings. Each face packet contains two VIF UNPACKs, 256 and 96 qwords,
which together contain 32 vertices of 176 bytes. Each vertex contains TEX0,
UV, normal, base position and seven position deltas. The VU accumulator sums
the seven weighted deltas, then adds the base position; normals remain
unchanged. The old 64-byte geometry decoder cannot parse this layout. The
port's `docs/VU1_FACE_MORPH.md` has the full program, its upload recipe and
its reference test.

```sh
python3 tools/export_opening_faces.py \
  --gs build/startup-reference/opening_gs.bin \
  --reference-ee build/startup-reference/opening_ee.bin \
  --out ../extermination-port/assets/scene_snow/opening \
  --report build/opening_face/export.json
```

The exporter writes `_face.emdl` and `_face.emfm` files, combining each face
with its body's unchanged vertices, textures and original bone7 palette. It
removes only wholly bone7 triangles: 1,124 for Dennis and 1,048 for Roger.
Both assets have zero mixed head/body triangles. The face exports contain
850/701 exact-weld vertices and 12/13 textures respectively (rerun
2026-10-09).

**Face state (001D0720).** The first two weights have separate timed state
machines; the remaining five approach randomized mouth targets. The
original's unusual choice 0..4 versus target 1..5 indices are part of the
behaviour. `001D06E0` clears targets when talking ends, allowing current
weights to decay. `001FD950` talks only on positive-timer services; its
completion draw still shows the subtitle but releases the speaker.
`00183090` services Dennis's face while the facial-state guard is 2;
`001BA580` consumes the Roger speaker mailbox and services his face.
`001B81D0` and `001BA8E0` select speed 1. Original `001AF780` reuses pool
slots; `001D0690` resets control state and targets but preserves current
weights, wait fields and speed, so a face's initial weights depend on the
earlier pool contents. An arbitrary mid-scene snapshot is not a universal
initial state.

The port translates 001D0720 as `em_opening_face_tick` in
`src/game/em_opening_face.c`. Its oracle, run from the port repository:

```sh
python3 tools/test_opening_face_reference.py \
  --reference-ee ../Extermination/build/startup-reference/opening_ee.bin
```

executes the original instructions of 001D0720 and passes 11,552 full-state
byte comparisons and 6,559 identical RNG call counts (rerun 2026-10-09),
covering state/timer boundaries and both captured actors. Its float
instructions follow the port's measured EE model (port
`docs/EE_FLOAT_MODEL.md`). The face's global RNG call order is audited in the
port's `docs/RAND_ORDER.md`; exact pooled initial weights remain open work
(port `docs/STARTUP.md`).

## Child lifetime

In `001BB0E0` phase 1, a bit in the owning controller's `+2E` mask changes the
child to phase 2 and returns before its animation or draw. The next callback
changes phase 2 to 3 and performs applicable cleanup; the following callback
frees the actor. Setting the controller mask to `FFFF` therefore stops child
draws on the first observing callback, although storage survives two further
callbacks. Same-frame ordering relative to the controller remains relevant.
The opening Roger and equipment are transient children; this does not remove
the controller's separate visible prop.
