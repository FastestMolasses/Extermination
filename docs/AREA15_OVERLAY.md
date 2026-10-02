# AREA15 overlay — decompilation status (lane A15C, 2026-09-28)

## What AREA15 is

AREA15 is not on the played route yet. The port's docs/SEVENTH_LEVEL_ROUTE.md
(section 3.4) found that AREA06's door [3] to AREA16 is gated on bit 2 of
D_00810847, and that a scan of every overlay and the boot ELF found one
writer of that byte: the AREA15 overlay. The C of this lane confirms the
write. The function at runtime 0x824B40 (link name 00824B00) ORs 4 into
D_00810847 at the end of script 0x828CA0 (see Behaviour). The door tables
show AREA19 -> AREA15, so AREA15 is reached from AREA19. That link is from
the tables, not from play.

Correction to the port's SEVENTH_LEVEL_ROUTE.md (section 3.4 and item 8.3,
corrected at the round-7 close): those places called the writer "the
function at runtime 0x824B80 (splat `func_overlay_AREA15_00824B40`)".
0x824B80 is a fake split inside the function. The function starts at
runtime 0x824B40, and its link name is 00824B00. The splat piece 00824B40
exists because 0x824990 calls runtime 0x824B40, and splat treats that link
address as a new function.

The code module is `OVERLAY/AREA15.BIN`: overlay id 12, text 0x3440, data
0x3580, bss 0x140C00, file 0x6A00 bytes. Like every overlay it is linked at
0x00823500 but runs 0x40 higher (the MWo3 header is loaded first). Splat and
link names are therefore the runtime address minus 0x40. A data address in
the code reads the file byte at (address - 0x823500) at runtime.

## Split handling

`overlay_match.py list AREA15` regroups the 45 splat pieces into 29
"functions". Sixteen groups have more than one piece. Fifteen of them
contain a fake split: an intra-overlay call target 0x40 into the called
function, which the link absorbs. The sixteenth (00823500) is not a fake
split: it is the entry pad with the separate init 00823540 (runtime
0x823580) after its nop sled. `true_functions` joins them because the pad
never returns. The init is its own function, and its C was already
committed (`overlay_AREA15_func_00823540.c`).

AREA15 has no register-indirect jump other than returns and method calls,
so `tools/overlay/jt_pin.py` has nothing to pin. It has no two-function
slot either: a scan of every slot for code after a return (build/a15c/hidden.py)
finds none. Fifteen compiled objects absorb one later piece each
(`[fill] 15 splat piece(s) absorbed by 15 compiled function(s)`).

## Status

