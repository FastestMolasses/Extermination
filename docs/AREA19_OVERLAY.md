# AREA19 overlay — decompilation status (lane A13C, 2026-09-30)

## What AREA19 is

AREA19 is below AREA13: AREA13's falls lead to its sub 0 entry 9 / 10
(00193EB0), and the ninth-level route reached entry 9 by the ladder under
AREA13's hatch [62] (docs/WORLD_GRAPH.md, route note 4 and the AREA13
arrival section). Its door table leads on to AREA03 ([25]) and, from sub
1, to AREA15 ([50] / [51]). This lane did not run the game; the behaviour
below is read from the byte-matched C.

The code module is `OVERLAY/AREA19.BIN`: overlay id 16, text 0x68C0, data
0x5A80, bss 0x15ED80, file 50048 bytes, two sub-states (placement tables
0x82E3D0 for sub 0 and 0x82ED60 for sub 1). Like every overlay it is
linked at 0x00823500 but runs 0x40 higher, so splat and link names are the
runtime address minus 0x40. A data address in the code is the runtime
address; it reads the file byte at (address - 0x823500). The boot area
dispatcher func_001E7780 calls the init at runtime 0x8250C0 (link
00825080, earlier C).

## Split handling

`overlay_match.py list AREA19` regroups the 49 splat pieces into 43
functions. Six groups hold one later piece each, all fake splits at an
intra-overlay call target 0x40 into the called function (`[fill] 6 splat
piece(s) absorbed by 6 compiled function(s)`). A scan of every slot for
code after a return (build/a13c/hidden.py) finds no second function.

One function is a jump-table dispatcher: the door 0x823580 (link
overlay_AREA19_func_00823540), a six-case switch whose table the original
keeps at 0x82F800. `tools/overlay/jt_pin.py` places the compiled table
there at link (`[pin] overlay_AREA19_func_00823540: .rodata 0x18 bytes at
runtime 0x0082f800`); `overlay_match.py check` reports it as 99.98
(`rodata-needs-pin`), and the linked overlay is the proof.

The fire 0x823D10 stores the runtime address of its collision method
0x823CA0 (link 00823C60); the C writes it as
`func_overlay_AREA19_00823C60 + 0x40` with the function declared as a char
array, which links to the original constant.

## Status

