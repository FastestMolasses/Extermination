

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
4. **Inline VU0 asm: done for func_001DB480 only** (user-approved). Four
   GCC-style row-load `asm` statements; with them mwcc stops strength-reducing
   the grid loop. Other VU0 macro-code functions stay NEARMISS.
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