30 functions: 26 are compiled C that links byte-identical (25 new in this
lane, plus the earlier init). 3 are NEARMISS readable C, linked from their
splat .s. The entry pad stays an asm body (user decision 2026-09-23). The
three hybrid-asm files 008244D0, 00824DC0 and 00825290 were replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 15`) |
|---|---|---|---|---|
| func_overlay_AREA15_00823500 | 0x823540 | 0x4 | asm pad | - |
| overlay_AREA15_func_00823540 | 0x823580 | 0x20 | C, byte-identical (earlier; init) | - |
| func_overlay_AREA15_00823560 | 0x8235A0 | 0x110 | C, byte-identical | sub0 place |
| func_overlay_AREA15_00823670 | 0x8236B0 | 0xC8 | C, byte-identical (absorbs 008236B0) | call from 0x8235A0 |
| func_overlay_AREA15_00823740 | 0x823780 | 0xC4 | C, byte-identical (absorbs 00823780) | call from 0x8235A0 |
| func_overlay_AREA15_00823810 | 0x823850 | 0x1A0 | C, byte-identical | sub1 place x2 |
| func_overlay_AREA15_008239B0 | 0x8239F0 | 0x144 | C, byte-identical (absorbs 008239F0) | call from 0x823850 |
| func_overlay_AREA15_00823B00 | 0x823B40 | 0x138 | C, byte-identical (absorbs 00823B40) | call from 0x823850 |
| func_overlay_AREA15_00823C40 | 0x823C80 | 0x1B4 | C, byte-identical (absorbs 00823C80) | call from 0x823850 |
| func_overlay_AREA15_00823E00 | 0x823E40 | 0x120 | C, byte-identical (absorbs 00823E40) | calls from 0x8239F0, 0x823B40 |
| func_overlay_AREA15_00823F20 | 0x823F60 | 0x110 | C, byte-identical | script 0x8277C0 op09 record 0x827840 |
| func_overlay_AREA15_00824030 | 0x824070 | 0x1D0 | NEARMISS 95.13% | sub0 place x2 |
| func_overlay_AREA15_00824200 | 0x824240 | 0x110 | C, byte-identical (absorbs 00824240) | call from 0x824070 |
| func_overlay_AREA15_00824310 | 0x824350 | 0x84 | C, byte-identical (absorbs 00824350) | call from 0x824070 |
| func_overlay_AREA15_008243A0 | 0x8243E0 | 0x124 | C, byte-identical (absorbs 008243E0) | call from 0x824070 |
| func_overlay_AREA15_008244D0 | 0x824510 | 0x4C | C, byte-identical (was hybrid asm) | sub0 place x2 |
| func_overlay_AREA15_00824520 | 0x824560 | 0x244 | C, byte-identical (absorbs 00824560) | call from 0x824510 |
| func_overlay_AREA15_00824770 | 0x8247B0 | 0x1D4 | C, byte-identical (absorbs 008247B0) | call from 0x824510 |
| func_overlay_AREA15_00824950 | 0x824990 | 0x1B0 | C, byte-identical | sub0 place x2 |
| func_overlay_AREA15_00824B00 | 0x824B40 | 0x148 | C, byte-identical (absorbs 00824B40) | call from 0x824990 |
| func_overlay_AREA15_00824C50 | 0x824C90 | 0x164 | C, byte-identical (absorbs 00824C90) | call from 0x824990 |
| func_overlay_AREA15_00824DC0 | 0x824E00 | 0x4C | C, byte-identical (was hybrid asm) | sub0 place x2 |
| func_overlay_AREA15_00824E10 | 0x824E50 | 0x1E0 | C, byte-identical (absorbs 00824E50) | call from 0x824E00 |
| func_overlay_AREA15_00824FF0 | 0x825030 | 0x294 | C, byte-identical (absorbs 00825030) | call from 0x824E00 |
| func_overlay_AREA15_00825290 | 0x8252D0 | 0x44 | C, byte-identical (was hybrid asm) | sub1 place x2 |
| func_overlay_AREA15_008252E0 | 0x825320 | 0x110 | C, byte-identical | sub0 place x3 |
| func_overlay_AREA15_008253F0 | 0x825430 | 0x8D4 | NEARMISS 96.42% | sub0 place |
| func_overlay_AREA15_00825CD0 | 0x825D10 | 0x8E4 | NEARMISS 96.45% | sub0 place |
| func_overlay_AREA15_008265C0 | 0x826600 | 0x244 | C, byte-identical | sub1 place |
| func_overlay_AREA15_00826810 | 0x826850 | 0x120 | C, byte-identical | sub1 place x3 |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`, except `-sdatathreshold 4` for the two NEARMISS twins,
which read the gp-relative D_00275B40). "Reached by" comes from the static
tables of `tools/area_overview.py --area 15` (placement records, script
op09 records, direct calls). No AREA15 capture exists, so none of these
was seen running. Each source header describes what the instructions do.
Header roles are read from the code, not from placement labels.

## Behaviour (from the byte-matched C)

- **The D_008107FA..FF / D_00810804 flags.** Four of the five placed
  dispatchers pick a sub-behaviour by one flag byte: 0x8235A0 by
  D_008107FA, 0x823850 by D_008107FB, 0x824070 by D_008107FC and 0x824990
  by D_008107FF. The fifth, 0x824E00, selects by the object's +0xD
  instead. The sub-behaviours advance these same flags at script
  ends. For example, 0x8236B0 sets D_008107FA = 1, 0x8239F0 sets
  D_008107FB = 1, and 0x824E50 sets D_00810804 = 1.
