# Startup checkpoint — SCUS-97112

Cold boot through the three startup screens, the intro movie and the
interactive title menu, as the original code does it. Written 2026-09-22 from
the user's local boot ELF, the generated splat functions, the readable C and
object metadata; reviewed against the current source and docs on 2026-10-09
(the provenance table below was redone then). This file cites addresses,
constants and what the code does. It holds no original code, no disassembly
and no disc bytes, and it describes on-screen text instead of quoting it.

The native port's startup (what it runs, its tests, its remaining work) is
the port's `docs/STARTUP.md`; this file is the decomp-side evidence it cites.

## Corrections that affect the port (2026-09-22)

**The intro is `MOVIE/E900.PSS`, and it is requested both before the first title
menu and after selecting NEW GAME.** Earlier FINDINGS/PROGRESS claims that the
new-game path has no FMV are wrong (FINDINGS.md "Startup continuation
corrections" records the correction). Those traces missed the shared
movie-request flag. Setting the area to 11 later in ordinary game code does not
imply that no movie runs before that write.

* `func_001AC3B0` stores 1 to `D_00821058` at 0x001AC434 and clears movie
  selector `D_00275C78` at 0x001AC438. This is cold-boot/title state 1.
* `func_001AD360` clears the same selector at 0x001AD3EC and stores the same
  request at 0x001AD3F4. This is the NEW GAME startup path.
* Main loop 0x001AAE40 (misleading current name `gs_readback_queue_run`) reads
  the request at 0x001AAFC4 and calls `func_00203350` at 0x001AAFD4 when it is 1.
  That call is a blocking MPEG playback driver, not merely audio service.
* `func_002032C0` fills nine `{LSN, byte_size}` pairs at 0x00821010 by looking
  up filename pointers at 0x00264FB0. Entry 0 points to 0x00273330, filename
  `\MOVIE\E900.PSS;1`. `func_002034C0` selects that pair using
  `D_00275C78`; `func_00203460` passes its size to the movie pump.

Also, the main loop's **second** `func_001AEE70` fade update and
`func_001D1C10` are inside the movie-request arm. They do not run twice on every
ordinary frame; the old frame-anatomy table obscures this condition.

## Cold-boot sequence

Task registration: `func_001AB740(0, func_001AB7E0)`. The 0x20-byte task record
is selected through scratchpad pointer 0x70003B6C. Its bytes +8/+9 are the
major/substate, +0xA is another substate, +0xF is the menu cursor, and +0x16 is
a 16-bit timer. Task replacement `func_001AB790` clears bytes +8 through +0x17,
so each installed task begins at state zero.

| Boot major | Handler | Required behavior |
|---|---|---|
| 0 | `func_001AB7E0` | Initialize bank index mapping; poll `func_001FB370(D_0028A4A4)`; publish resource end pointer and initialize card UI state. |
| 1 → 2 | `func_001AB9D0` | Set black; asynchronously load screen module **0x28 / chunk40** through `func_001FF080(0,0x28)`; wait for busy byte `D_00275BD8` to clear; fade in; run `func_0022A460(0)` card-check flow; fade out at speed 8; at black initialize timer 300, fade in at speed 4, and present startup screen A. |
| 3 | `func_001ABC60` | Load screen module **0x29 / chunk41**; wait for the loader; fade in at speed 4; present startup screen B. At completion, synchronously load resource bank 0x1B. |
| 4 | `func_001ABE10` | Present startup screen C using already-resident art (no additional module request); fade in at speed 4. At completion, synchronously load bank 0x1C. |
| 5 | `func_001AB7E0` | Restore the two audio-channel levels with `func_00119978(ch,0x3FFF,0x3FFF)`. |
| 6 | `func_001AB7E0` | Set from-death flag `D_00275BDC=0`; install title task `func_001AC070`. |

Original art decoded by the startup exporter identifies **A: the violence
warning; B: the Sony Computer Entertainment America presentation card; C: the
Deep Space logo**. A is module40's four-tile bank, B/C use different four-tile
banks resident after module41 loads. The first fixed screen is therefore a
warning, not a logo.

`tools/export_startup.py` reconstructs the original upload and sprites.
Palette data is present on disc inside those uploads: for example title
CBP 0x2F80 lies within module1's PSMCT32 destination span 0x2A0000..0x300000.
Correct CSM1 access swaps index bits 3/4 **then** reads the 16x16 palette
through PSMCT32 addressing. The previous claim that title colors require
synthesized/captured VRAM was false; a linear-byte CLUT reader caused the
failed searches. Old captures may have been overwritten with gameplay; verify
their contents instead of trusting state-slot names.

**Logo timing and skip:** each screen sets timer +0x16 to **0x12C (300)**.
The drawing state performs `old = timer; timer = old - 1; if (old == 0)` to
start fade-out. That is **301 drawing-state ticks**, not 300; fade-in occurs
during this countdown. Fade-out is speed 4 and completion waits for fade state
2 (black). None of these three logo drawing/countdown handlers reads pad input.
Do not add a logo skip key without marking it as a port convenience.

`func_0022A460` is a real card check/prompt gate before A's fixed hold. Its
states run `func_00229770`, `func_00229C00`, or `func_00229C90`; card/no-card
conditions can require a prompt. Automatically bypassing this entire gate is
a port policy, not evidence of original unconditional progression. A formatted
PS2 card (type 2, format 1) proceeds to `func_00229A70`, which checks both ports
for directory `BASCUS-97112-DS00-00`. An existing save permits completion.
Without one, `func_00229960` tries the two free-cluster counts through
`func_00228320`: required space is
`ceil(D_0028A4A0->header_byte_count_at_0x18 / 1024) + 18` clusters. Either port
with enough space selects +5 = 1; the next `func_00229A00` sets state0 = 3, then
`func_0022A460` returns 1. No card or insufficient space enters a prompt.
Mapping a writable native save directory with sufficient free space to that
successful branch is an explicit native storage adaptation.

Fade state at 0x0028A9A0: 0 = clear/idle, 1 = revealing, 2 = black/hold,
3 = covering. `func_001AEE10(4,0)` reveals by subtracting 4 from the 255-level
black overlay; `func_001AEDE0(4,0)` covers by adding 4. A full transition takes
64 ordinary updates. The direction labels in the current `func_001AEE70.c`
comments are reversed relative to the visible black overlay; use the state
writes and arithmetic, not those labels.

## Intro and title task

`func_001AC070` state 0 sets black, clears attract-cycle count
`D_00275BD4`, and because `D_00275BDC==0` selects state 1 with task+0xE=1.
State 1 delegates to `func_001AC3B0`:

1. Stop current sound/stream work.
2. Once `D_00282157==0`, request **movie selector 0 / E900.PSS** and advance.
   The main loop services this request later in the same game iteration.
3. After playback returns, advance the substate once more.
4. Return completion; title task moves to state 2 and resets its substates.

Movie driver `func_00203350` configures movie output, calls
`func_00203460` until done, restores the normal rendering/audio path, and
clears `D_00821058`. The normal task loop is suspended inside this call;
the decode pump performs its own input servicing (`func_001B57E0`, the pad
read the main loop also runs).

**Movie skip:** `func_002036E0` tests **held** pad word `D_00810E70`.
With scratchpad byte 0x70003B90 clear (the title task clears it), allowed bits
are **0x8F0 = START plus all four face buttons**. Other contexts allow only
START (0x800). The request is accepted only when signed stream field +8 is
at least **11**. `func_00109FD0`/`func_00109E68` write that field as the completed
MPEG picture index (`D_00241404 - decoder_state[0xAC]`), incrementing the global
per completed picture. It is not queue depth, display suppression, or VBlank
count (the NEARMISS header of `func_002036E0.c` still calls it a queue depth).
Use picture index 11, about 0.367 s at 30000/1001 fps. On skip,
`func_00206B70` sets stream state +0xA8 to 1 and the decoder drains/tears down.

Title state 2 runs `func_001AC480`:

* Initialize both audio-channel levels to 0x5998, timer to **1200**, cursor
  to **0** (or 1 when arriving from death), and request module **1 / chunk01**.
* Wait for module-ready and `func_001FB370(D_0028A4A8)` success, then reveal.
* Draw `func_001AC7F0` every tick. Input is accepted only when fade state is 0.
* START or CROSS pressed edge (**0x840**) confirms. It takes precedence over
  DOWN (**0x4000**) and UP (**0x1000**). The three positions are clamped to
  0..2, with no wrap. Movement cue is 5 (played only when the cursor moves);
  confirm cues are 0x5DD/0x5DE/0x5DF.
* Any held button resets the 1200 counter. No held button post-decrements it;
  old zero begins idle fade-out (therefore 1201 eligible idle ticks).
  A held button during the fade cancels the timeout unless black was already
  reached: the black-completion check happens first.
* On confirmed black: cursor 0 (**New Game**) installs gameplay (`func_001ACEC0`) with
  `D_00275BE0=0`; cursor 1 enters `func_00225AC0(0)` save/load flow; cursor 2
  enters `func_00200A40` options. The decoded labels are New Game / Load Game /
  Option (singular).
* On timeout: alternate movie-entry state 1 and attract state 3 using +0xE.
  State 3 calls `anim_frame_top_a` (0x001ACA20), cycles attract selection 0..2
  when it finishes, and returns to the title prompt. It is not the same thing
  as `anim_frame_top_b` (0x001AE040), which is the gameplay/pause flow.

`func_001AC7F0` has readable NEARMISS C (97.84173%). It plays a
one-time entry cue **0x5DC**, draws a four-tile background via `func_001ABF90`, then
three overlaid option sprites whose TEX0 selection depends on cursor +0xF.
A fullscreen scaling of the entire exported 512x768 atlas is not faithful:
this is a sprite sheet, and the selected/inactive options occupy separate
regions. Keep image composition work tied to these draw calls.

## Decomp provenance of the startup functions (2026-10-09)

Read-only `tools/decomp/audit_link_provenance.py` over the 2026-10-09
`build/obj` artifacts: which object the linker uses for each function. The
2026-09-22 version of this table is superseded. Every startup function
except the five NEARMISS rows below now links from compiled C, the
jump-table dispatchers through local `.rodata` pinned at the original
addresses (`tools/decomp/rodata_pin.py`).

| Function | Linked from | Notes |
|---|---|---|
| `func_001AB7E0`, `func_001AB9D0`, `func_001AC070`, `func_001AD360`, `func_001AEE70` | compiled C | jump-table dispatchers; local tables pinned |
| `func_001ABC60`, `func_001ABE10`, `func_001AC3B0`, `func_001AC480` | compiled C | `func_001AC480` was promoted from NEARMISS 97.33% |
| `func_001ABF90` | compiled C | the four-sprite compositor, now readable C (64-bit TEX0 arguments) |
| `func_001FF080` | compiled C | no longer forced to assembly |
| `func_002032C0`, `func_00203350`, `func_00203460` | compiled C | movie table, lifecycle and wrapper |
| `func_001AC7F0` | original assembly (NEARMISS 97.84%) | the C omits three unreachable instructions (544 vs 556 bytes) |
| `func_002034C0` | original assembly (NEARMISS 96.10%) | MPEG setup/callback scheduling residual |
| `func_002036E0` | original assembly (NEARMISS 93.33%) | movie pump: input, skip and drain |
| `func_0022A460` | original assembly (NEARMISS 82.25%) | card check/prompt gate |
| main loop 0x001AAE40 | original assembly (NEARMISS 82.25%) | structural: one symbol with two entry points (the VBLANK handler at 0x001AB140) |

For every compiled-C row the audit found the linked filler's text and
relocations (and, for the dispatchers, the pinned `.rodata`) equal to the compiled
object. Residuals of the NEARMISS rows are in `docs/NEARMISS.md`. A score is a
claim about the object; a promotion still needs the link-provenance audit and
the byte-identical ELF gate (`tools/verify_all.py`).

## Checkpoint acceptance evidence (list of 2026-09-22)

The port's `docs/STARTUP.md` ("Behavior and verification", "Remaining fidelity
work") and `docs/FIDELITY_FEATURES.md` hold the current status of each item.
The first-level census does not cover boot before the title.

1. Identify all three screen images and preserve tile positions, palettes,
   per-sprite orientation, fade/countdown overlap, and optional card prompts.
2. Decode E900.PSS with both video and original audio; natural completion and
   held-input skip must converge on the same title entry.
3. Compare captured original startup frames and the first stable title screen
   against native output. The task is incomplete with placeholder logos,
   grayscale palettes, omitted movie audio, or a stretched title atlas.
4. Exercise title DOWN/UP clamp, START/CROSS priority, fade input gate, and
   idle cancellation boundary; verify NEW GAME replays selector 0 before area 11.
5. Every matching promotion needs the canonical object measurement, actual
   filler-source audit, and byte-identical linked ELF gate. Native rendering
   fidelity needs runtime captures in addition to that decomp gate.