43 functions: 36 are compiled C that links byte-identical (31 new in this
lane, including the door above, plus 5 earlier files). 6 are NEARMISS
readable C, linked from their splat .s. The entry pad stays an asm body.
The five asm-body files 00825200, 008253E0, 008264C0, 00826500 and
00829330 were replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 19`) |
|---|---|---|---|---|
| func_overlay_AREA19_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA19_func_00823540 | 0x823580 | 0x1EC | C, byte-identical at link (jump table pinned) | sub0 place [27] |
| func_overlay_AREA19_00823730 | 0x823770 | 0x8 | C, byte-identical (earlier; empty) | code pointer in 0x823780 |
| func_overlay_AREA19_00823740 | 0x823780 | 0x520 | NEARMISS 99.39% | sub1 place [40] |
| func_overlay_AREA19_00823C60 | 0x823CA0 | 0x6C | C, byte-identical | code pointer stored by 0x823D10 |
| func_overlay_AREA19_00823CD0 | 0x823D10 | 0x97C | NEARMISS 93.11% | sub1 place [39] |
| func_overlay_AREA19_00824650 | 0x824690 | 0x3F4 | C, byte-identical | sub0 place [11] |
| func_overlay_AREA19_00824A50 | 0x824A90 | 0x148 | C, byte-identical | - (no static reference) |
| func_overlay_AREA19_00824BA0 | 0x824BE0 | 0x310 | NEARMISS 98.66% | - (no static reference) |
| func_overlay_AREA19_00824EB0 | 0x824EF0 | 0x74 | C, byte-identical | group 0x82b5a0[n] |
| func_overlay_AREA19_00824F30 | 0x824F70 | 0xC0 | C, byte-identical | group 0x82b5a0[n] |
| func_overlay_AREA19_00824FF0 | 0x825030 | 0x8C | C, byte-identical | group 0x82b5a0[n] |
| func_overlay_AREA19_00825080 | 0x8250C0 | 0x30 | C, byte-identical (earlier) | boot 0x1e7780 (area_dispatch_off1BC0_state1300) |
| func_overlay_AREA19_008250B0 | 0x8250F0 | 0x150 | C, byte-identical | sub0 place [6] |
| func_overlay_AREA19_00825200 | 0x825240 | 0x1D4 | C, byte-identical (was asm body) (absorbs 00825240) | call from 0x8250f0 |
| func_overlay_AREA19_008253E0 | 0x825420 | 0x1AC | C, byte-identical (was asm body) (absorbs 00825420) | call from 0x8250f0 |
| func_overlay_AREA19_00825590 | 0x8255D0 | 0x1EC | C, byte-identical | sub0 place [43] |
| func_overlay_AREA19_00825780 | 0x8257C0 | 0x164 | C, byte-identical | sub0 place [7] |
| func_overlay_AREA19_008258F0 | 0x825930 | 0x174 | C, byte-identical (absorbs 00825930) | call from 0x8257c0 |
| func_overlay_AREA19_00825A70 | 0x825AB0 | 0x1B8 | C, byte-identical (absorbs 00825AB0) | call from 0x8257c0 |
| func_overlay_AREA19_00825C30 | 0x825C70 | 0x270 | C, byte-identical | sub0 place [9] |
| func_overlay_AREA19_00825EA0 | 0x825EE0 | 0x21C | C, byte-identical | sub0 place [47] |
| func_overlay_AREA19_008260C0 | 0x826100 | 0x364 | C, byte-identical | sub0 place [18] |
| func_overlay_AREA19_00826430 | 0x826470 | 0x8C | C, byte-identical | script 0x82c6e0 op09 record 0x82c9a0 |
| func_overlay_AREA19_008264C0 | 0x826500 | 0x3C | C, byte-identical (was asm body) | script 0x82c6e0 op09 record 0x82c7a0 |
| func_overlay_AREA19_00826500 | 0x826540 | 0x30 | C, byte-identical (was asm body) | script 0x82c6e0 op09 record 0x82c8a0 |
| func_overlay_AREA19_00826530 | 0x826570 | 0x2CC | C, byte-identical | sub1 place [38] |
| func_overlay_AREA19_00826800 | 0x826840 | 0x2EC | C, byte-identical | sub1 place [37] |
| func_overlay_AREA19_00826AF0 | 0x826B30 | 0xE0 | C, byte-identical | script 0x82d290 op09 record 0x82d490 |
| func_overlay_AREA19_00826BD0 | 0x826C10 | 0x818 | NEARMISS 99.94% | sub1 place [36] |
| func_overlay_AREA19_008273F0 | 0x827430 | 0x104 | NEARMISS 94.91% | sub1 place [35] |
| func_overlay_AREA19_00827500 | 0x827540 | 0xC | C, byte-identical (earlier) | script 0x82d290 op09 record 0x82d310 |
| func_overlay_AREA19_00827510 | 0x827550 | 0x234 | C, byte-identical | sub1 place [53] |
| func_overlay_AREA19_00827750 | 0x827790 | 0x250 | C, byte-identical | sub0 place [10] |
| func_overlay_AREA19_008279A0 | 0x8279E0 | 0x124 | C, byte-identical | sub1 place [34] |
| func_overlay_AREA19_00827AD0 | 0x827B10 | 0x10 | C, byte-identical (earlier) | script 0x82e090 op09 record 0x82e150 |
| func_overlay_AREA19_00827AE0 | 0x827B20 | 0x38 | C, byte-identical (earlier) | script 0x82e090 op09 record 0x82e250 |
| func_overlay_AREA19_00827B20 | 0x827B60 | 0x264 | C, byte-identical | sub1 place [6], [7], [8], [20], [26], [28], [29], [30], [31], [32], [33] |
| func_overlay_AREA19_00827D90 | 0x827DD0 | 0x15A0 | C, byte-identical | sub0 deferred group 0x829e00[n] x2 |
| func_overlay_AREA19_00829330 | 0x829370 | 0x4D0 | C, byte-identical (was asm body) (absorbs 00829370) | call from 0x827dd0 |
| func_overlay_AREA19_00829800 | 0x829840 | 0x84 | C, byte-identical (absorbs 00829840) | call from 0x827dd0 |
| func_overlay_AREA19_00829890 | 0x8298D0 | 0x198 | C, byte-identical | sub0 deferred group 0x829e00[n] x2 |
| func_overlay_AREA19_00829A30 | 0x829A70 | 0x320 | NEARMISS 99.53% | sub1 place [46] |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`; `-sdatathreshold 4` for 0x825C70, 0x826470, 0x827DD0,
0x829370 and 0x829A70, which read gp-relative D_00275B40 or D_00275B08).
Each source header describes what the instructions do; roles are read from
the code, not from placement labels.

