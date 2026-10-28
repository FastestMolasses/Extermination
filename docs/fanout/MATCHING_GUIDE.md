

Eleventh (final) batch, same wave (8 byte-matched, all
compiled_object_ordinary_c; three NEARMISS bodies corrected):

- func_00186A60 (a ternary clamp like twin func_0018C4B0; locals read before
  a literal-address store), func_0012E560 (both bit-0 paths store the same
  table; two-argument func_00102948), func_00171670 (twin func_00171320;
  case 1 increments the mode, case 12's test was inverted), func_00203E60
  (`int *` size table read as `arg2[0x14002]`; one capacity local),
  func_001551B0 (idiom-32 on 0x700038A4 / 0x700038B4), func_001C4A00
  (self-taking callees; idiom-32 on 0x70003B92), func_001AF2C0 (the
  0x008107xx destinations volatile), sub_The_size_of_work_area_is_too_sma
  (ee-gcc; the success path ends in the tail-call statement with no
  `return`, which is the only form ee-gcc turns into a sibling jump, so the
  original very likely had the same missing return).
- NEARMISS bodies corrected: anim_matrix_multi 96.60 -> 99.88 (three-float
  func_001B1560), func_00195130 88.48 -> 99.96 (-598.9 not -598.4; the -20
  limit stores 0.5*(t - lim)), func_00203D30 78.08 -> 99.19 (a missing
  first parameter; five-argument func_00206BA0).
- The 0x008107xx game-state globals needed `volatile` to keep store order in
  three byte-matched functions (anim_frame_top_a, func_001AB430,
  func_001AF2C0); the original likely declared that block volatile.
- fill_unmatched.py: func_0012E560 dropped from GPREL_FORCE_ASM.

### Wave summary and open decisions (2026-10-27)

134 NEARMISS functions byte-matched from readable C in eleven batches (one
subagent per function, every result re-measured, every batch gated by a
fresh build + verify_all + audit_link_provenance); about 25 NEARMISS bodies
corrected. The levers, by frequency: wrong callee prototypes / arity
(checked against each callee's definition in src/), idiom-32 scratchpad
externs, `break` vs `return` and switch case order, byte-matched twins'
spelling, idiom-24 int-staged float constants, wrong bodies. Rejected as not
honest C: permuter-made comma expressions and write-only locals.

Open, each needs a decision or a tool change before more functions can
match:
1. **Relocation naming** (func_002034C0 99.85, func_0022E7F0 99.87,
   func_001575E0): the instructions match, but the compiled object relocates
   against `D_x+n` where splat names the split symbol (or a bare literal).
   Normalising both sides to the containing data symbol + offset in the
   objdiff inputs would score them 100.
2. **Compiler mix** (func_002101C0, func_001F88C0, func_0020CDC0,
   func_0022DCD0, func_001E7D20): 2.3.3 scheduling with 991202's delay-slot
   moves. mwcc 3.0 / 3.0.1 are configured in build.py but not installed;
   the original 2.3.1.01 build would be the real answer.
3. **ee-gcc 2.96 assembler** (func_0010E088, func_0010EAA0): a per-file
   option to assemble with the 2.96 gas, which fills a call slot with `ei`.
4. **Inline VU0 asm** (func_001DB480): 100 with four row-load `asm`
   statements; not accepted without a user decision.
5. **Symbol split** (gs_readback_queue_run): the code is identical; splat
   folds the VBLANK handler at 0x1AB140 into the symbol.
6. Callee definitions that disagree with every byte-matched caller:
   func_0021BE40 (callers pass two arguments), func_0020CDA0,
   func_001FEC20 / func_001FECB0, func_00100130 / func_001000C0
   (pass-through doubles), anim_bone_array_setup (callers pass the count),
   func_0010BF10, func_001F4F90.

### Tooling batch: open decisions 1-5 (2026-10-27)

The user approved all five items above. Results:

1. **Relocation naming: done.** `tools/decomp/reloc_normalize.py` (tests:
   `tools/decomp/test_reloc_normalize.py`) reads the compiled object's
   HI16/LO16 relocations and respells the matching site of the expected `.s`
   to the compiled spelling (`D_x+off`, or a bare literal where the compiled
   side has none). It changes a site only when both sides resolve to the same
   address and the instruction words agree outside the immediate, so a real
   mismatch stays visible. `build.py compile` runs it after inject_relocs and
   reassembles the changed targets. func_0022E7F0 scored 100 with no source
   change; func_002034C0 and func_001575E0 followed with corrected bodies.
2. **Compiler mix: blocked.** No mwcc 3.0 / 3.0.1 / 2.3.1.01 build is on
   disk. The five functions stay NEARMISS until the user supplies one.
3. **ee-gcc 2.96 assembler: done.** `// EEGCC_AS: 2.96` in an eegcc file makes
   `tools/eegcc/ee-compile.sh` assemble with the 2.96 gas (it fills a call or
   return slot with `ei` or a preceding load; 2.9 never does). Matched
   func_0010EAA0 (C unchanged), func_0010E088 and func_00113478.
4. **Inline VU0 asm: approved for the VU0 class** (user, 2026-10-28: "allow
   inline VU0 macro asm for the VU0-class functions, the same way as
   func_001DB480"). First done for func_001DB480 (four GCC-style row-load `asm`
   statements; with them mwcc stops strength-reducing the grid loop). Rules:
   only the VU0 macro-mode instructions the original contains (COP2 loads and
   stores, qmtc2 / qmfc2, ctc2 / cfc2, the v* macro ops, vcallms) may be
   `asm("...")` statements; every other instruction stays C. No `.word`, no asm
   for non-VU0 instructions (di / ei / sync / MMI stay excluded), comments
   describe the VU0 work in prose. SDK libvu0 helpers (0x001026A0..0x00103230)
   follow the SDK's own libvu0.c shape (C functions whose bodies are inline
   VU0 asm blocks). Extended by the user 2026-10-28 ("approve the
   float-to-GPR move (plus its nop) before a VU0 transfer as an inline asm
   statement"): where the original moves a float into a GPR (mfc1) and pads
   it with a nop immediately before the qmtc2 / ctc2 that hands it to VU0,
   that move and that nop may each be their own asm statement. Nothing else
   outside VU0 macro code is approved (general nop padding, di / ei / sync,
   MMI and multi-instruction statements stay excluded).
5. **Symbol split: done.** `config/symbol_addrs.txt` names the VBLANK handler
   at 0x1AB140 `func_001AB140`; its existing CodeWarrior `asm` body moved
   unchanged to `src/func_001AB140.c`, and gs_readback_queue_run (the code was
   always identical) is a 100% unit. A re-run of `build.py setup` changed only
   those two `.s` files.

Also: `build.py setup` now runs splat as `python -m splat` with stdin closed.
The venv's `splat` launcher kept the shebang of the iCloud copy of the repo
it was created in, and one file read there blocked for two hours.

Byte-matched in this batch (14): func_0022E7F0, func_002034C0, func_001575E0
(relocation naming); func_0010EAA0, func_0010E088, func_00113478 (2.96 gas);
func_001DB480 (inline asm); gs_readback_queue_run (split); func_001AC7F0
(`break` not `return`, restoring three unreachable slot-fill leftovers),
anim_matrix_player (one variable carries the requested clip in both arms),
func_00107CF0 (`(int)(frac >> 1)`), func_0014C680 (state 0 falls through to
case 1; idiom-32), func_001B3F10 (idiom-32 query vectors), sub_pts_is_not_used
(ternary clamp).

### Queue batch after the tooling batch (2026-10-27)

13 byte-matched (all compiled from C; audit_link_provenance clean):
func_001D8690 and func_00181E20 (ternary clamp / one combined result test),
func_00199770, func_001814E0 (a wrong body: the retry also runs when the
copied byte is neither 0x34 nor 0x1E), func_001E7050, func_001BFFD0 (each arm
stores its own flag instead of a shared `goto`), func_00116DB8 (ee-gcc: the
slot index re-read at each use), func_0011DBB8 (ee-gcc fdlibm: the float word
read inside a GET_FLOAT_WORD-shaped `do { ... } while (0)` block, which makes
ee-gcc align the following label), sub_TTY_receive_error (a missing second
argument), func_00112440 (2.96 gas only).
**2.96-gas sweep:** every eegcc NEARMISS re-measured with `// EEGCC_AS: 2.96`
and no other change: func_00112AD0, func_00112B90, func_00112C58 byte-match;
seven gain against a fresh baseline and now use it (func_0010AA80 92.19 ->
98.75, dma_wait_and_submit 89.66 -> 97.55, func_00111680 91.28 -> 94.68,
func_0010A400, dmac_reset, func_00108300, sub_Invalid_macroblock_type_code_0).
NEARMISS improved and parked: func_001D89D0 99.89 (one flag-byte reload in
v0 instead of v1; callers pass a 4th argument that func_001D8690's definition
lacks), func_001EF780 97.69 (easing-tail constant order shared with the
func_001EF510 family), func_001D0720 97.31 (compiler mix).

### Queue batch 2 (2026-10-27)

19 byte-matched: func_0015A2C0 (an if/goto chain that is a `switch`),
func_00217090 and its twin func_00218640 (literal addresses for the
0x002821Bx / 0x00810CAx words keep source order; the twin's slot-search loop
spelling), func_00185760 (a 16-byte struct copy, not a u128 cast),
func_0010AA80 (an unused int return declared void), func_00115068 (the name
buffer is +0x20 of the D_0027B100 block), func_001B1630 (volatile anchor
floats, as its twin), dma_wait_and_submit (five copies of the register dump
that ee-gcc cross-jumps into one), func_00134500, func_00157F60 (returns int
1), func_001C1030, func_001FEE60 (twin func_002032C0's shape), func_00146110,
func_00198440 (one array extern per scratchpad word), func_001963A0,
sub_sceMpegGetPicture_decode_error (a top-tested wait loop; `long long`
frame size), func_001328D0 (each success path its own `|= 4`), func_0015EC50,
sub_BASCUS_97112_DS00_00_EX_DATA_00_2 (inner switch cases `return`; twin's
SaveTask struct). Improved and parked: func_0014B7B0 99.86 (one FPU
coalescing choice), func_001EF780 98.06 and func_001EF1C0 96.80 (the family
tail as an in-place `*p += (0.02f - *p) / 10.0f`; the remaining residual is
the constant-argument order at the func_001CFB50 calls), func_001370C0 97.53.

### Queue batch 3 (2026-10-27)

19 byte-matched: func_001509A0 (each clamp reads the volatile scratch once into
a temp and stores the temp), func_00197390 and func_001F9660 (an if/else or
`!=` chain that is a `switch`; cases listed in reverse of the compare order),
func_00154120 (anim_advance_time takes self; six per-address globals are one
block), func_00198F10 (`void *` callee parameters so no conversion reorders
the argument setup), func_00109068 (separate PTS/DTS locals; no early return),
func_001D9070, func_00185E30, func_0010DD00 (store order only), func_00181730,
func_0014A730 (a store in each if/else arm, not a default-then-overwrite
temp), func_00107A28 (ee-gcc: a short-circuit `&&` result), func_001D63B0
(twin func_001D6580's spelling), func_00214570 (a wrong call:
func_0020B0D0((int)arg0, D_00265B80)), func_0010A650 (multiply order; an early
read into its own local), func_00171E90 and func_00171B00 (twins func_00171320
/ func_00171670), func_001D6C90 (one extra local holding the returned
pointer), func_0019A6F0 (one-int callees; indexed copy loops over
D_70003190[]). Improved and parked: func_001EE830 96.11.
**Class to investigate once:** about ten func_001E* functions (func_001EB020,
func_001EC5F0, func_001EF510 / 780 / 1C0, func_001EE830, func_001ED100 / 450
/ 7A0, func_001EEBA0 / EEB0) stop on the order in which the float constants
before their func_001CFB50 calls are materialised; per-function sweeps,
permuter runs and other compilers did not move it.
**Compiler-mix parks may be spellings:** func_0019AFE0 looked like a 991202
slot-fill case but matched under 2.3.3 once one gate became a block-local
`unsigned int` (sibling func_0019AD00); re-try the five compiler-mix parks.

### Queue batch 4 (2026-10-27)

8 byte-matched: func_0019AD00 and func_0019AFE0 (twin func_0019A6F0's
spelling; the path-B gate as a block-local `unsigned int t = (flags & 1) ?
0x40 : 0;` — func_0019AFE0 had looked like a 991202 slot-fill case),
func_002177B0 (a wrong state-1 body; twin func_00217090's literal
addresses), func_00118790 (ee-gcc: store order, `cursor + (int)base`),
func_00156620 (switch cases `break`; the range test `D_x = 50.0f; D_x *=
50.0f;` reproduces an FP self-move), func_0018DD20 (20 scratchpad externs,
five constant stores kept literal for their aliasing order), func_00205740
(statement order only), func_00106490 (a post-increment test, `if (n++ >=
K)`). Improved and parked: func_001936E0 99.24, func_0019BC40 99.12,
func_001A0B10 97.36, func_001D2730 95.52 (register colouring). The mwcc 2.3.3
permuter is tools/match/permuter/run_func233.sh.

### Queue batch 5 (2026-10-27)

13 byte-matched: func_001E0380 (statement order before the loop),
func_00173DD0 (a 16-byte struct copy; the callee declared as its definition
says), func_0013EDB0, func_001FDDB0 (four body errors: flush order, an
explicit empty `case 0`, inner cases `break`, `i <= n`), func_0020A7A0 (a
0x20-byte record array; u64 tex0 argument), func_0018CE60 (scratchpad words as
lcf symbols, array elements where needed), func_00181F60 (twin
func_00181E20), func_0013BF20 (volatile relocated staging words; the old
comment had the retry logic inverted), func_0016BC40 (one shared exit; the
switch compare constant reused as an argument), func_001F2BA0 (`i % 2` row
index; for-loop shapes), func_00162A40, func_00108DB0 (ee-gcc: the loop
counter set only by its `for`; 16-byte struct table), func_00119AA0 (ee-gcc:
struct-array table and a cursor struct). Improved and parked: func_001A7BA0
98.69, func_0010DEB8 97.92.

### Queue batch 6 (2026-10-27)

8 byte-matched: func_0018CBD0 (the falloff local is never initialised in the
original; see CURIOSITIES 21), func_00218D90 (a colour clamp that is an
ascending `switch`; twin func_00218640), func_0018F870 (twin func_0018DD20's
SPAD set and shared-tail `goto`), func_0013C8C0 (a coin flip as `switch`; each
turn result stored in its branch), func_001D91A0 (the real twin is
func_001F6210; a three-argument func_001028B8), func_001E3630 (one scratchpad
store through the extern, the other three literal), func_0013F770 (the
compound `D_700038A0[1] += 10.0f` of twin func_0013EDB0), func_0017E250
(callee pointer parameters, as their definitions declare).

### Queue batch 7 (2026-10-27)

5 byte-matched: func_0015A070 (the enable-bit test spelled as twin
func_00158EC0: table byte first, `1U << n`), func_001107C8 (ee-gcc: the +0x10
request word cleared as a pointer, so it no longer aliases the int store),
func_00217FA0 (twin func_00217090's literal addresses; the wanted id re-read
each pass), func_00112758 (both RPC globals volatile, as its siblings),
sub__02d_02d_02d (the glyph string written through one base pointer).
Improved and parked: func_0022EEF0 99.95, func_00195130 99.99, func_0016A8B0
94.06.
**Tool:** tools/decomp/reloc_normalize.py now also respells R_MIPS_GPREL16
sites (`%gp_rel(D_x+off)` against the compiled base symbol); a gp-relative
byte inside a small global was the only residual of sub__02d_02d_02d.
**Constant-order class, a hypothesis:** mwcc 2.3.3 materialises float
constant call arguments strictly left to right; in func_001F28D0 the original
set up the second constant first. With func_0016F600, func_001916C0,
func_0018BC20 and the func_001CFB50 / func_001CFAE0 callers (about 30
functions) parked on this, the class is probably a compiler-version
difference (the original's 2.3.1.01), not a source spelling.

### Queue batch 8 (2026-10-27)

5 byte-matched: func_00111680 (ee-gcc: one RPC word volatile, reloaded at
each use) and the new file func_00111740 (the thread creator, split out of
func_00111680's old C after splat named it; written from its target: a
result local with one return, ThreadParam-shaped struct fields filled
stackSize / gpReg / func / stack / initPriority), func_0017C860 (a Vec4
struct scratch vector; storing a difference to a scratch extern and reading
it back reproduces an FP self-move), func_00197870 (the final range test's
arms were swapped), func_001181B0 (ee-gcc: table fields read as `T[i].f`,
not through a cached element pointer). Improved and parked: func_001916C0
99.31, sub_Invalid_macroblock_type_code_0 99.39, func_001EEEB0 98.88. The
first of the 57 functions with no C file at all is now decompiled.

### Queue batch 9 (2026-10-27)

5 byte-matched: func_001C7C00 (each clamp branch stores all eight floats and
returns itself; one global gp-relative, so -sdatathreshold 4),
func_0019E640 (twin func_0019E280; `i == 2 || i == 3`), func_001469B0 (twins
func_0014BB10 / func_001418F0: counter tests wrap the stores instead of an
early return), func_00219870 (switch cases `break`; each branch writes its
own effect fields; an unsized extern stays absolute), func_00151200 (per-slot
`signed char[4]` arrays indexed by i). Improved and parked: func_0021AAC0
99.89, func_001C3DB0 99.61, func_001373B0 99.06 (if/goto dispatch was a
`switch`; state 1 had a wrong body).

### Queue batch 10 (2026-10-27)

3 byte-matched: vif_build_unpack_const (the quadword count an unsigned local,
`n + 1` and `(n << 16) | ...`, which stops the constants folding; the second
packet built through its own pointer), func_002036E0 (one `while (a && b)`
tail loop; `> 10` compares; a struct member for the 0x50000-offset field),
func_001FCF90 (an inverted loop arm; declaration order; an integer-sum
address). Improved and parked: func_00131740 99.88, func_0019FE50 96.91,
func_001CABA0 94.04 (the 2.3.3 permuter reached 100 only through a
self-assignment, which was rejected).

### Queue batch 11 (2026-10-27)

4 byte-matched: func_001FA0D0 (`idx += 1`; two adjacent bytes as one struct
extern), func_001FE920 (the function takes two parameters and passes them
through; the old `(void)` signature was the whole residual), func_00178910
(loop index declared first; two scratchpad vectors as `extern Vec4`),
func_0011E748 (ee-gcc fdlibm wrapper in its twins' shape; the domain-error
path returned the wrong value, a behaviour fix). Improved and parked:
func_00144C20 98.62, func_00150F40 97.92.

### Queue batch 12 (2026-10-27)

9 byte-matched: func_001A9360 (the callee func_0011E748 is a one-float sqrt,
not `(void)`), func_00113D08 (ee-gcc: three logic errors, among them the last
error saved only in debug mode and a goto-into-loop retry), sub_c_invalid_
motion_type_d_2 (ee-gcc: a `char` table index; separate flag variables; one
compare against a local holding 2, which the permuter found — plain C but an
unusual spelling), func_001F4010 (the slot pointer built as base, then
`+= index * stride`), func_00156F30 (switch cases `break`; the rig as an array
of 4x4 matrices), func_00177030 (values stored to scratchpad and read back),
func_001084B0 (ee-gcc: an int-returning callee declared void caused the v0/v1
swap), func_0013EB90 (returns void), func_0018C5A0 (twin func_0018C4B0's
shape). Improved and parked: func_001DA310 99.09, anim_advance_time 96.76
(compiler mix). A mechanical sweep that moves `r = f(...); if (r == 0)` into
the test lifted func_00159B90 (90.64) and func_001EF9D0 (90.07) and matched
none.

### Queue batch 13 (2026-10-27)

12 byte-matched: func_001B3440 and func_001B32F0 (a call result assigned
inside its own test, `if ((r = f(...)) == 0)`), func_001581A0 (two cases
sharing one body), func_001D30A0 (the global re-read in a block-local per
use; an int array indexed `T[frame * 32]`), func_001179E0 (ee-gcc: ternary
absolute values; one nested expression instead of temporaries),
func_001DAFA0 (one grid aggregate; the inner counter declared first),
func_0019DF10 (a wrong body in five places), func_00157360 (`void *` callee
parameters; one 8-word block), func_001FCBD0 and its sibling func_001FCE30
(74.38 -> 100 from the sibling's spelling: a 32-bit 5th parameter, a `desc`
local, one Ctx record), func_001FD0E0 (the glyph dispatch is a fall-through
`switch`), func_00191D40 (twin func_00192010's ternary min; its old
"2.3.1-only codegen" verdict was a spelling). Improved and parked:
func_001486E0 98.33, func_00108300 95.88 (a volatile completion counter; four
callee return types), func_0019F1A0 93.20.

### Queue batch 14 (2026-10-27)

8 byte-matched: dmac_reset (ee-gcc: volatile channel-register pointers keep
the six clears in order; a plain indexed loop), func_0015AC00 (each case
writes all three fields; case labels sharing a body in compare order; a case
falling into `default:`), func_0012F100 (state 2 lacked a range test; the
probe vector constants were wrong), func_00199C50 (a table value hoisted
before the loop), func_0011C7B0 and func_0011D878 (fdlibm `__kernel_*` shapes
with GET/SET_FLOAT_WORD; 66.98 -> 100 for func_0011D878), func_00106D80
(ee-gcc: a trailing call in each arm; the clamp in its own statement),
func_002117D0 (callee arities; one float temporary reused for two roles).
Improved and parked: func_0015FDF0 98.95, sub_EXTERMINATION 94.31.
**Local data:** func_0011C7B0 and func_0011D878 declare `static const float`
constants. audit_link_provenance routes them `original_assembly_local_data`:
the unpinned local data makes the build link the original assembly, so the
boot ELF stays byte-identical but these two do not yet count as C routes.
Pinning that data (as tools/decomp/rodata_pin.py does for jump tables) or a
spelling that emits no local data is open.

### Queue batch 15 (2026-10-27)

11 byte-matched: func_0019AA80 (destination array and 0x700031D0 as relocated
externs; the loop counter declared first), func_001BC560 (stores through
literal addresses keep each load-add-store in order: mwcc never moves a load
above a store to a different literal address), func_002082B0 (a wrong tag
constant, an int NLOOP, one 128-bit clear), func_00109E68 (ee-gcc: the arm
result in a block-local; the flag set last), func_00185A10 (the cone distance
used the wrong vector; -sdatathreshold 8), func_0019E930 (pointer strides of
one element; a short field), func_001D6BA0 and func_001D64A0 (the DMA packet
family spelling: `char **` table, slot `&T[idx]`, cursor `slot[4]`, body
pointer before the cursor advance), func_0019D770 and its twin func_0019CF50
(each span arm stores its own halfwords; a `for` walk), func_001368D0 (direct
`return 0` guards; one scratch word as an unsized relocated array). Improved
and parked: func_0010A400 96.23 (constant order).

### Queue batch 16 (2026-10-27): asm bodies replaced by C

12 byte-matched. **Four CodeWarrior `asm` bodies replaced by readable C** and
their src/readable/ companions retired (git rm; history keeps them):
func_001D7000 (written in the func_001D7100 / func_001D71A0 family style; it
returns the tag pointer read first), and func_00205F90, func_00204250,
func_001CC170 — their companions already scored 100% "on their own" but
needed their leaf callee in the same TU; `// TU_STATIC:` (added in decomp
lane round 7, after the companions were written) now provides that.
Eight more: func_001D6E60 and func_001DD7B0 and func_001D9720 (func_001006D8
takes six arguments, char * plus five shorts; func_001008C0 takes eleven;
GIFtag words as 64-bit bitfield structs), func_001D71A0 (returns the tag
pointer; twin func_001D7100's `CUR(v, off)` slot), func_001D7510 and
func_001D7A80 (the packet family spelling; a `(x == 0) ? 0 : K` default),
func_001E0C80 (a struct field pointer taken once), func_00106540 (ee-gcc:
post-increment spin tests; a plain 64-bit load folds its address).
**Next pool:** 100 src/readable/ companions remain (asm-bodied functions with
readable near-miss C); ~15 score >= 95%.

### Queue batch 17 (2026-10-27)

9 byte-matched: func_00114360 (ee-gcc: func_00123418 is a strncpy-shaped
copy returning its destination; a void declaration moved the request pointer
from v1 to v0), func_001C40B0 (the D_00810Cxx meters `volatile`; strict
clamps; one value read into a local and reused), func_00178080 (a one-argument
callee declared with two; named scratchpad externs), func_00117BA0 (ee-gcc: a
record-pointer field stepped with `+=`), func_00159B90 (switch cases `break`;
a result test as an if / else-if chain; an int callee parameter), func_001D8C30
(a ternary clamp; a file-scope `static const float` bias that folds and emits
no data but sets the constant up early), func_001AF8E0 (statement order: each
list set up in turn), func_001E4610 (dispatch and size pick as switches;
values updated in place), func_00187CC0 (an if/else ladder; a flag set in each
arm; a named scratch block). Improved and parked: func_001D7C30 99.68,
func_00117918 94.20, func_001D6930 83.69.

### Queue batch 18 (2026-10-27)

6 byte-matched: func_00107310 (ee-gcc: the order of an if/else test decides
the branch-likely inversion), func_0022FCA0 (one scratchpad slot as a
relocated extern; branch sense of the arc pick), func_001B62C0 (one combined
condition with two call sites where the C duplicated cos/sin calls),
func_0019D330 (collision walker: each span arm stores its own halves, a nested
type filter, early `continue`s), func_0011FD88 (ee-gcc newlib fflush: a FILE
struct, the reentrancy record as the init argument, newlib's CHECK_INIT
`do { } while (0)` block), func_0019F730 (collision walker: four scratchpad
words as relocated externs, triple indexing, a pointer-walked list). Improved
and parked: func_00112150 99.51, sub_Invalid_modion_type_ignored_d 99.02 (a
wrong DMA destination constant fixed).

### Queue batches 19 and 20 (2026-10-27)

17 byte-matched. Batch 19: func_001A0B10 (segment hit-scan: scratchpad
externs; pass 2's bound copy reuses `hit` as its counter, so a later 0x8000
prim repeats the hit block, which only rewrites the same values),
func_00198050, func_00199FA0, func_001B65C0, func_001FF3F0. Batch 20:
func_00112610 and func_001129E8 (ee-gcc RPC pair: volatile RPC state words,
`// EEGCC_AS: 2.96`, func_0010DFD8 takes (buffer, size); 001129E8's
"branch-likely wall" was an 8-argument call to the 9-argument func_0010E8A8),
func_001236D8 (newlib vfprintf: the formatter returns int, so the tail call is
not a sibling jump), func_0018CA90 (six adjacent floats are one struct, which
stops mwcc batching the loads), func_0019DB50, func_0019CB60, func_0019C830
(collision walkers rewritten in the shape of the matched twin func_0019DF10:
short-pointer tables, scratchpad short bounds, one short per node),
func_0010CD58 (typed ring struct; the callee took the struct, not an int),
func_001B55E0 (record-pointer argument, `signed char` selector, two
scratchpad floats as relocated externs), func_00193EB0 (goto chain is a
switch; old body had an inverted D_00810701 test and swapped band arms),
func_0017AF70 (swapped blend arms; the second angle uses the linked entity),
func_00153770 (one combined reset condition; the flags word read once).
Improved and parked: gs_vram_readback 97.89 (jump-table rodata for ee-gcc),
func_00117428 99.16, func_00210030 99.57 (constant order), func_00199220
99.06 (one FP schedule), plus the batch-19 near-misses func_0019FE50 98.54,
func_001854E0 93.47, func_001A6440 99.87, func_00132FB0 98.98.

Lesson: objects must not change under a running gate. Batch 20's files were
copied into src/ while the batch-19 gate was building; the link then took
objects compiled from the old NEARMISS C for functions no longer marked
NEARMISS, and the boot ELF differed (2224 bytes, all in those functions). The
rerun with every file in place passed. Results now wait in to_accept.txt until
the gate finishes. `reconcile_functions_csv.py` gained a `c_local_data` status
for objdiff-100 C whose local .rodata cannot be pinned (func_0011C7B0,
func_0011D878), which had stopped it since batch 14.

### Queue batch 21 (2026-10-27)

11 byte-matched: func_001221E0 (newlib `_pow5mult` in newlib's own shape),
func_001E8E80 (gp-relative flag word; per-cell distance locals stop the loop
hoist), func_001F9CF0 (old body had the ring-size test arms swapped; state
switch with a case-0 fall-through), func_001C92C0 (twin of func_001C90D0),
func_0019A180 (if/else arm order; one scratchpad word as a relocated extern
frees a delay slot), func_00111818 (ee-gcc: record argument with inline
buffers), func_00158590 (the facing test is a stored flag; array and volatile
scratchpad externs), func_001118B8 (ee-gcc: volatile thread-state words, store
order, `// EEGCC_AS: 2.96`), func_001C5FB0 (digit loop: plain for-loop, flag
set at the end of each arm), func_00228C90 (record array indexed in plain
for-loops; one gp-relative name buffer), func_001B2E50 (loop test re-reads the
scratchpad count, as the matched twin func_001B2F70 does). Improved and
parked: func_001CFFE0 97.89 (caller-saved colouring after a call),
func_001A06A0 99.64 (two FP registers swapped).

### Queue batch 22 (2026-10-27)

7 byte-matched: func_00194DB0 (old body clamped only one side and snapped
in-range values; level-3 yaw step is nested), func_001E0E80 (unsized array
extern keeps one global absolute; ternary clamp; named quotient),
func_001FE070 (old body never reset the width accumulator on a newline;
`-sdatathreshold 4`), func_001BD9F0 (one switch with shared cases replaces two
early returns), func_001114B8 (ee-gcc: an early return instead of a ternary
gives the branch-likely the old header called impossible), func_001A9480 and
func_001A8F40 (scratchpad radius words as relocated externs, squared in place,
as the matched twins func_001A9360 and func_00156620 do). Improved and parked:
func_001FB3E0 99.54 (the loader state is one struct), func_001F0720 98.88,
func_0010D160 99.67 (instruction stream identical; its local .rodata needs
pinning), func_0020F950 98.67 (callee arity; swapped wrap arms),
func_001FD950 99.24, func_001E8B90 99.01 (mwcc 2.3.3, not 2.4).

The strongest lever of batches 21-22: scratchpad vectors written through
literal 0x7000xxxx addresses. As relocated extern arrays indexed by element,
mwcc may move loads past the stores as the original does; literal addresses
pin them. Several "scheduling" and "FP-colouring" walls were this.

### Queue batch 23 (2026-10-27)

11 byte-matched: func_001BBBF0, func_0022E630 and func_00210F30 (scratchpad
vectors as relocated extern arrays; 00210F30 also passes a sign-extended
constant where the old C had an unsigned long long), func_00117D70 (ee-gcc:
typed voice/song records as in the matched sibling func_00117EE8; store order),
func_0019A570, func_0019A440 (callee arity: `(void)` callees were passed
pointers; indexed copy loops as in func_0019B7D0), func_00194D10 (a `ret`
local returned at the end, as in func_0012E2C0), func_0018B9C0 (callee arity;
case order; separate ifs; one float array for four adjacent globals),
func_00199DB0 (nested tests as in func_00199FA0), func_00169250 (the old C
passed the state byte where the original passes arg0), func_00219550 (one
switch for the states; the first callee takes self+0x1F0; argument order of
func_001C5570; the second height bound is floor+12; the rig state becomes 3).
Improved and parked: func_0011A070 92.90 (old C ran the scan for an
out-of-range index; register copy in the first block).

### Queue batch 24 (2026-10-27)

3 byte-matched: func_0018D910 (the two scratchpad results are relocated
externs adjusted in place; the old body wrote one the original leaves alone on
a third-probe miss), func_0019B6C0 (indexed copy loops as in func_0019B7D0),
func_0017B910 (`-sdatathreshold 4`; one extern per scratchpad word; a clip
limit local. It declares func_0017B490 as returning long long and
func_001C61D0's second parameter as short, which is how the original
sign-extends them; their definitions in src/ say short and int, to reconcile).
Improved and parked: func_001F3340 98.83, func_0012ADC0 99.87 (float copies
where the old C copied ints), func_00158810 99.75 (FP and integer colouring
only).

### Queue batch 25 (2026-10-27)

8 byte-matched. The segment hit-scan family, rewritten in func_001A0B10's
spelling: func_001A2AE0, func_001A1B80, func_001A1390 (identical to 001A1B80
apart from one kind threshold), func_001A32C0, func_001A3980 (identical to
001A32C0 apart from its kind gates). Every old body in this family was wrong in
behaviour, not just spelling: pass 1 used `continue` where the original stops
at the first entry with bit31 clear; kind gates were inverted or misplaced; the
AABB axis order was wrong (it differs per function); the prim dispatch had a
default case the original lacks; and hit blocks wrote a local or a phantom
floor clamp where the original stores into the high-y bound word 0x700031A4.
Also func_001A7280 (segment vs the fixed mesh D_008102B0, in the shape of
func_001A6AD0; the old bounds test was inverted and it copied the plane point
instead of the normal), func_001F77B0 (state 0 falls through into the animate
pass; corner switch; loop locals declared in saved-register order) and
func_001F1AA0 (the random value is the signed seed >> 16 masked with 0xFFFF;
the AREA00 twins' float spelling). Improved and parked: func_001A2370 92.49
(delay-slot fills from the fall-through block that mwcc 2.3.3 does not do;
likely the 2.3.1.01 compiler), func_001D2300 93.62 (argument setup of two
matrix calls; old C doubled a texture stride the original does not).

The port should re-check its copies of these functions: the corrected C
changes collision and particle behaviour.

### Queue batch 26 (2026-10-27)

3 byte-matched: func_001FDB80 (one state switch; unknown states return 1, not
0; the tail reset is nested; the +0x34 index is passed to func_001FD6A0 as a
full 64-bit value, while that function's definition declares an unsigned int
parameter, to reconcile), func_001EBD20 and func_001EBC30 (twins: the float
at 0x70003434 is element 13 of `extern float D_70003400[16]`, bumped with a
compound `+=`; the old literal-address `x = x + K` was the "FP-constant emit
order wall"). Improved and parked: func_001DD2F0 89.51 (the templates are
0x50/0x28 float-pair tables, not 0x48/0x38 blobs), func_001F7E40 84.91 (seven
random draws per particle, each read before the seed advances; the old C drew
too few), func_0019C6F0 92.38, func_001E3D90 97.04.

Constant-order class, new evidence: in the 0x001EA000-0x001EE000 wrapper
family (func_001EC1F0, func_001EC3F0 and siblings, one func_001CFB50 call then
func_001CFBE0 calls) the only residual is the order of the 1.0 / 5.0 / 1e-6
setup. A throwaway function using 1e-6 compiled before func_001EC1F0 in the
same file lifts it from 80% to 99.6%, so the order depends on the earlier
functions of the original translation unit, not only on the compiler version.
Compiling with the real preceding byte-matched neighbours (TU_STATIC-style) is
being tested.

### Queue batch 27 (2026-10-27)

7 byte-matched: func_001E5AC0 and func_001E67C0 (twins: per-bank locals,
setup after the wrap-fraction adds, the 0x700038A0/0x700038B0 vectors as
relocated externs, area selection as one switch), func_001EBF10 (the random
bias is 0.0001f, not 1e-6; `-sdatathreshold 4`; float array elements where the
old C stored int bit patterns), func_001FC280 (the high-half shift is signed;
volatile sound-state words), func_00124F58 (newlib `__swsetup` in newlib's
shape, `short _flags`), func_00120CE8 (newlib dlmalloc 2.6 `malloc_extend_top`
with its 32/64-bit and signed types), func_001CB2C0 (packet family: body
pointer before the cursor advance). Improved and parked: func_001EC1F0 93.55
(now matched in batch 28 through a TU neighbour), func_001DD600 86.52 (do-while
loop shape; argument-register colouring).

TU neighbours (constant-order class). Compiling a function after its real
immediately preceding byte-matched neighbour, as a `// TU_STATIC:` static copy
that is removed after the guard, reproduces the original float-constant setup
order in the 0x001EA000 wrapper family (func_001EC1F0, func_001EACF0,
func_001EBE10, func_001EBE90 reach 100%; the same bodies alone score about 80%).
The mechanism is compiler state left by everything earlier in the file, not a
semantic dependency: only the immediate real predecessor works, and longer
chains change already-matched neighbours. A neighbour need not be called; the
copy becomes an unreferenced undefined symbol. tu_static.py's guard now
resolves the copy's HI16/LO16 pairs against undefined D_XXXXXXXX externs that
the expected object spells as bare constants (`resolve_named_addresses`), so a
neighbour that reaches scratchpad words through `// SPAD:` externs still passes
the byte comparison. Promotion order matters: a dependant only matches once its
neighbour's own C is in src/, and editing a neighbour's file (even its extern
declarations) can un-match its dependants (they then fall back to the .s, so
the build stays safe).

### Queue batch 28 (2026-10-27)

15 byte-matched. Through TU neighbours (`// TU_STATIC:` with the real
immediately preceding byte-matched function): func_001EC1F0 (after
func_001EBF10), func_001EACF0 (after func_001EAB50), func_001EBE10 (after
func_001EBD20) and func_001EBE90 (after func_001EBD20 and func_001EBE10). Sound
side, typed Voice/Chan/Song/Seq records as in func_00117EE8: func_00118EC0 (the
second loop reuses the inner counter; one chained store), func_00118078 (the
scale written `(x << 2) * tempo / 60`). Others: func_001CEEE0 (separately named
globals are fields of one struct passed to func_001CFBE0; the stack quad is a
struct), func_00128B80 (one if/else instead of an early-return guard),
func_0022A460 (three callees take only the state pointer; one switch),
func_001B37D0 (a goto/parity loop is two nested for-loops; the scalars handed
to func_001026A0 are one float array), func_001FF1E0 (unsized arrays keep two
globals absolute; chained header read as in func_001FF3F0), func_001FB9F0 (one
(table, id) pair per id range feeding a shared tail), func_001F1180 (the old
glow visibility test was inverted; nested switches), func_00161020 (state 1/2
guard failures return instead of running the shared tail; func_001607D0 gets
the entity, not the state byte), func_0015C1F0 (case lists in reverse of the
original's compare order; `case 0: default:` shared bodies). Improved and
parked: func_0011EBA8 99.68 (newlib `_dtoa_r` with native doubles; the
instruction stream is identical, but its local .rodata block needs pinning and
the soft-float routines need their libgcc names), func_001D98A0 95.25,
func_0011A4E8 83.30, func_0010CE28 90.70.

### Queue batch 29 (2026-10-27)

5 byte-matched: func_001186B0 and func_00119650 (sound side, typed records as in
func_00118078; the flag test written `== 0` with the mask arm first; byte
copies in the original's order), func_001FE9E0 (a plain switch with a `ret`
local, as in func_001FECB0), func_00118418 (typed Voice/Event records, plain
loops with `continue` guards; func_00117BA0's result is used as a signed int
while its definition returns unsigned char, to reconcile), func_001AF220 (the
three patched state bytes are volatile because the original reads two of them
back). Improved and parked: func_001E13E0 99.10, func_0010D2C8 88.01 (its 64-bit
`/` and `%` are written as explicit calls because the symbol map has no
libgcc names for those helpers), func_00107648 93.66, func_00110508 93.88 (the
old C returned 0 where the original returns func_00110368's result),
func_001DAE10 85.68 (the remaining block is VU0 macro code; a 100% version with
inline VU0 asm is kept outside src/ because inline asm is approved for
func_001DB480 only). Rejected: a 100% func_001B11E0 spelling that needs a local
assigned inside an expression and never read.

### Queue batch 30 (2026-10-27)

10 byte-matched: func_001190B8 (sound: typed TempoBlock with volatile state
words), func_0011CE20 (fdlibm `__kernel_rem_pio2f` in newlib's shape: the
q0 == 0 shift is 8, the tables are const, scalbnf is (float, int); its file-local
constants become `static const` data, so it routes `local_data` in this batch;
batch 31 spells them as macros), func_00113A58 (RPC: SignalSema takes the id by
value; the 8-byte result copy is a memcpy from the uncached address),
func_001115D0 (the RPC completion callback; all eight RPC state words volatile),
func_001F4190 (particles: each velocity component is stored right after its own
random draw), func_0010EFD0 (libkernel file-I/O client init: func_0010E8A8 is
SifCallRpc with all nine arguments), func_001BA1F0 (script runner: one
`do { switch } while` loop; the stay and advance scans differ),
func_0010F870 (`-fno-schedule-insns`, as two matched files already use),
func_00102530 (SDK DMA wait: the channel as a volatile register block, CHCR
bitfields cleared through a temporary copy), func_001F3FA0 (a large offset
symbol is a struct field offset inside D_0028F700). Improved and parked:
func_00228320 81.51 (mwcc copies the 12-byte template through a float register
where the original uses integer loads).

### Queue batch 31 (2026-10-27)

12 byte-matched. SDK low range: func_00110368 (libkernel IOP reset in the SDK's
shape; the packet is an extern 64-byte-aligned struct with psize/dsize
bitfields; sceSifGetReg takes and returns unsigned int), func_00111950 (void
return, so the last DeleteSema is a tail jump; volatile state words;
`// EEGCC_AS: 2.96`), func_00107098 (the old C stored a bogus address on the
inverted condition), func_00102468 and func_001021B0 (libdma waits in the shape
of func_00102530). Others: func_001C1F50 (all four level-key tests are
switches), func_001D21E0 (hardware registers as CodeWarrior bitfield structs, as
in func_001AB590), func_00117CB0 (sound: typed Seq/Tempo records),
func_00182BF0 (grouped OR bail-outs in the original's order), func_001C5930
(three switches; the area-5 name draw passes D_00265520), func_001176E0 (the
exact and partial voice masks are separate), and func_0011CE20 again: its
fdlibm constants are now macros, so it compiles to code only and routes as
ordinary C. Improved and parked: func_001DB9D0 96.08 (the start vector's sine
and cosine were swapped), func_00208750 99.81, func_00118CF8 80.17 (strength
reduction of the record stride), func_001DFA40 98.55 and func_001DF600 99.92
(both matched in batch 32), func_001DF180 81.46, func_00101CD0 91.34 (libdma
sceDmaPutEnv; a 100% spelling needs register reads into locals that are never
read and is held back).

### Queue batch 32 (2026-10-27)

11 byte-matched. libdma (the matched SDK DMA family: a volatile DmaChan register
block with CHCR as a ChcrBits bitfield, a `--count < 0` countdown, STR cleared
through a temporary-copy loop, the final CHCR read into a ChcrBits local, its
fields set in the original's order and written back once): func_00101F08,
func_001020C8 (MOD = 2 interleave) and dma_kick (the old C had its MADR test
backwards). Sound: func_00115850 and func_00115E50 (typed Voice/Event/Song/Tempo
records; func_00117BA0 takes two arguments and returns int), func_00119C98 (the
product reads back the just-stored field). The ripple twins func_001DFA40 and
func_001DF600 (the X step in a named local assigned just before the conversions;
the warp sum in its own temporary). Others: func_00226070 and its callee
func_002267A0 (switches with cases -3/-2, -1, 0, default; the old C returned the
wrong value in state 1 and changed a flag the original leaves alone),
func_001AAD00 (each list publishes its count, then its cursor, in descending
address order). Improved and parked: func_001F4A10 80.55 (the rest is VU0 macro
code), func_001002E0 91.15, func_00108818 97.79.

### Queue batch 33 (2026-10-28)

6 byte-matched: func_00102368 (libdma, MOD = 2; the old C cleared the wrong
CHCR bit), func_001FC7B0 (one struct copy of a 128-byte template; an
`unsigned char` character local; a plain while loop), func_001AF800 (each
record is cleared with quadword stores, as in its twin func_001AF890),
func_00110608 (struct-array slot table; the two address computations keep the
original's grouping; an SDK-sized local DMA descriptor array of which only
entry 0 is used, which gives the original's frame), func_001E10A0 (water
ripple: plain indexed loops over `WaterVtx D_0081E0F0[16][16]`; the old C had
the splash rows in a different order and a different impulse constant),
func_0010F9E0 (a record with a mode word and a 0xFC-byte name copied by a
bounded loop; SifCallRpc with nine arguments). Improved and parked:
func_001DF180 83.14, func_001CBC20 92.37, func_001172B8 98.21 (the voice
allocator; see CURIOSITIES 32), func_00116598 99.80 (one constant gcc folds
from a compare), func_001CC3B0 90.13 (old packet constants were wrong).

### Queue batch 34 (2026-10-28)

8 byte-matched. libsifrpc in the SDK's shape with SifRpcClientData_t-style
records: func_0010E6F8 (SifBindRpc; the blocking path is the then-arm of
`!(mode & 1)`; the rpc_id store precedes the packet-address store) and
func_0010E4C0 (SifGetOtherData; rpc_id read into a local first, as in
SifCallRpc). The two bind-retry loops func_00112E28 and func_00111F18 (a failed
bind prints, delays and retries; a zero server word also retries; only a
nonzero server word clears the latch; the old C did neither). libdma
func_00102278 (MOD = 0; the old C had the MADR test inverted and returned
CHCR). Sound: func_001177E8 (one `&&` chain; the selector's arm order),
func_0011A2B0 (the voice-steal routine, see CURIOSITIES 32), func_001FD790 (the
+0x34 index is a 31-bit bitfield; the old C tested a flags byte the original
never reads). Improved and parked: func_00119EA0 91.74 (the old C had the
0x8000 test inverted), func_00117088 82.88.

### Queue batch 35 (2026-10-28)

9 byte-matched plus one symbol split. Split: splat had merged func_001B99F0
(0xC bytes, script opcode 0x09, a tail jump through the record) and the opcode
0x0A handler at 0x001B9A00 (0x19C bytes) into one 0x1AC symbol; the existing C
already matched both halves. config/symbol_addrs.txt now sizes func_001B99F0 as
0xC and names func_001B9A00 (new file src/func_001B9A00.c); the word at
0x1B99FC is alignment padding, and the link still places it (boot ELF
identical). As with the func_001AB140 split, docs/FUNCTIONS.csv keeps the
original row and size. Wrong compiler labels: the four MPEG decoder error stubs
sub_sequenceScalableExtension_is_not, sub_pictureTemporalScalableExtension,
sub_Unknown_Extension and sub_pictureSpatialScalableExtension_ are ee-gcc SDK
code (a void call in tail position becomes the tail jump); they had scored 0%
under mwcc. Others: func_00106948 (the callee func_00106490 returns long),
func_002081A0 (an `int` length where the old C had short), func_0010A998 (two
inline helpers recompute each buffer address as the original does),
quat_nlerp (the clamp as a ternary). Improved and parked: func_001E1AD0 96.08,
func_00103DC8 74.77, func_0010A248 94.26, func_001EEBA0 98.88, and near-misses
whose remaining code is VU0 macro code or privileged instructions (inline asm
needs a user decision): func_001CD070 45.25, func_001DA6A0 84.63,
func_0010B160 71.89 (also relabelled ee-gcc), plus the hand-written assembly
`_start` 26.40, func_001029E8 and func_00105088 (relabelled ee-gcc; their
headers now say they are hand-written).

### Queue batch 36 (2026-10-28)

2 byte-matched: func_00207E40 (`-sdatathreshold 4`; the packet family's
spelling; the texture width and height shifts as locals reassigned in place,
which keeps the UV word ahead of the header stores), func_001C1570 (the old C
misread a cos() argument and the interpolation step; both random draws are
inside the loop; four globals are fields of the record passed to
func_001CFBE0). Improved and parked: func_001EE4E0 95.99, func_001EE190 95.99
(the old C passed 0 where the original passes 1 to func_001CFBE0),
func_0022BBC0 96.10 (a same-file static callee via `// TU_STATIC:`; the
caller-register reads of CURIOSITIES 21 kept as locals left uninitialised on
those paths).


### Queue batch 37 (2026-10-28)

10 byte-matched. Six effect spawners finished by idiom-24 (int locals
converted to float, larger value first, only in the spawn whose constants the
original sets up early): func_001EB020, func_001EB250, func_001EB440,
func_001EB600; two by a TU neighbour (`// TU_STATIC:` of the real predecessor):
func_001ECB00, func_001ECFB0. Four readable companions replaced their asm
bodies: func_00204390 (the scaled sector position in its own local before the
modulo add), func_0019F330 (volatile int reads of the node's list offsets, the
func_0019F680 spelling), func_001F4BF0 (the second 3.0 size as an int converted
to float binds the constant to the second argument register), func_001D6F60
(returns the entry packet pointer like its sibling func_001D7000; the companion
returned void). Improved and parked: func_001EC270 95.31, func_001ED100 96.93,
func_001ECE70 80.61 (its fourth float argument is 0.1, not 1e-6 as the old C
had). func_00225720's companion now carries func_00207E40's corrected prototype.


### Queue batch 38 (2026-10-28)

27 byte-matched. The constant-order spawn family is finished by a second lever
beside idiom-24: where the original sets up the 1e-6 argument early, the source
computes it from an int local, `eps_i = 1000000; eps = 1.0f / (float)eps_i;`
(a plain `1.0f/1000000.0f`, a double, or a float local do not keep the early
setup). Per call, apply idiom-24 and the eps local only to the constants the
original sets up early, in its order (scale, then unit, then eps); some
siblings need `-sdatathreshold 4`. Matched this way: func_001EAF00,
func_001EBBB0, func_001EC3F0, func_001EC470, func_001EC5F0, func_001EC820,
func_001EC9A0, func_001ED100, func_001ED450, func_001ED7A0, func_001EDAF0,
func_001EDE40, func_001EE190, func_001EE4E0, func_001EE830, func_001EEBA0,
func_001EEEB0, func_001EF1C0 (func_001EDAF0 / func_001EDE40: the old C passed 0
where the original passes 1 to func_001CFBE0). Nine readable companions replaced
their asm bodies: func_001D7080, func_001F54E0, func_0019ED80; func_00126BE8
(libgcc fp-bit unpack_d shape with `NO_DENORMALS`: the dead denormal loop still
aligns the next label); func_00191390 (`case 0:` listed in the default group
keeps the original's compare for move 1, as in func_0016CD70); func_001CBA50 /
func_001CC1E0 (declaration and zeroing order, a `switch` for the dollar glyph,
the index scaled in place; func_001CC1E0 also a `// TU_STATIC:` of its leaf
callee func_001CBE10); func_001CA3B0 (x and y half angles in fresh locals, z
halved in place as `z = z / 2.0f`); func_0011D770 (newlib __kernel_sinf in
newlib's source shape, constants as `#define` literals so no unused .rodata).


### Queue batch 39 (2026-10-28)

18 byte-matched; 16 readable companions replaced their asm bodies.
- newlib / libgcc in their own source shapes (GET_FLOAT_WORD / SET_FLOAT_WORD
  do/while(0) union blocks, `ix &= 0x7fffffff;` as its own statement, newlib's
  if / else-if chains and switch order, constants as `#define` float literals so
  no unused .rodata is left): func_0011DE90 cosf, func_0011E2A8 sinf,
  func_0011E398 tanf, func_0011CCC8 __kernel_cosf, func_0011C128
  __ieee754_asinf (newlib's misleading-indent polynomial kept), func_0011DF78
  fabsf, func_0011CB90 __ieee754_sqrtf, func_001278C0 fp-bit unpack_f
  (FLO_union_type bit-fields plus the NO_DENORMALS dead branch),
  func_001205D8 __sfvwrite (no initialisers at declaration; `len` read first).
- SDK: func_0011B910 / func_0011B5E0 (VIF resets) only needed
  `// EEGCC_AS: 2.96`; func_00109B20 (callback table typed as a struct, the entry
  indexed per use); func_00108790 (bit reader: clears and the position store
  before the byte cursor).
- Game code: func_00177460 and func_0019A310 by idiom-32 (one relocated extern
  per scratchpad word via `// SPAD:`; mwcc then reloads x and keeps z as the
  original does); func_001B1EA0 (indexed vertex reads, if/else next index, far
  vertex first); func_001EC270 / func_001ECE70 (constant order: scale/unit/eps
  int locals; 0.1 as `1.0f / (float)tenth_i`).
Improved companions: func_0021B1B0 99.47, func_001A4830 99.87.


### Queue batch 40 (2026-10-28)

4 byte-matched. func_001C2770 and func_001486E0: idiom-32 with one VOLATILE
relocated extern per scratchpad word (vec4 slots as float arrays used as [0],
a pointer word as `char *volatile`), callee pointer parameters as `void *`;
func_001486E0 also needs idiom-24 for the 15.0 in one timer arm only.
func_00134090: the scratchpad vectors and the source vector as volatile Vec4
externs read field by field (mwcc24). func_001D04B0 (companion, 81.67): source
PARAMETER ORDER. EE float parameters arrive in f12.. and ints in a0..
whatever their position, so callers cannot tell, but mwcc's argument-copy
schedule follows the source order: the function declared
`(char *obj, float a, float b, int a1, void *a2)` and its callee func_001CFA60
declared floats-first. This lever was tried on eight other near-misses with no
effect (their residuals are load order or colouring, not argument copies);
it did help func_0014BC30 (callees declared float-first, 87.60 -> 99.84).
Two findings: with mwcc 2.3.3 a load from a volatile relocated extern right
after a store to it is forwarded (the stored value is reused), while a plain
extern or a literal volatile cast is re-read; for floats copied from the
scratchpad the last-declared local gets the highest FPU register.
Improved: func_001E4CE0 99.96 (eps lever), func_0014BC30 99.84, func_001F0720
99.02, func_001B8AB0 95.37 (one shared zero-return exit); companions
func_001A4D10 98.85, func_00205240 97.07 (the packet cursor is a 64-bit value
end to end), func_00133DB0 91.33.


### Queue batch 41 (2026-10-28)

9 byte-matched; 6 readable companions replaced their asm bodies.
- func_001741D0: the "original never fills delay slots" residual was source
  shape, not a compiler wall: the compare-chain switch's cases end in `break`
  (not `return`), a field clear moved after an if/else, and `arg0[6]++`.
- func_00139E00: volatile player-block globals, `void *` callee parameters (as
  its twin func_0013A3B0), one value read into a block local before a call.
- func_001B41F0: idiom-32 per-address externs (volatile for the words that are
  stored then re-read, plain for the spray words).
- SDK (ee-gcc): func_00108608 (bit-reader init: statement order only),
  func_00108660 (post-increment inside the OR), func_00109AF8 (the CbTab struct
  of func_00109B20), func_0011B9E0 (returns the value it writes: the original
  leaves it in v0; plus `// EEGCC_AS: 2.96`), func_00108AA0 (libmpeg demux:
  STRICT ALIASING — the event type as an enum, header/data as
  `unsigned char *`, unsigned len decide ee-gcc's store/load order; loop
  rotation by the condition order), func_0011E0A8 (newlib modff).
Improved: func_00137EB0 99.89, func_00144040 99.38, func_001A50A0 99.61,
func_00177B80 95.31.


### Queue batch 42 (2026-10-28)

Tooling: tools/decomp/rel_order.py. objdiff pairs each REL-style R_MIPS_HI16
with a following R_MIPS_LO16 to recover its addend, and GNU as reorders HI16
entries differently from ee-gcc's assembler, so an object with byte-identical
.text and the identical relocation set could score below 100%
(func_00116598: 99.98). When both hold, the target object's .rel.text is
rewritten in the compiled order; anything else is left alone. The target is
an objdiff input only, never linked. build.py runs it after renormalize
(147 targets reordered, no score lowered) and the agents' m1.py runs it per
trial.

8 byte-matched; 7 companions replaced their asm bodies: func_00116598 (sound
voice update: the 12-bit length compared as an unsigned short, plus the
relocation order); func_0011B328 (GIF reset: `// EEGCC_AS: 2.96`);
vtable_a0_at_0011FE90_off24 (newlib __swrite over the full FILE struct;
_lseek_r / _write_r return `long`, 64-bit on ee-gcc); func_0011E080 (newlib
isnanf); func_001216B8 (newlib _mbtowc_r without MB_CAPABLE: newlib's test
order); func_00101810 / func_001015A8 (libgraph half-offset setters: the
register as an `unsigned long` bit-field struct and the SCE_GS_SET_XYOFFSET
macro expression, `short halfoff`); func_0010E3A8 (sifrpc reply callback: a
third, empty `case` label makes ee-gcc build the original's decision tree).
Improved: func_00117428 99.31, func_001172B8 98.32, func_00107648 94.47;
companions block_copy 86.70 and func_00121920 81.23 (newlib memcpy / memmove:
built by ee-gcc 2.95.3, which is in tools/eegcc but has no build.py mode yet),
func_0010BCD0 95.37.


### Queue batch 43 (2026-10-28)

11 byte-matched; 4 companions replaced their asm bodies.
- func_001DEE80 / func_001DEEC0: `// TU_STATIC: func_001DEDB0` (the original
  keeps a value in a1 across the same-TU leaf); CFLAGS must be the callee's
  own `-sdatathreshold 4` or tu_static's guard rejects the static copy.
- func_00204E90 (IPU): `int` (not unsigned) locals for FP / IFC, the
  original's statement order before the semaphore wait, declaration order.
- func_001D6DD0 (DMA packet): the packed doubleword as its own first local,
  one `DmaTag **slot` pointer, the body pointer taken before the cursor advance.
- func_00108818 (bit reader, ee-gcc): an `int` parameter widened only at the
  shift; declaring it 64-bit recoloured every temporary.
- func_001E1AD0 (`ch = BASE; ch += off;`), func_00199220 (`ys /= 16.0f;`).
- gs_vram_readback: wrong body (the pad-drain loop belongs inside the
  `if (rb != 0)`), `// EEGCC_AS: 2.96`, and its jump table: run
  tools/match/gen_jtbl_rodata.py for the function so build/jtblrodata/ has it.
- func_0021AE90: returns void (the old C returned 0; no caller uses it); both
  dispatches are switches whose cases end in `break`.
- func_001459A0: the float-first parameter order on three capsule-test callees
  (same registers, different argument-setup order).
- func_001CC3B0: declaration order and in-place packet-pointer advances.
Improved: func_001E3D90 97.69, func_001CFFE0 97.97, func_0020F950 99.90,
func_001F5F60 98.51 (two wrong-body fixes), func_0019C6F0 92.50,
func_001CBC20 92.45, func_00119EA0 92.13; companion func_002047D0 97.55.


### Queue batch 44 (2026-10-28)

2 byte-matched: func_001063E8 (SDK: the register read through a constant
address at each use, the guard compares and returns a local, the watchdog is a
post-increment test), func_00146F80 (wrong body: a first move-probe hit always
returns 1; the threshold is pi/4). Improved: func_0021A500 98.38 (the PRNG seed
and the draw colour are separate), func_00205050 98.39, func_00117088 84.37,
func_0010D2C8 89.23.
Signature reconciliation: 82 files now declare and define functions the way
the original calls them (returns that pass v0/f0 through, unused trailing
parameters callers really pass, 64-bit `long` in newlib, SDK syscall stubs,
float-first parameter order where callers need it). Every file was re-measured
before and after with the same score. Kept for matching, commented in place:
func_0017B490 returns short, func_001C61D0 keeps an int parameter,
func_001FD6A0 / func_001FE480 keep 32-bit first parameters,
func_001FECB0 / func_001FE9E0 keep an argument-less call declaration,
func_001D5C80 keeps one parameter.


### Queue batch 45 (2026-10-28): VU0 class, libvu0 helpers

User decision: inline VU0 macro asm approved for the VU0 class, written as in
func_001DB480 — ONE VU0 instruction per `asm __volatile__("...")` statement
(tools/check_no_disassembly.py rule [c] rejects 3+ per statement), operands
as %N, everything that is not a VU0 macro instruction in C. 14 libvu0 SDK
helpers (func_001026A0, func_00102718, func_00102738, func_00102760,
func_00102850, func_00102870, func_001028B8, func_001028D0, func_001028E8,
func_00102900, func_00102918, func_00102990, func_001029C0, func_00103230)
are ee-gcc C with real prototypes. Levers: a float scalar reaches VU0
through `register float t asm("$8")` assigned after the first vector load;
a float result leaves through a `"=r"` output; u128 row copies stay C with
register variables. Kept as asm bodies: func_001026D0 (hand-written loop),
func_00102A60 / func_00102B08 / func_00102BB0 (return address kept in a2,
second entry labels mid-function), func_001029E8 (branches on the caller's
a3, VU0 ops in delay slots), func_00102798 / func_001027E0 (MMI).
mwcc notes: asm strings may not contain \n (';' separates), "=r" outputs
warn about an uninitialised variable; with split statements the order in
which pointer locals are assigned decides v0/v1.


### Queue batch 46 (2026-10-28): VU0 class, game code

11 byte-matched with one-instruction VU0 asm statements: func_001DAE10,
func_001E2BA0 (per-point clip as a `static inline` helper; `-sdatathreshold 4`),
func_001D72D0, func_001CD070 (clip flags read into an int through a "=r"
output), func_001CD180 (per-word `// SPAD:` externs as unsized arrays),
func_001D0540, func_001D5A70 (companion; clip matrix held in VU0 rows 24..27
across three vertices), func_001E04E0 (the old C passed a third argument to
func_001CAAC0), func_001CCF70, func_001DD170 (projection reuses the fog row
23 without reloading), func_001CE660 (sin / cos through scratchpad words).
Most of these were 30-60% because the old C imitated the VU0 work with FPU C.
Improved: func_001DA6A0 99.97, func_001F4A10 99.82, func_001CAAC0 99.78,
func_001F15F0 99.78, func_001D75E0 99.53, func_001F8D30 99.11,
func_001DE920 98.89, func_001E4A00 96.35, func_00179910 95.31,
func_001776E0 94.53, func_001D5C80 93.44, func_001D5370 92.76,
func_001CE860 90.67, func_001F6FB0 82.07 (its update runs inside switch case
1), func_001C9610 69.18, func_001C9940 67.74; companion func_001CF470 95.49.
Blocked by the asm rules, not by C: func_001CAAC0 / func_001F15F0 reach 100
only with the 14-instruction projection as ONE asm statement; func_001C9610,
func_001C9940, func_001CE860 need a float-to-GPR move before a VU0 scalar
load; func_001D5370 / func_001D5C80 need EE nop padding after clip-flag
reads. Each is a user decision.


### Queue batch 47 (2026-10-28): VU0 class, large functions

build_trs_matrix byte-matched (companion; the scale step is twelve
one-instruction VU0 statements). Improved from 0-23%: func_001CD520 98.72,
func_001F0A60 98.69, func_001E0750 98.32, func_001CEFD0 95.71,
func_001C69A0 94.83, func_001CDDC0 94.70, func_001E2800 94.65,
func_001CE300 94.43, func_001CD940 94.03, anim_eval_skeleton 92.91 (needs
`#pragma inline_depth(8)` so the per-node inline expands its matrix helper;
user-confirmed), func_001E17E0 90.29, func_001C7900 86.82. func_001CB6B0's
fourth argument is a 32-bit address (user-confirmed; the old C widened it).
The common wall: the original hands a float to VU0 through a fixed GPR
(float-to-GPR move, nop, VU0 transfer); mwcc 2.3.3 silently drops float asm
operands and CodeWarrior asm blocks reject float variables, so the C passes the
bits through a stack union (bigger frame, other registers). func_001CF970 stays
an asm body (MMI, absolute jump).


### Queue batch 48 (2026-10-28): VU0 class, the float-to-GPR move

User decision: the float-to-GPR move (and its nop) before a VU0 transfer may
be an asm statement. Working spelling under mwcc 2.3.3:
`asm("mfc1 $8, %0" : : "f"(x));` followed by the one-instruction qmtc2
statement. mwcc accepts the "f" operand, inserts the hazard nop itself (an
explicit asm nop doubles it, so none of the files has one), and turns the
move into a direct load into t0 when the float is still in memory — exactly
the original. Pass the float as a `float` (a parameter or local), never an
int pun or a stack union; the frame then shrinks to the original size.
4 byte-matched: func_001C69A0, anim_eval_skeleton (keeps
`#pragma inline_depth(8)`), func_001C9610, func_001C9940 (the last two also
use func_001C69A0's helpers, which copy pointer arguments to locals
destination first). Improved: func_001CD520 99.96 (rgba before the three
floats in its prototype), func_001E0750 99.75, func_001F0A60 99.67 (the two
corner offsets as one `D_70003400[2][4]` extern), func_001CD940 98.08,
func_001CE860 97.79, func_001E2800 97.62, func_001CDDC0 95.79,
func_001CE300 95.32. Residuals: the colour-repack OR operand order
(func_001CD520 / func_001F0A60), a nop mwcc adds at clamp join labels
(func_001E2800 / func_001CD940), register colouring.


### Queue batch 49 (2026-10-28): asm-word bodies to readable C

New pool: 230 functions (90 game, about 140 SDK) linked from `asm void`
bodies of raw instruction words with no readable C, plus 10 INCLUDE_ASM
stubs. Recipe (build/agentkit/DECOMP_BRIEF.md): m2c the normalised .s, rewrite
with real types and callee prototypes taken from what the call sites pass
(many stubs declared `(void)` really take `(self, ctx)`), mwcc233
-O4,p -sdatathreshold 0 unless a byte-matched neighbour differs. Levers that
mattered: switch bytes as unsigned char, compound `+=` / `-=`, no `ret`
local, if / else-if chain vs switch, switches in reversed compare-chain order,
a named `float zero` local, one plain extern per scratchpad word, a final
plain call becomes a tail jump. 12 byte-matched: func_0012DD70,
func_0012F980, func_00131510, func_00131650, func_00136140, func_00137830,
func_0013B350, func_0013DC60, func_00140690, func_0014C220, func_0014C3A0,
func_001450B0. NEARMISS (body-correct): func_00134300 98.97 (mwcc24),
func_0013BBB0 99.77, func_0013BE60 94.58, func_0014C0B0 91.45.


### Queue batch 50 (2026-10-28): asm-word bodies, game code

51 game functions decompiled from asm-word bodies and byte-matched (actor
state machines and per-tick drivers, jump-path setups, hit reactions,
targeting predicates, collision overlap / column / reach tests, a ground
probe, menu scroll steps, damage reactions, status helpers); NEARMISS
func_00154F00 97.60 (spark column) and func_00211310 99.75 (minimap marker).
Extra levers: ternaries in exactly `(x == 0) ? 1 : 0` form; arrays
(`float ground[4]`) when the original takes a stack slot's address;
pointer arguments as struct-field addresses rather than `(int *)` casts
(argument-setup order); a case that needs its own `return 0`.
Found while decompiling: func_0025F0D4..func_0025F144 (8 entries) and
func_00243BA8 are data mis-split as functions; func_00180000 + func_00180004 are
one function split mid-body (merged C at build/agentkit/split/, needs the
symbol split recipe).


### Queue batch 51 (2026-10-28): asm-word bodies, game tail and first SDK

Game: func_001B3670 (steering decision), func_001B5860 (record init),
func_001C9570 (scale / rotate / translate matrix with VU0 row scales),
func_001DC020 / func_001DC890 / func_001DCFF0 (AUTO_SIGHT reticle frame and
rings; `r = ...; r *= 142.0f;` in-place chains fix the FPU destination),
func_001DEDF0 (`// TU_STATIC: func_001DEDB0`), func_002255C0, func_002256E0,
func_00225700, func_00226010, func_00227C60, func_00229960, func_00229C00, and
the merged func_00180000. SDK (ee-gcc): func_00101EE0 (D_STADR helper, 2.96
gas), func_00102638 / func_00102658 (CHCR as the SDK tD_CHCR bit-field struct),
func_00102890, func_00102980, func_001029A0, func_001029B0, func_00102CA8,
func_00103198, func_00103200, func_00103248, func_00103290 (libvu0 with
one-instruction VU0 statements), func_00102EA8 / func_00102FB0 (view and
projection matrix builders, plain float C), func_00101898 (libgraph
double-buffer setup; DISPFB with 32-bit bit-fields), func_00109A40,
func_00109BD8, func_00109BE8, func_00109CE0, func_0010AD88, func_0010AD98,
func_0010C278. NEARMISS: func_001DAC90 99.38, func_001DB040 99.41,
func_001E00F0 93.65, func_00103140 89.09 (hand-written loop steps),
func_0010BC90 69.00 (vblank wait: the original re-reads the INTC address in
its loop).
ASM-only (no C form under the rules): padding / crt0 / syscall stubs,
vu1_cold_start, gs_readback_build_packet and func_00101630 (end in sync),
func_001032F0, func_00103380, func_00104378..func_00104F70 (MMI image loops),
func_0010ADA8 / func_0010AF28 (IPU-DMA save / restore under di / ei),
cache-op and di / ei wrappers 0x0010C020..0x0010C568.