- **0x824B40 (D_008107FF 0).** For an object whose +0xD is 0x5A, sub-state
  0 waits for D_00810702 == 1 and starts script 0x828CA0. At the script's
  end it calls func_001C47E0(0x2B, 1) and func_001C4760(0x1C, 1). It then
  sets D_008107FF = 1, ORs 4 into D_00810847 (the AREA06 door [3] lock bit
  of SEVENTH_LEVEL_ROUTE.md) and calls func_001FB0B0(0). For +0xD 0x50,
  while D_0081077F != 1, it only animates.
- **0x823C80 (D_008107FB 2..4).** The last step of the 0x823850 chain:
  script 0x8277C0, func_001C47A0(6, 1), func_001C4760(0xE, 1), a 60-frame
  wait, then D_008107FB = 0xFF, D_008106B8 = 1, D_008106B5 = 0xF, D_008106B6 = 0,
  D_008106B7 = 0, and D_00810730/31/32/34/36 = 0x82/0x81/0x82/0x81/0x81.
- **0x825430 and 0x825D10** are two mirrored functions, placed at
  (868.5, 246.5, 950) and (868.5, 246.5, 968). They drive the angle of the object at
  *D_00275B40 (+0x74) by the player's offset from (869, 951.5) or
  (869, 966.5), with sounds 0x403 and 0x404.

## Matching notes (mwcc 2.3.3)

- The +0xD "model id" pattern needs `int id = self[0xD]; if (id == K)
  func_001B10B0(self, (unsigned char)id, ...)`. This reproduces the
  original's in-place mask of the id (the AREA01 0x825950 idiom).
- **Byte stores kept in order.** In 0x823C80, stores to D_008106B5..B8
  and D_00810730..36 must be written as extern-array element stores
  (`D_008106B5[3] = 1`). Scalar externs let mwcc schedule the zero stores
  ahead (93.55 -> 100).
- **Literal address as an alias barrier.** In 0x824B40, the read-modify-
  write of D_00810847 must be a literal-address access. With a scalar
  extern, mwcc hoists the load above the D_008107FF store (92.56 -> 100).
- **Structure of 0x823F60.** It is written as `if (obj != 0) { ...; return 0; }
  return 1;`. Early `return 1` for the null case gives a duplicated epilogue.
- **0x826600** needs the idiom-24 style int-staged `4096.0f` in the
  func_001FC3C0 call (99.92 -> 100). The `D_70003B92` test is an
  if/else with the func_001FC520 call in the else arm.
- One-case dispatch 0x8252D0 is `switch (self[3]) { case 1: ...; default:
  ... }` (AREA01 0x826CF0).

### NEARMISS functions

- **0x824070 (95.13%, 0x14 bytes long).** The original fills five delay
  slots by moving the successor's first instruction, and leaves no dead
  copy behind. Those instructions are the D_008107FC address load (in the
  state-dispatch slot), the 0x23 and 0x28 argument loads (the first
  check), and the 0x52 and 0x65 arguments (the two +0xD checks). mwcc
  2.3.3 copies the 0x23 and 0x28 loads into their slots and keeps the dead
  originals (idiom-13b). It leaves the other three slots as nops: the two
  +0xD checks load their argument after the nop, and the address load is
  emitted separately where it is used. The 0x14 bytes are those 2 dead
  copies, the 2 nops of the +0xD checks and the 1 separately emitted
  address load. mwcc 2.4 and 991202 (also tried) likewise keep dead
  copies or leave slots empty. Sibling functions with the same
  shape (0x8235A0, 0x823850, 0x824560) do keep dead copies in the original,
  and they match. The following were tried: return / goto / break exits, a
  switch local, -O3 / -O4,s, and the other compilers.
- **0x825430 (96.42%) and its twin 0x825D10 (96.45%).** The original stores the
  scratchpad value 0x70003A2C and reloads it twice. It also keeps the
  0x70003A24 product in the register through a float move from a register
  to itself. mwcc 2.3.3 forwards the stored value, and the ordering around
  the two sine calls then differs. The following were tried: extern arrays,
  a struct, volatile, literal addresses, locals and statement orders.