## Behaviour (from the byte-matched C)

- **The fire [39], the flames [40] and [38].** [38] (0x826570) is an
  examine point: +0xB bit 2 starts script 0x82CA20, and at its end
  D_00810778 = D_008107F8 = 0xFF, func_001F6BA0() and func_0019C6F0(7,
  0); it spawns the group 0x82AB00 when D_0081077B is also 0xFF. The fire
  [39] (0x823D10, at (848, 377, 920)) is removed at load when D_008107FB
  == 0xFF or D_008107F8 != 0; otherwise it burns until D_008107F8 != 0,
  then after 60 frames its size falls to 0 over 240 frames, and it ends
  500 frames after that delay. Its collision method 0x823CA0 calls
  func_001EFE00(0x80000027, other) on an object that touches it unless
  func_0021BB00(&D_008102B0) is set. The two flame columns [40]
  (0x823780, x 870, y 448) do the opposite: they wait for D_008107F8 !=
  0 and then burn (func_001F02C0 0x8E8, func_001FC3C0 0x8E9); at load
  they are removed, spawning 0x82AB00, when D_0081077B and D_00810778 are
  both 0xFF.
- **The lift [36] (0x826C10) and [37] (0x826840).** D_008107F9's low nibble
  holds the lift's position (0 or 1, 15 units up). [37]'s examine starts
  script 0x82CFF0 (nibble set) or 0x82CC70 (nibble clear, D_008107F9 |=
  0x80 until the script ends) and sets +0x2EC = 0x8A on the object at its
  +0x18. [36] counts its own +0x2EC down and then moves 1/12 unit a frame
  for 180 frames (15 units) and flips the nibble. While its +0x2EC is 0,
  standing in the box x 850..859.6, z 850.5..855 starts script 0x82D290;
  its callback 0x827540 sets +0x28 = 1, and once that count reaches 70
  the platform falls (speed += 0.2 a frame) to its rest height, and at the script end D_00810779 = 0xFF (flag 0x21)
  and func_0019C6F0(0x15, 0). The fire's smoke is hidden while
  D_008107F9 bit 7 is set.
- **[6] (0x8250F0) and D_008107F5.** Bit 0 clear: 0x825240 starts script
  0x82B6E0 when the player enters the area 0x82BD00 not below y 210; at its
  end D_008107F5 |= 3 and D_008106C0 = func_001B6660(0x82A590). Bit 2
  clear: 0x825420 starts script 0x82BA00 in the area 0x82BD40 at y 190..200,
  and its end sets D_008107F5 = 0xFF. [43] (0x8255D0) swaps to model 0x25
  once bit 0 is set.
