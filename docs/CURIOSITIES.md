# Extermination — Curiosities & Hidden Systems

Interesting, obscure, or possibly-cut features discovered during the
decompilation. Each entry cites the FINDINGS section with the evidence and a
status: **decoded** / **partial** / **unknown**. This is the fun document —
the things a remake or a lore writeup would want to know.

## 1. The light-based stealth system (partial — flagship curiosity)

The game ships a complete enemy-awareness stealth loop that casual play never
surfaces (FINDINGS s28b, s51):

- **Two player lights, independent flags:** the L3 *shoulder light*
  (player+0xA — a 300-frame/5s burst with an auto-off gesture anim, bound to
  an UNLABELED row of the button-config screen) and the Square-in-aim *gun
  light* (`D_00810D3C`, a persistent preference).
- **Neither flag renders light.** No render code reads them; the visible
  "flashlight look" comes from the per-room light-rig table (`D_00251C50`,
  45 entries) and an always-on camera fill light. The flags are gameplay.
- **Detection:** `func_001418F0` drives an enemy awareness meter (0–240 at
  enemy+0x70). Light ON → instant max awareness. Light OFF → gradual ramp,
  gated on a 12.5° view-cone test, ramp rate indexed by difficulty
  (`0x81050C & 3`). Darkness trades visibility for safety.
- **Dark-area anim rows:** `func_001B0070()&4` swaps the player onto
  alternate animation variants in dark areas.
- **Possibly dormant:** the awareness consumers (the uncharacterized
  `0x13Dxxx` enemy family) never executed in any live session — common
  crawlers sense by distance + group alarm only. Specific late-game enemies,
  or cut content?
- Open: awareness decay, the consuming enemy family, room-light gating, why
  the off-gesture shares clip id 0x15D with the idle look-around fidget.

## 2. The unreachable sprint (decoded)

The locomotion tables define FOUR gaits — turn/walk/run plus a sprint slot
(anim id 3, 0.8 u/tick = 48 u/s, footstep frames 21/2) — but the gait
quantizer clamps at 3, so the sprint is unreachable data. (s31, s46)

## 3. Hidden animation directory entries (unknown)

The player clip directory has 459 entries; four (ids 54/94/115/375) carry
non-sentinel headers in a format the parsers don't decode. Unknown payloads,
the source of the long-lived "clip shift" exporter bug. (s48 door session)

## 4. Passcode keypads with codes in memory (partial)

Status-menu pages 4/5 are 3x4 passcode keypads (external-trigger only,
`D_008106C5`), comparing input via strcmp against strings at
`D_00275858[n]` and unlocking via `0x810845 |= 0x20`. The code strings and
their doors are undumped. (s39)

## 5. Dead code shipped on disc (decoded)

- AREA13.BIN contains a *drifted, broken* region helper (mid-function entry
  at 0x823FE0, `lq ra` against the caller's frame) — reachable only by an
  area-13 gate, would likely crash; the boot ELF's `vcallmsr` instruction
  appears exactly once and is statically unreachable. (s50, early VU work)
- The entire VIF1 helper library at 0x0011BA00..0x0011BCF8 is statically
  unreachable dead code. (s4-era)

## 6. The battery is a currency, not a timer (decoded)

The "battery" never drains from lights — devices SPEND half-units
(2x device cost) and stations recharge +4/cycle; pickups add +12/+36.
The flashlight costs nothing. (s28b)

## 7. Mag-equivalent 30:1 fold (decoded)

Ammo bookkeeping keeps a hidden "magazine-equivalent" counter (cap 98) that
folds into reserve at 30:1 — and reserve is the TOTAL pool including the
loaded mag; reloading never subtracts. (s18, s47)

## 8. Per-area crate disguises (decoded)

The infected-crate enemy's disguise model is bound per-area via the area
model table: a beveled wooden crate in some areas, a taped cardboard box in
others — same enemy, regional camouflage. (s23, s34, s52-era)

## 9. The 15 named RECON dogtags (partial)

The item catalog (message bank group 3) names 15 collectible dogtags of the
RECON squad members — a full collectible set with named characters. Pickup
placement/reward logic untraced. (s42)

## 10. Infection diary (decoded)

The status hub's idle help panel cycles a 5-grade first-person infection
diary ("I feel dizzy… I can't stop shivering…"), graded by the live
infection percentage, ending at "Dennis Infected" at 100%. (s42, s49)

## 11. Elevated-context knife rows (unknown)

The melee tables carry a parallel set of knife anims (0x1BD..0x1C1) behind
an untranslated gate (player+0x236) — alternate attacks in some context
(ladders? water? low ceilings?). (s36)

## 12. The unlabeled 7th config row (partial)

The BUTTON CONFIG screen has an unlabeled row that binds the shoulder light
(L3 in TYPE A). The game hides a configurable control it never explains.
(s28b)

## 13. Cutscene multi-actor track sets (partial)

Character mesh files carry synchronized 401-frame multi-actor animation
track sets (e.g. f17_id8f's 0x74+0x2c+3x0x24 prefix) — whole cutscenes
stored as coordinated clip bundles, separate from gameplay anim banks. (s4b)

## 14. MUSIC cue 1 = the End Credits (decoded)

The first real entry of the music cue table is the credits roll; cue 24 is a
hardcoded alert-mode override that pre-empts area BGM. (s27 music session)