- **Equivalence evidence.** The lane's scratch audit (build/a15c/audit.py)
  compared only multisets of instructions after dropping every branch
  (except jal), every nop and every FP/GPR move, with registers
  normalised. It cannot see branch conditions, branch targets or register
  data flow, so it does not show equivalence. The evidence is the round-7
  review, which diffed each NEARMISS function against the original
  instruction by instruction (re-run at the round-7 close with
  `overlay_match.py check --show`, from a copy whose work directory is
  scratch). Each body is equivalent. In 0x824070
  the differences are the delay-slot filling above and the branch-offset
  shifts it causes. In the twins, the two reloads of the value just stored
  are replaced by forwarding, a store moves into a delay slot, and a few
  FPR choices differ.
- **Lane DMATCH (2026-10-02).** The `volatile`-read lever that matched
  AREA21 0x82A1D0 does not fix the twins: a volatile Spad struct gives
  94.94 (0xC longer) and selective volatile reads of +0x0 / +0xC give
  93.38-96.11. All three stay NEARMISS (docs/LEVELS_DECOMP.md section 7).

## Verification

- `overlay_match.py check AREA15 src/overlays/AREA15/*.c`: 25 new files
  100.00 BYTE-IDENTICAL, 3 NEARMISS as above.
- `python3 tools/check_no_disassembly.py src/overlays/AREA15/*.c`: clean.
- Bounded mutation sweep (single-constant edits in scratch copies):
  0x823C80 `D_00810730[0] = 0x82` -> 0x83 (98.75), 0x823E40 `% 8` -> `% 16`
  (99.97). Neither stays byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA15` (25 C objects
  plus the pad and init, 3 NEARMISS skipped). Then `tools/overlay/build.py
  --area AREA15 --no-extract --no-yaml --no-splat`: PASS. The rebuilt
  AREA15.BIN equals the extracted file (27136 bytes, full file). Every
  linked filler object's `.text` equals the compiled `.text` outside
  relocation fields, zero-padded to its slot (build/a15c/prov.py). This
  regenerated `config/overlays/AREA15.lds`, which gains only the absolute
  definitions of the new externs.
- Full `tools/decomp/build.py build` + `tools/verify_all.py` under the
  lock: see "Gate".

## Gate

One locked run (build/a15c/gate.sh full; lock acquired 21:54:11, done
22:06:29; log
build/a15c/gate_full.log): `compile_overlay_src.py` and `tools/overlay/build.py
--area ... --no-extract --no-yaml --no-splat` for AREA15 and AREA16 (both
"1/1 overlays passed"), full `tools/decomp/build.py build` (rc 0, 2211
units), then `tools/verify_all.py`: all six stages PASS. The boot ELF is
byte-identical (0x175b00 loadable bytes), 19/19 overlays pass, matched_code
is 98.62% (2152/2211), and glTF, selftest and gs-offset pass. Overlay C is
not an objdiff unit, so matched_code does not count this lane. The source
headers were reworded after that compile step (comments only), so the
overlay compile and link were run again under the lock with the final
files: AREA15 and AREA16 PASS, full files byte-identical.

## Binding

The native port has no AREA15 module, and this lane edited nothing in the
port. For a port binding, the C above is the ground truth, and the NEARMISS
bodies are correct apart from the listed scheduling. The flag writes
(D_008107FA..FF, D_00810804, and bit 2 of D_00810847) are what the AREA06
door [3] question in SEVENTH_LEVEL_ROUTE.md needs.

## Known gaps

- Three NEARMISS functions (above). The entry pad is an asm body.
- The data section (0x3580 bytes: scripts, the placement tables 0x829800
  / 0x829C90, deferred groups 0x826980 / 0x826AC0) is linked from splat's
  data assembly, as in every overlay.
- No AREA15 capture exists. The route into AREA15 (AREA19), which
  placements are live, and when 0x824B40's script runs are not played.
- The three NEARMISS files are registered in `docs/NEARMISS.md` (overlay
  rows, added at the round-7 close).
- The verification above cites git-ignored scratch scripts
  (build/a15c/{hidden,prov,audit,perm,gate}.py/.sh, build/a15c/gate_full.log
  and lane A04C's build/a04c/chk3.py). They are receipts a committed doc
  cites, so they must not be deleted. Moving the reusable checks (the scan
  for code after a return inside a slot, and the per-.text check of
  two-function objects) into tools/overlay/ is open.