- **[7] (0x8257C0) and D_008107F6.** At 0, 0x825AB0 waits for spawn entry
  0xA (D_00810702), runs scripts 0x82C410 / 0x82C4D0 / 0x82C690 around a
  500-frame hold of D_008102B5, and sets D_008107F6 = 1. From then on
  0x825930 runs script 0x82BD90 on +0xB bit 2; at D_008107F6 == 4 it calls
  func_001E8B40(1) and play_sound 0x8DF, and at 0xFF it ORs 4 into
  D_00810854 (this area's lock byte, the WORLD_GRAPH "0x825930 stores
  D_00810854" entry). [47] (0x825EE0) swaps its model once D_00810776 ==
  0xFF or D_008107F6 > 5.
- **[9] (0x825C70).** Once D_00810776 == 1 it waits 300 frames, plays
  effect 2 at (1012, 135, 917.6) and for 260 frames adds +0x1F8
  (0.0577) a frame to the +0x80 float of the object at *(D_00275B40 + 4)
  and twice that to the one at *(D_00275B40 + 8); it zeroes both when
  D_00810776 becomes 0xFF.
- **[18] (0x826100).** In the box x 962..968, y > 293, z 923.6..929.6 with
  D_008104A0 == 0x2A it starts script 0x82C6E0 and moves the player
  (D_00810358 down to 796.589, D_00810354 down between z 916.3 and 810);
  below z 808 it sets D_00810777 = 0xFF (flag 0x1F) with effects and
  func_001B1E20(7, 40). Its script callbacks: 0x826470 plays sound 0x134
  once when D_008102EC <= 23 and waits for D_008102BA, 0x826500 stores
  the handle of play_sound 0x135 in +0x2EC, and 0x826540 passes that
  handle to func_0011A070.
- **[10] (0x827790), [34] (0x8279E0) and the eleven [6]..[33]
  (0x827B60).** [10] runs script 0x82D990 at spawn entry 0xD and 0x82DA10
  366 frames later (then D_0081081D = 0x80), then re-arms script 0x82DD10
  on the area 0x82E050. [34] runs script 0x82E090 at spawn entry 1; its
  callback 0x827B10 sets D_0081081E = 1, which starts each 0x827B60
  object's countdown (its +0x9A); at zero each one probes below itself and
  hands its behaviour to 0x156620.
- **[46] (0x829A70).** Unless D_00810838 is set it spawns a model-0x18
  child and, on an examine with +0xB bit 0, sets D_00810838 = 1, calls
  func_001C47E0(0x25, 1), turns the two objects at D_00275B40 + 8 / + 0xC
  to -pi/2 / +pi/2 and clears D_70003B8D / B91 / B92.
- **[53] (0x827550).** Present only while flag 0x25 is set and 0x26 is
  clear; between y 445 and 460 in the area 0x82F820 it starts script
  0x82D590, and at its end calls func_001C47A0(0x24, 1).
- **Door [27] (0x823580).** Standard door steps; with bit (+0x34) of the
  area's lock byte D_00810841[D_00810700] set it takes the short path,
  otherwise it runs script 0x82AD50 and calls func_001C4760(0xA, 1)
  unless D_00810CCD is set.
- **The AREA01 twins.** 0x827DD0, 0x829370, 0x829840 and 0x8298D0 are the
  AREA01 functions 0x826D40, 0x8282F0, 0x8287C0 and 0x828850 (0x827DD0
  without the flag 6 / state 0x64 logic and with two extra probe-miss
  effects).
- **A dead store in 0x824BE0.** The effect sets its start value to 0.1 for
  model 0 and falls through to the 0.3 of model 1, so both start at 0.3;
  the original instructions do the same (0.1 is stored and overwritten).

## Matching notes (mwcc 2.3.3)

The lane's AREA13 notes apply (docs/AREA13_OVERLAY.md). Specific to AREA19:

- **Data labels.** link_overlay.py defines only splat's data labels; an
  address inside a label is written from its label
  (`&D_overlay_AREA19_0082BD40 + 1` for 0x82BD80 in 0x825420, where the
  area is a 64-byte `Poly`).
- **Code addresses as data.** Runtime text addresses stored by the code
  use existing labels (`D_overlay_AREA19_00823770`) or `func + 0x40`
  (0x823D10).
- **Aligned struct copies** (`Poly`, 64 bytes, aligned 16) for the
  func_001B1EA0 areas copied to the stack (0x825240, 0x825420, 0x827790).
- **idiom-24 int-staged float** for func_001C67E0's last argument
  (0x825240) and the spark height in 0x824690.
- **The random-fraction idiom** `r = (float)((seed >> 16) & 0xFFFF); r =
  r / 65535.0f; r += 0.0001f; seed = seed * 37 + 11;` (0x823780,
  0x823D10, 0x824BE0) and `int pkt[24]` packets for func_001CFA60 /
  func_001CFB50.

### NEARMISS functions

- **0x823780 (99.39%, same size).** Registers and order match; the original
  has one nop after the outer-loop phase store and mwcc 2.3.3 one after the
  inner-loop wrap, which shifts two branch offsets.
- **0x823D10 (93.11%, same size).** List scheduling in the flame and smoke
  loops: the float constants of the func_001CFB50 calls and of the size
  products come in a different register and order, one scratch store uses
  a different base form, and the original pads the end of the flame loop
  with one nop. Writing the +0x204 test as `== 0` first raised it from
  92.35.
- **0x824BE0, resolved by lane DMATCH (2026-10-02; was 98.66%).** In the
  second func_001CFA60 call of each case the phase +0x1F0 is read into a
  local before the random fraction is built; it is then loaded into f12
  first and a0 / a1 are set last, as in the original. Byte-identical and
  compiled.
- **0x826C10, resolved by lane DMATCH (2026-10-02; was 99.94%).** State 0's
  raise is written `(x = 15.0f) + *p` (constant in f1, loaded value in f0,
  as in the original), and the last rest-height read (+0x2E8, after the
  +0x4C method) is a `volatile` read, which keeps it ahead of the +0xB4
  load (idiom-22). Byte-identical and compiled.
- **0x827430 (94.91%, C 0x8 shorter).** The original computes the store
  address (self + 0x1F0) + 0xF4 as a pointer before the compare (its first
  add fills the case-1 dispatch slot) and stores through it; mwcc 2.3.3
  folds the store to self + 0x2E4. Pointer locals keep both adds but move
  them after the compare and lose the branch-likely (90.00 .. 90.69).
- **0x829A70, resolved by lane DMATCH (2026-10-02; was 99.53%).** Declaring
  the child pointer before the +0x2E8 slot pointer puts the child in s0 and
  the slot pointer in s1, as in the original. Byte-identical and compiled.
- The status table above still lists these three as NEARMISS; the entries
  here supersede it (docs/LEVELS_DECOMP.md section 7). Three NEARMISS
  remain: 0x823780, 0x823D10, 0x827430.
- **Equivalence.** Each was diffed against the original instruction by
  instruction (`overlay_match.py check AREA19 .. --show`): the differences
  are the register, scheduling and nop placements listed; calls, stores
  and control flow are equal.

## Verification

- `overlay_match.py check AREA19 src/overlays/AREA19/*.c`: 30 new files
  100.00 BYTE-IDENTICAL, the door 0x823580 at 99.98 (`rodata-needs-pin`
  only), the 6 NEARMISS as above.
- `python3 tools/check_no_disassembly.py src/overlays/AREA19/*.c`: clean.
- Bounded mutation sweep (build/a13c/mut): 0x826B30 `855.9f` -> `856.9f`
  (99.96) and 0x827790 `> 0x258` -> `> 0x257` (99.99). Neither stays
  byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA19` (37 C objects
  with the pad, 6 NEARMISS skipped), then `tools/overlay/build.py --area
  AREA19 --no-extract --no-yaml --no-splat`: PASS, AREA19.BIN equals the
  extracted file (50048 bytes, full file). Every linked filler object's
  `.text` equals the compiled `.text` outside relocation fields
  (build/a13c/prov.py). `config/overlays/AREA19.lds` was regenerated
  (absolute definitions of the new externs and the pinned-table layout).

## Gate

See docs/AREA13_OVERLAY.md, "Gate" (one locked run for both areas).

## Binding

The native port has no AREA19 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS
bodies are correct apart from the listed scheduling.

## Known gaps

- Six NEARMISS functions (above). The entry pad is an asm body.
- The data section (0x5A80 bytes: scripts, placement tables 0x82E3D0 /
  0x82ED60, groups 0x829E00 / 0x82A590 / 0x82A5F0 / 0x82B5A0) is linked
  from splat's data assembly.
- 0x824A90 and 0x824BE0 have no static reference; the group 0x82B5A0
  (0x824EF0 / 0x824F70 / 0x825030) is not traced to its spawner.
- No AREA19 capture was taken by this lane; the order of the D_008107F5 /
  F6 / F8 / F9 events on the played route is not measured.
