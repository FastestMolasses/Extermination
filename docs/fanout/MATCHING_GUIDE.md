# Subagent matching guide — Extermination decomp (fan-out campaign)

You are a matching-decomp subagent. Goal: convert as many of YOUR assigned
frontier functions as possible to a **true objdiff 100%** byte match from C, so
they become human-readable `readable` source. Work ONLY on functions in your
assigned list file. NEVER fake a match.

## Absolute rules (these protect a byte-identical boot ELF)

1. A function may stay real C **only if it reaches a true objdiff 100%.** If it
   does not, restore the file to its byte-exact original: `git checkout -- src/<name>.c`.
2. **Use an ISOLATED scratch build dir** — never touch the canonical `build/obj/`
   or `build/expected/` (the orchestrator's build state depends on them). Your
   scratch dir is `build/agent_<AGENT>/` where `<AGENT>` is given in your task.
3. Do NOT edit: `objdiff.json`, `config/symbol_addrs.txt`, `docs/FUNCTIONS.csv`,
   any shared header, `tools/`. Do NOT run `build.py` (any subcommand) or
   `verify_all`. The orchestrator owns those and integrates/verifies your results.
4. Touch ONLY `src/<name>.c` for names in YOUR list, and YOUR `build/agent_<AGENT>/`.
5. Legal: never print disassembly in full to any external context (it is
   git-ignored disc-derived material). Your deliverable is the C you write.

## Toolchain (Apple `container` CLI, never Docker)

Image `exterm-toolchain:latest`, repo bind-mounted at /work. Compiler is
Metrowerks `mwccmips` (qemu-i386 + wibo32); assembler `mipsel-linux-gnu-as`.
Run from repo root: /Users/abe/Documents/Extermination.nosync/Extermination

## Per-function loop (S = build/agent_<AGENT>)

For a target `func_XXXXXXXX` (the .s label and src file are func_<VRAM> even if the
list shows a symbol name):

1. Read `build/asm/matchings/main/code/func_XXXXXXXX.s` (size in the
   `nonmatching ..., 0xNN` header). Decode to C. For a `word` function also read
   the current `src/func_XXXXXXXX.c` (its `.word` bytes / asm) to see the exact code.
2. CFLAGS line as first line of the .c: `// CFLAGS: -O4,p -sdatathreshold 0`
   (use `-sdatathreshold 4` if it uses `%gp_rel` globals).
3. Write your C to `src/func_XXXXXXXX.c`.
4. Build + diff into YOUR scratch (one container call; BATCH several funcs per call):
```
A=build/asm/matchings/main/code; S=build/agent_<AGENT>
container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c "
mkdir -p $S/expected $S/obj
N=func_XXXXXXXX
FL=\$(grep -m1 '// CFLAGS:' src/\$N.c | sed 's|// CFLAGS:||'); [ -z \"\$FL\" ] && FL='-O4,p'
mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc \$A/\$N.s -o $S/expected/\$N.o
qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c \$FL -o $S/obj/\$N.o src/\$N.c 2>&1 | grep -v MWCIncludes
"
```
   diff (host, reloc-aware):
```
tools/bin/objdiff-cli diff -1 build/agent_<AGENT>/expected/func_XXXXXXXX.o -2 build/agent_<AGENT>/obj/func_XXXXXXXX.o func_XXXXXXXX -o - --format json 2>/dev/null \
 | .venv/bin/python3 -c "import sys,json;d=json.load(sys.stdin);print([s['match_percent'] for s in d['left']['symbols'] if s['name']=='func_XXXXXXXX'])"
```
   To inspect the instruction diff while iterating, objdump both objects in $S and compare.
5. Iterate until 100.0. **Effort cap ~6 focused attempts per function.** If still
   <100%, `git checkout -- src/func_XXXXXXXX.c` and move on. Breadth over grinding.

## CFLAGS / addressing
- `&D_xxxx` extern + `-sdatathreshold 0` → a relocated %hi/%lo address pair.
- `%gp_rel` globals → `extern T D_xxxx;` (or `extern T *D_xxxx;`) + `-sdatathreshold 4`.
- Force ABSOLUTE addressing for one small global while others stay gp-rel: over-declare
  it as an array, `extern int D_xxxx[2];` (idiom #20).
- Reload idiom (#10): re-read the same pointer field before each use to reproduce
  CW's non-CSE'd reloads.

## Idiom cheat-sheet (full: docs/PROGRESS.md "matching idioms — CRACKED", ~line 1656)
1 float ops `x += K` not `x = x + K`. 2 chained mul: call result FIRST. 3 saved-reg
alloc = declaration/first-use order. 10 reload defeats CSE. 12 mwcc emits independent
scalar stmts in SOURCE order (split/reorder/materialize-arg-as-stmt to steer). 15 hoist
a global load to first statement to win a register. 16c park a constant in a dead PARAM
var to pin its register. 18 `volatile` a function-pointer FIELD to keep a beqz nop. 19
tail call = last statement → a plain jump to the target (no link). 20 array over-decl forces absolute addressing.

## Recently discovered levers (rounds 1-2 — try these)
- **slti vs sltiu split**: `if (p[5] < 3)` on an unsigned-byte field yields `sltiu`;
  to get the target's SIGNED `slti`, cache into an int first: `int v=p[5]; if(v<3)`.
- **per-access global recompute**: `*(volatile int *)0x700031F4 = x;` reproduces CW's
  per-store recompute of the address's upper half followed by a store through its low half (full-constant address only — `volatile`
  on `base+offset` FAILS, mwcc splits the offset into an upper-half load plus an OR-immediate).
- **return-value-before-void-call**: compute a call's result into a local BEFORE a
  following void call, so the computation emits before the call (matches CW).
- **interleaved quadword load/store copy**: assign 128-bit elements (`__attribute__((mode(TI)))`)
  one at a time to get CW's load-then-store per slot (vs mwcc's batch load/store).
- **idiom 12c (const call-arg)**: `n=K; f(p,0,n);` — materialize a constant arg as its
  own statement to fix "a2-before-a1" / const-vs-forwarded call-arg ordering.
- **sq-frame call-wrappers** (return a comparison of a callee result, e.g.
  `return f() < 0;`) match cleanly; `sd`-frame SDK functions do NOT (mwcc always
  emits a 128-bit return-address save) — skip those, they're a toolchain wall.
- **branch-arm store duplication**: when CW duplicates a common store block into
  BOTH if/else arms (keeping a shared const live), write the stores INSIDE each arm
  in the C source, not once after the branch.
- **named-symbol call args**: pass a global as a named symbol (`extern T D_xxxx[];`
  then pass `D_xxxx`/`&D_xxxx`) so the call-arg gets a `%hi/%lo` reloc pair;
  a raw `(void*)0xADDR` cast lowers to an upper-half load plus OR-immediate of the constant and won't match. (Stores can still
  be raw `*(volatile T*)0xADDR`; only pointer ARGS need the named symbol.)
- **quadword-store offset folding**: `*(u128*)(base+off)=0` does NOT fold (emits
  an address add, then the quadword store); use a struct member (`p->q10=0`) or array index (`((u128*)p)[1]=0`)
  to get a zero quadword store at `p+16`. (`typedef int u128 __attribute__((mode(TI)));`)
- **mixed-type vec4 stack buffer**: to get float stores plus an int store of 0x3F800000 without an extra
  pointer reg, use `struct{float x,y,z; int w;}` not `float buf[4]`+`*(int*)&buf[3]`.
- **large struct-member offset (>0x7FFF)**: declare the field at its exact offset in a
  struct (`struct{char _p[0xA0B8]; char *pA0B8;}`) to reproduce CW's
  %hi-of-offset added to the base, then the load at the %lo offset (raw `*(int*)((char*)p+0xA0B8)` gives `addiu`).
- **0.0% diff = addressing-mode divergence**: if objdiff reads 0.0% but the disasm
  looks structurally right, the FIRST instruction's addressing mode is wrong (gp-rel
  vs absolute) — fix `-sdatathreshold` or the array-overdecl size first.
- **64-bit bit-packer idiom**: GS-register packers using a 32-bit left shift then arithmetic right shift (signed) or
  a 32-bit left shift then logical right shift (unsigned), plus shifts and ORs, are matched by a `long long` expression
  with EACH field cast/shifted individually: signed→`(long long)(int)f<<k`,
  unsigned→`(long long)(unsigned)f<<k`; the callee's packed param must be `long long`.
  Do NOT group: `(long long)(a|b<<16)` fails; cast per-operand.
- **byte-global absolute addressing**: idiom #20's array over-decl must EXCEED the
  sdata threshold IN BYTES — for a byte global use `extern unsigned char D_xxxx[8];`
  (not `[2]`, which stays gp-rel under -sdatathreshold 4).
- **nop-padded busy-wait**: model as `do { r=f(); asm{nop; ...} } while(r==0);` — the
  asm nop count = (visible body nops) + 1 (the jal delay slot eats one).

## DEEP-DIVE idioms (s82 — high value, recently cracked)
- **Float-constant-local (CRACKS the float-operand-order wall)**: mwcc canonicalizes a
  float LITERAL to the first operand of `c.eq.s`/`add.s` (losing source order). Put the
  constant in a `float` LOCAL: `float zero=0.0f; if (x==zero)` → the compare with the value first and zero second
  (matches); `x+one` with `float one=1.0f;` → the add with the value first and the constant second. Source operand-swap of
  a literal does NOT work; the local does.
- **Float-compare branch shape (controls f0/f1 + polarity)**: write the body as the
  if-TRUE arm with the condition NEGATED — `if (x!=zero || ... || (flags&bit)) { body;
  return 1; } return 0;` — gives the float equality compare with f0 as its first operand, a
  branch-on-false into the body, then a branch-if-zero to the end. The `&&`-early-return form gives the wrong `bnez` polarity.
- **VERIFY CALL ARITY first (diagnostic)**: a "swapped prologue saved-reg move" near-miss
  (99%+) is very often a WRONG call argument count, not a backend wall. Read the callee's
  prologue (how many of a0..a3 it consumes) and forward ALL the real params — when a
  function saves both params and its first call forwards both, mwcc copies a0 into s1
  before the jal and a1 into s0 in the delay slot, matching CW. (Cracked func_001914A0.)
  Second symptom (2026-10-20): the target loads the first field into a2 or a3 before
  copying the parameters into saved registers, while yours copies a1 first and loads
  into a1. A later call forwards (a0, a1) unchanged, so a1 must stay live: declare and
  pass both arguments (func_0013D850 and func_0013D980, both through
  func_001418F0(self, arg1); 98.16% -> 100% for 0013D980).
- **idiom-13 REFINED**: mwcc fills a conditional-branch delay slot with the first
  SPECULATABLE (pure-ALU: upper-half load, load-immediate, add-immediate, shift, add, mtc1-feeder) instruction from EITHER
  successor; loads/stores are never speculated. Matchable from C ONLY when every
  candidate first-instruction in both successors is a memory op off an ALREADY-LIVE base
  register (no separate `lui` exposed). Globals via %hi/%lo pairs are NEVER matchable (volatile
  doesn't help — the address `lui` hoists independently of the load). Multi-function TU
  does NOT change scheduling (per-function — falsified).

## DEEP-DIVE idioms — refinements (s82 round 7)
- **Float-local vs inline literal — pick by operand position AND register need**: the
  float-local form yields the constant as the SECOND `c.eq.s`/`add.s` operand; a raw
  literal yields it FIRST. Use the local only when the value genuinely needs a register;
  for a constant consumed ONCE, use an inline literal — a float-local grabs a spurious
  saved FP reg (f20/f21...) and tanks the match.
- **Int/pointer early-return branch shape**: `if (cond==0){ work; return K; } return K;`
  (work as the if-TRUE arm + DUPLICATED return) reproduces CW's branch-block layout and
  epilogue duplication; the `if(cond) return K;` early-return form gives the wrong
  `beqz`-merge polarity. (Cracked func_001782A0; recovered polarity on several others.)
- **`unsigned short >> k` → `lhu`+`sra`**: subscripting an `unsigned short*` promotes to
  int (non-negative), so `>>` is `sra` on a zero-extended `lhu`; do NOT add `(short)`.

## PERMUTER (tools/permuter/) — for register-permutation walls
The decomp-permuter CRACKS pure register-allocation-order walls (it matched
func_00179CA0, func_001C6120). Workflow: get a 95%+ base.c, run the permuter
(it narrows to score ~30), then hand-finish the last 1-3 instructions with idioms.
It does NOT crack scheduling/instruction-count walls (it can't ADD an instruction
CW emits, nor remap the compiler-internal `$at`). See docs/fanout/PERMUTER_GUIDE.md.
NOTE: the permuter's internal score is NOT objdiff% — re-evaluate output-*/source.c
with objdiff-cli to pick the real best candidate to reseed.

## KNOWN WALLS — recognize fast, park, move on
- **CW branch-target alignment nop**: CW pads loop-test/branch-target labels to
  8-byte alignment with a `nop` inside the function body; mwcc does not align branch
  targets. Unmatchable (a single inline `asm{nop;}` forces the loop compare to `$at`).
  (func_001C6200.)
- **idiom-#13 delay-slot fill**: target leaves a `nop` in a conditional-branch delay
  slot whose fall-through first instr is SAFE (`lui` addr-hi / `li` const) → mwcc fills
  it, unmatchable (cure ONLY if the slot candidate is a memory LOAD: make it `volatile`).
- **register-allocation-ORDER**: sequence matches but registers are a permutation that
  declaration-order tricks won't fix (high register pressure). Park after ~3 tries.
- **mwcc-vs-CW branch lowering**: CW two-exit (two separate returns, useful delay slots); mwcc
  merges via an unconditional branch, compares into `$at` and zeroes with a byte-add move. No known C lever. Park.
- **saved-reg-arg-in-jal-delay-slot**: CW puts the move of a saved register into an argument register in
  a call's delay slot; mwcc hoists it out. Park.
- **FPU div/madd latency nops** and **dead-`paddub`/`b` coalescing**: backend. Park.
- **DENSE-SWITCH JUMP-TABLE DISPATCH (the jtbl wall — ~146 funcs, PROVEN unmatchable s84)**:
  any function whose target `.s` builds the table address from %hi/%lo of a `jtbl_` symbol, scales the
  index by 4, adds, loads the entry and jumps through it (a real jr-table) is UNMATCHABLE as a C `switch`. Root cause: the original emitted ALL
  jump tables to a consolidated EXTERNAL rodata TU (0x0026xxxx–0x0027xxxx; `jtbl_XXXX` is an
  *undefined external* in the expected .o, defined in a splat data .s). A C `switch` makes
  mwcc emit its OWN LOCAL `@NN` table → (a) the post-RA scheduler freely reorders it (`lui`
  into the `beqz` delay slot, `addiu`/`sll` swapped), and (b) objdiff counts the local-vs-
  external reloc as a mismatch *even when the schedule is byte-identical* (func_001B9C10:
  schedule matched, still <100% purely on `@13` vs external `jtbl_0026E0E0`). EXHAUSTED s84:
  no -O/-opt/-sdatathreshold/-model/-gpopt lever (all illegal or no effect; `-sdatathreshold 8`
  is the right default — fixes an incidental 8-byte gp-rel s64 access, NOT the wall);
  ALL section/scheduling pragmas illegal in mwcc 2.3; `#pragma schedule off` reproduces the
  EXACT dispatch but is function-global (wrecks prologue → 68%); inline asm that loads the external table's address
  emits the byte-exact dispatch but mwcc-2.3 asm has no C-var operands / no reg-pinning and
  dead-code-eliminates the case bodies; no computed goto (C89). Reference mwccps2 projects
  (recvx/sssv/decompedia) document NO trick and treat these as non-matches. DISPOSITION: leave
  as INCLUDE_ASM stub (already byte-identical). jtbl_prep.py + jtbl-aware m2c still DECODE them
  to readable `switch` C — keep that for the NATIVE PORT / documentation, NOT the matching src.
  DO NOT attempt to match a jr-table dispatcher; recognize the pattern and park immediately.

## Report back (final message = your return value, NOT shown to a human)
- MATCHED (100%): each `func_XXXXXXXX` + one-line C description. Leave those as real C.
- PARKED: each `func_XXXXXXXX` + best %% + one-line wall/reason. Files restored.
- Confirm: every non-100% file restored via git checkout; you never wrote canonical
  build/obj or build/expected; you used only build/agent_<AGENT>/.

## WALL-CLASS IDIOMS (s83 RE — each unlocks a CLASS; HIGH confidence)
- **FP odd/even register coloring**: a single-precision producer (cvt.s.w / lwc1 / mtc1-reinterpret)
  colors its dest to the EVEN reg (f0/f2) in isolation, but to the ODD companion (f1/f3 — the target's
  form) when it is the FIRST/longer-lived operand of a float binop whose 2nd operand is ALSO a
  materialized float. Idiom: chain the conversion into a binop, e.g. `float a=(float)(u8)G; a=a+a;
  return a/256.0f;` → the conversion, the self-add and the divide all accumulate in the ODD f1
  while the constant stays in f0. CONSTRAINT: only ONE float
  constant may be live at a time — two simultaneously-live consts co-hoist into f0/f1 and bump the
  accumulator back to EVEN f2 (separate the 2nd const by control flow / a call).
- **2nd-float-arg f13 (NOT a wall — prototype hygiene)**: mwcc passes single-`float` args in
  f12,f13,f14… by single-float-param COUNT and declaration order (ints and `double`s don't consume
  FPU slots). The "2nd float lands in f14" near-miss is ALWAYS a bad callee prototype: K&R/unprototyped
  extern promotes floats to double→GPR (dsll32), and a stray `double` or wrong arity shifts the count.
  FIX: declare the callee with every FPU arg as single `float`, exact arity, no K&R. Unlocks the
  anim_clip_init / func_001CFB50-caller family.
- **switch branch-likely / case-chain direction**: mwcc already emits beql for switch cases; it tests
  the compare-chain in the REVERSE of source case order (first test = plain beq w/ filled slot, rest =
  beql). Write cases in ASCENDING source order so mwcc tests DESCENDING (matches CW). Use a REAL switch,
  not if-else-if (which emits bne/bnel in source order — wrong polarity). Per-case beql-vs-beq still
  follows idiom-13 (beql only when the case body's first instr is a speculatable pure-ALU op).
- **saved-register allocation ORDER**: raw params used across a call → mwcc assigns saved regs TOP-DOWN
  in decl order (1st param→HIGHEST s-reg: 3 params a0=s2,a1=s1,a2=s0). Copy each param into a SURVIVING
  local (used in arithmetic so it isn't coalesced) → BOTTOM-UP (a0=s0,a1=s1,a2=s2). Flip direction to
  match the target: target 1st-param-in-low-s → local-copy form; 1st-param-in-high-s → raw-param form.

## RESIDUAL-WALL IDIOMS (s83 RE round 2 — conditional levers)
- **Float early-return-0 / two-exit epilogue (CRACKS "branch-lowering, no C lever")**: write a float
  early `return 0` as the FALL-THROUGH after a POSITIVE-condition if-block: `if (x > K) { work;
  return 1; } return 0;` (negate the test into the if-TRUE arm). mwcc then places the return-0
  zeroing of v0 directly in the `bc1t` delay slot, landing on the shared epilogue —
  the target form. The `if (x <= K) return 0;` form instead gives `bc1f` + an extra `b` + shared
  paddub (the near-miss). Duplicate `return 0` at EACH nesting level to reproduce CW's two-exit /
  duplicated return-address-restore epilogue. (Float-domain analog of the int work-as-if-TRUE-arm idiom.)
- **idiom-13 nop, RMW subset**: the conditional-branch delay-slot NOP is reproducible at zero regalloc
  cost ONLY when the success block's first instr is non-speculatable. A read-modify-write field write
  (`p->f |= 1` → load byte, OR, store byte) leads with the byte load, so the branch keeps its NOP slot and the RMW follows. So this matches
  ONLY when the target's success block is genuinely RMW. For a CLEAN constant store (`p->f = 1` →
  load-immediate, then the byte store) mwcc always fills the slot with the load-immediate — GENUINE WALL (the `int one=1;` hoist makes the
  nop but burns a saved reg + grows the frame; only use it if the target pays that same cost).
- **$at-vs-GPR compare, stored-boolean subset**: mwcc keeps a branch-feeding compare in a NAMED GPR
  (set-less-than into vN, then branch on vN) only when the boolean is ALSO stored to memory:
  `c = v < N; p->flag = c; if (c) {...}` → the compare lands in v1, the branch tests v1, and the
  store of v1 fills the delay slot. For a PURE branch (boolean never stored)
  there is NO lever — mwcc always uses $at. Secondary: cache an unsigned-byte field into an int
  before comparing (`int v=(u8)x; if (v < K)`) to flip mwcc's `sltiu` to the target's signed `slti`.

## RESIDUAL-WALL IDIOMS (s83 RE round 3 — 3 more cracked)
- **Field-address CSE hoist (anim state-machine family)**: a POINTER-TYPED LOCAL aliasing a field
  (`u8 *q = &p->f6; ... *q = *q+1;`) makes mwcc hoist `&p->f6` into a saved reg + keep the base
  separately = extra saved reg + bigger frame (the wall). CURE: access `p->field` DIRECTLY (member/
  index off the single base pointer) in every read AND write — mwcc copies the BASE into the saved
  reg (a byte-add-with-zero register move) and recomputes each field offset from that saved reg inline (target form, smaller frame). An INT
  value-local (`int s=p->st; ...; p->st=s+1;`) is SAFE (does not hoist) — only a pointer alias does.
- **jal-delay-slot saved-reg copy (cracks "saved-reg-arg-in-jal-delay-slot")**: mwcc fills a call's
  delay slot with the LAST callee-saved param copy when (a) 2+ params live across the call, (b) the
  call forwards the ORIGINAL aN (not the saved copy), and (c) the call RESULT is DISCARDED/STORED
  (not kept in a saved reg — keeping it makes the s0 `sq` spill fill the slot instead). saved-reg
  NUMBER = first-use-after-call order; before-jal copy order is locked ascending-source-param (a
  residual if the target's order differs, e.g. func_001B1020 a3-then-a0).
- **Address-escaped loop counter (cracks counter inc/store/reload/compare scheduling)**: when the
  target reloads the counter from the stack each iter (load, increment, store, reload, compare, branch), make the counter
  memory-pinned by letting its ADDRESS ESCAPE once (`int i; h(&i);`). Use `do{ body; i++; }while(i<K);`
  (matches a fall-through zero-store init of the stack slot; a for/while top-test instead emits a leading `b`).
  A trailing body pointer-advance (`p+=K;`) lands in the bnez DELAY SLOT. `i<K` signed -> slti, unsigned
  -> sltiu. NON-lever: a struct-FIELD counter with a NOP branch slot (func_0014BB10) is idiom-13 — park.

## FP-LOAD-COLORING IDIOM (s83 RE round 4) — operand position
- **FP odd-companion coloring on a plain lwc1 LOAD = OPERAND POSITION** (generalizes idiom-1 beyond
  cvt.s.w producers). The LEFT (first) operand of a single-precision binop (subtract, add or multiply) whose
  result targets f12 colors to the ODD companion f1 (f3 for a nested pair); the RIGHT operand -> EVEN
  f0. So to match a target that loads p's field into f1, q's field into f0 and computes f12 = f1 - f0,
  put the value that must land in f1 on the LEFT of the binop: `fabsf(p->field - q->field)`. No materialized const / 2nd-float needed.
  Holds even when the loaded value is the syntactic right operand (rule is positional in the EMITTED
  binop). The li-hoist-above-store wall (func_00179680/001CB950) is CONFIRMED GENUINE — mwcc can't
  hoist a `li` past an unrelated store while keeping it in the same caller-saved reg (mutually
  exclusive live-range constraints); those stay asm-void.

## MWCC POST-RA SCHEDULER MODEL + idioms 12-18 (s83 RE — comprehensive)
### Conditional-branch delay-slot fill (refines/partly de-walls idiom-13)
mwcc fills a cond-branch (any of the equal, not-equal, zero, nonzero, FP-condition, <= 0 or > 0 forms) delay slot with the FIRST
EMITTED instr of a successor IFF it is a speculatable pure-ALU/immediate op (li, lui, addiu,
addu/subu, sll/sra/srl, mtc1-feeder, global/float-const address lui). LOADS (word, byte or FPU loads),
STORES (word, byte, halfword or FPU stores, including a store of the zero register), and CALLS (jal) are NEVER speculated -> slot stays NOP.
The decider is the FIRST EMITTED instr (after value materialization), not the C source order.
- **idiom-14 (store-of-LIVE-value = nop, the big de-wall)**: a clean store whose VALUE is already
  live needs no materializer, so the STORE is emitted first -> NOP at zero cost. `p->f = 0`,
  `p->f = <live param/saved reg>`, or `p->f = <precomputed local>` all give the branch, an empty NOP slot, then the store.
  Only a store whose value must be MATERIALIZED (`p->f = 3` -> load-immediate, then the byte store) lets the load-immediate fill the slot
  = GENUINE wall. Also: if the success block genuinely begins with a load/store/call, write THAT
  memory op as the FIRST if-body statement -> nop (multi-stmt blocks fine; const stores emit after).
- Unconditional `b`/`j` ASYMMETRY: a `b`/`j` slot CAN take a STORE (and arg addiu). So a store in a
  b/j delay slot is normal+matchable; a store in a beqz/bc1f slot never happens.
- mwcc does NOT duplicate the slot filler at the target label, and NEVER emits an 8-byte
  branch-target alignment nop (that CW-only nop = genuine wall). The filler may come from either
  successor (whichever leads with a speculatable op); branch polarity is irrelevant.
- GENUINE (no lever): clean-CONSTANT-store idiom-13 (materialized value, no leading mem op);
  global-store-first still fills via its %hi load.

### List-scheduling / instruction order
- **idiom-15 (reload-vs-CSE across a call)**: a call is an aliasing barrier — a field read placed
  textually AFTER a call is never CSE'd with one before it. To match a reload-per-call target, keep
  the BASE ptr in a saved reg and re-read `base->field` AFTER each call. To match a kept value, read
  the field into a surviving local BEFORE the call (mwcc loads the value into a saved reg, keeps it).
- **idiom-16 (const-in-delay-slot)**: write a const store / const call-arg as the LAST statement
  before a call -> mwcc sinks the store/li into the j/jal delay slot and hoists the value
  materialization (load-immediate, upper-half load, or upper-half load plus OR-immediate, width-independent) to the earliest free slot. The address
  half of a global store (its %hi) is formed per-store, not hoisted. For multi-const call args,
  the LAST arg's load-immediate or add-immediate fills the jal slot; earlier ones materialize before in arg order.
- **idiom-17 (paddub timing)**: with >=2 params live across a call, exactly ONE saved-reg param copy
  (argument register into saved register) fills the following branch/jal delay slot, the rest hoist before it; saved-reg
  NUMBER follows first-use-after-call order. A single param forwarded as a tail-call arg needs no paddub.
- **idiom-18 (s64-param direct store)**: declare a 64-bit-stored param `long long` to get a direct 64-bit store of the
  saved register with no 32-bit shift-pair sign-extension (an `int` widened to 64 adds the spurious extend).
- **idiom-19 (inverse-CSE / anti-frame-growth)**: when the TARGET recomputes a repeated subexpression
  at each use (e.g. two identical `s6 - s3` subtractions) but mwcc CSEs it into a callee-saved reg —
  adding a save and GROWING the frame (the classic `0x90 -> 0xa0` tell) — do NOT share a temp: INLINE
  the expression literally at every use site, including inside the branch CONDITION that guards it.
  mwcc then recomputes it per-path, matching the target's separate emissions and keeping the frame
  small. Pairs with the no-prototype call trick (`int f();`) when the arms pass a varying arg count /
  carry a leftover param. (Cracked func_00203F40 s84: inlined `(a1-t1)` into its `if` condition + both
  use sites; was 99.87% with the shared temp, 100.0 inlined.)
- **idiom-20 (BRANCH-LIKELY dispatch — mwcc DOES emit the equal, not-equal and equal-zero likely forms; NOT a wall)**: the s84
  "mwcc cannot generate branch-likely from C" parks were WRONG — 73 matched mwcc funcs contain
  branch-likely (e.g. src/func_0014E4F0.c switch dispatch whose likely slot computes `st + 1`;
  src/func_00154F00.c loop whose likely slot increments the counter; src/func_001AF7C0.c early-return
  `if(g>0){...}return 0;` -> a likely branch-if-<=0 whose slot zeroes the return value). mwcc emits a LIKELY branch (nullifies its
  delay slot when NOT taken) whenever it can fill that slot with a SPECULATABLE pure-ALU op from the
  TAKEN (branch-target) path. The C must make that op the FIRST emitted op of the taken path:
  - SWITCH state-machine: `switch(st)` on a LOCAL `st = *(unsigned char*)(e+6)`; case labels ASCENDING
    0,1,2,… (mwcc reverses to descending compare chain 2,1,0 = CW). Write the state advance as
    `*(unsigned char*)(e+6) = st + 1;` USING THE SWITCH LOCAL `st` (NOT `(*(e+6))++`, NOT
    `*(e+6)=*(e+6)+1` which RELOADS) -> mwcc drops the `st + 1` into the dispatch branch slot via
    beql/beqzl. Keep `st` live. (Verified synthetically + on matched siblings.)
  - EARLY-RETURN GUARD: `if(cond){body} return X;` with X simple -> `<inv-cond>l epilogue` + X-setup slot.
  - LOOP counter: the `i++` fills the loop-test likely-branch slot.
  - PRINCIPLE: get the body byte-identical; if the SOLE residual is "target beql/beqzl where mine has
    plain beq/bne (+nop)", read the target .s to see which op sits in the likely slot, then make THAT op
    the natural first statement of the taken path. DEFAULT: branch-likely IS matchable — don't park it.
- **idiom-21 (FLOAT COMPOUND-ASSIGN picks the add.s operand order)**: `x = x + y` makes mwcc load the
  LHS first into `$f2` and add with `$f2` as the first source; `x += y` makes it load the RHS first
  into `$f1` and add with the `$f0` accumulator first and `$f1` second — the CW form. Whenever the sole residual on a float accumulate is the
  add.s operand order (or its FP register numbering), flip the statement between the two spellings.
  (Cracked func_0016D130 s85: 9 accumulate sites converted to `+=`, 99.87% -> 100.0.) Corollary
  confirmed on the same func: mwcc lowers `switch` to a DESCENDING beq chain, so an ASCENDING
  `bnez/bne` if-else-if chain in the target must be written as if/else-if, NOT a switch (93.44 ->
  97.62); and the `-sdatathreshold N` recorded in the stub header must actually be passed on the
  CFLAGS line for the `%gp_rel` refs to resolve the same way (97.62 -> 98.54).
- **idiom-22 (VOLATILE SCRATCHPAD preserves access order)**: the original treats the 0x7000xxxx
  scratchpad as volatile — accesses stay in source order and are never CSE'd or reordered across each
  other. When a near-miss's residual is scratchpad load/store ORDER (e.g. a 4-word copy block emitted
  in the wrong sequence, or a load sinking past following stores), declare those accesses
  `*(volatile int *)0x700038A0` / `*(volatile float *)…`. (Found s85 on func_00144040: 94.6 -> 96.8;
  recommended for the whole parked scratchpad-copy sibling family — func_0013F240, func_0014A0D0,
  func_00143AF0, func_00135870.)
- **idiom-23 (INLINE ZERO-TEMP — float compare against 0.0f)**: mwcc 2.3.3 chooses BOTH which FPR
  holds the loaded value vs. the zero AND which operand lands in `fs` of the `c.eq.s`, and the two
  are separately steerable. Measured on a micro-testbed at `-O4,p`:
  - `x != 0.0f` / `0.0f != x` / `!(x == 0.0f)` / `x == 0.0f` → zero is moved into `$f1`, the
    value is loaded into `$f0`, and the compare takes `$f1` first (zero is `fs`).
  - `float z; z = 0.0f; ... x != z` (zero assigned in a SEPARATE statement) → zero goes into `$f0`
    and the loaded value moves to the second FPR, `$f1`.
  - `x != (z = 0.0f)` (assignment INLINE in the condition) → flips both dimensions, giving the
    CodeWarrior form: load into `$f0` first, then zero into `$f1`, compare `$f0` first.
  Reach for this whenever the sole residual is an `mtc1`/`lwc1` ordering or a `c.eq.s` operand swap
  around a compare with zero. (Cracked func_0021F330 s85 — analytically, after ~thousands of
  permuter iterations had failed on the same function; the permuter cannot reach it because mwcc
  folds the temp back before scheduling.)
- **idiom-24 (FP-ARG-ZERO-STAGING — the f13-before-f12 wall, CRACKED)**: when the target emits the
  trailing `0.0f` argument's move of zero into `$f13` BEFORE the move into `$f12`, stage the zero through an
  **int converted to float**: `int zi = 0; float z = (float)zi; f(self, clip, 5.0f, z);`
  The int→float CAST survives as a real IR node and gets scheduled ahead of the constant
  materialization; a plain `float z = 0.0f;` is const-folded straight back into the call and does
  NOT work (measured 99.826% — this is the obvious variant that made prior agents declare the class
  unmatchable). Placement is forgiving: before the guarding `if`, at the top of the case, or inside
  the arm all reach 100.0. (Cracked func_00137C80 and func_0014D5F0 s85, both of whose NEARMISS
  headers had asserted "no source change fixes here" — those assertions were wrong.)
  **This is a class lever**, not a one-off: the same f13-then-f12 residual is recorded on
  func_0012D580, func_00131B10, func_0013B5B0, func_0013DD40, func_001424C0, func_001437E0,
  func_00147960, func_00148520, func_00149B50, func_0014A350, func_0014D7C0, func_0017F130,
  func_001F6640 and func_0017E7C0. Try this before spending any permuter time on them.
- **idiom-25 (FLOAT TRUTHINESS — steers c.eq.s operand order without touching FP coloring)**:
  `if (x)` / `if (!x)` on a float lvalue puts zero in `$f0` and compares `$f1` first (the VALUE as `fs`).
  Every explicit spelling — `x != 0.0f`, `0.0f != x`, `!(x == 0.0f)`, `x != 0`, `x != (float)0`, and
  a hoisted `float fv = x; fv != 0.0f` — instead compares `$f0` first (zero as `fs`). Note this is
  a *different* dimension from idiom-23: idiom-23's `x != (z = 0.0f)` moves the compare-operand order
  but also swaps the FPRs, trading one two-instruction diff for another; truthiness moves the operand
  order alone. (Cracked func_002236F0 s85 via a 49-cell variant sweep.)
- **idiom-26 (COMPOUND-ASSIGN steers add.s operand order)**: `*p += -0.2f;` adds with the loaded
  value (`$f1`) as `fs`; the expanded `*p = *p + -0.2f;` and `*p = -0.2f + *p;` both put `$f0`
  first and `$f1` second. A `float cv = -0.2f;` temp also gives the correct order. Beware
  `*p = *p - 0.2f;` — that emits `sub.s` and is a different instruction. (Generalizes idiom-21.)
- **idiom-27 (BREAK-NOT-RETURN — stops delay-slot speculation in switch dispatchers)**: in a switch
  state machine where the switch is the LAST thing in the function, terminate each case with
  `break;` rather than `return;`. It is a pure CFG-join change (semantically identical when no code
  follows the switch) but it stops mwcc 2.3.3 speculating constant materializations into
  conditional-branch delay slots, restoring the target's unfilled NOPs. On func_0012D580 it fixed
  two separate residuals at once — the switch-dispatch slot AND one three levels deep inside a case
  — for 96.84% → 98.50% in a single edit. **Precondition:** nothing after the switch, or `break`
  and `return` are not equivalent. Related: a shared `goto done;` trailing-label exit has the same
  effect and is what made func_00131B10 match (plain `return;` there scored 95.4%).
- **NOTE on idiom-24's scope**: it applies only when the residual really is the f13/f12 arg-emit
  order. On func_0012D580 the same headline symptom turned out to be four different residuals and
  zero-staging produced a byte-identical object. Confirm the actual per-instruction diff before
  reaching for it.
- **idiom-28 (STRICT-RELATIONAL RESPELL — steers the compare temp between `$at` and a GPR)**: when
  the sole residual is a `slt`/`slti` whose destination is `$at` in the target but a GPR in yours
  (or vice versa), respell the relational in its strict form against the adjacent constant —
  `x > K-1` for `x >= K`, and the reverse. Semantically identical for integers, but it flips mwcc's
  compare-temp allocation. Measured on func_00164220's case-2 guard: `>= 2` → 99.897, `> 1` →
  **100.0**, `2 <= x` → 99.897, `!(x < 2)` → 99.897, hoisted int temp → 99.897, unsigned `>= 2u` →
  99.330, `x - 2 >= 0` → 98.763. Do NOT reach this by swapping the if/else arms — that reorders the
  whole case (92.35%, 100 vs 97 instructions).
- **CHECK THE EXTERN PROTOTYPES BEFORE BLAMING CODEGEN.** A wrong `extern` declaration in a parked
  file produces a residual that *looks* like an FP-arg-order or coloring artifact and will survive
  every idiom and any amount of permuting. func_00164220 sat at 99.588% because its file declared
  `func_001749A0(float, int, int)` when the real signature — confirmed against five already-matched
  callers — is `(char *self, int code, int flags, float blend)`. Fixing the prototype took it
  straight to 99.897%, and one idiom-28 respell finished it. **Verify every extern against an
  already-byte-matched caller first**; the corpus is the authority, not the parked file's own header.
- GENUINE (no lever): LI-HOIST — a plain-literal `li`/`lui` hoisted above an unrelated store while
  kept in the immediate-scratch $v1. $v1 is rewritten per literal; a value living across a store gets
  a DISTINCT value reg (a0/a1), so "hoisted order" and "$v1 coloring" are mutually exclusive. (A
  surviving const-local reproduces the ORDER but colors to a1 = the func_00179680/001CB950 residual.)

### idioms 12-13
- **idiom-12 (FP-param companion pairing = positional)**: NOT a wall. N single-`float` params live
  across a call are saved TOP-DOWN by decl order: param k (incoming f(12+k)) -> f(20 + (N-1) - k).
  N=2 -> f12:f21(odd), f13:f20(even). Just write natural code in true param order; inline `0.0f`
  literal in a compare for compare operands f0-then-f21 (a `float zero` local reverses it).
- **idiom-13b (dead-const re-materialization)**: when mwcc speculates a const into a cond-branch
  delay slot it ALSO re-emits it DEAD at the target label (no cross-branch CSE of the const). Force
  it with: an if/else-if chain where every arm passes the SAME large two-half (upper/lower immediate) const to a call (dead
  lui); or two stores of the SAME nonzero const straddling a branch whose delay slot has NO real
  store available (dead li). A store schedulable into the branch slot OUTRANKS const speculation.

### idiom-29 (strength-reduced multiply: fresh vs in-place shift destination) — s86
- **The wall**: for the final shift of a strength-reduced multiply, the original allocates a
  **FRESH** register and adds into the other one
  (`FRESH = src << k; FRESH >>= 15; src = FRESH + B`), while mwcc 991202/2.3.3/2.4
  all reuse the dying source register **in place**. Found on the random-scaling idiom
  `(func_00122BB8() >> 16) * K >> 15`, which had **no matched exemplar anywhere in the corpus** —
  all seven users of it were parked as near-misses.
- **NOT a compiler or flag wall.** It is a source spelling. Give the value a home register by
  writing the multiply and the shift as compound assignments to the SAME variable:

      int x = func_00122BB8() >> 16;
      x *= K;
      x >>= 15;
      ... = x + B;                 /* the + B must be a SEPARATE expression */

  `x = x * K; x = x >> 15;` (plain assignments, same variable) works identically, as does landing
  the add in its own variable (`y = x + B`).
- **What does NOT work**: folding the multiply into the initialiser (`int x = r * K; x >>= 15;`)
  stays in-place, and so does splitting the stages across different variables. ~50 respellings and
  ~90 context probes were tried before this one — do not re-derive them.
- **Fold composite scales into one multiplier** when applying it: `((v >> 16) * 5 << 4) >> 15`
  becomes `x *= 0x50`.
- Cracked the whole family at once: func_001549C0 64.14 -> **100.00**, func_001DE920 69.43 ->
  98.82, func_001D0720 90.72 -> 97.26, func_001C1A80 97.83 -> 98.74, func_00128C10 97.96 -> 98.48,
  func_001551B0 97.41 -> 97.92, func_001BF6B0 99.94 -> 99.99.
- **Method note**: the permuter did NOT find this despite it being pure register colouring; the
  source-spelling lens did, with corpus mining supplying the colouring model. When an idiom has no
  matched exemplar, respelling beats searching.

### idiom-30 (rand-in-float-argument delay slot) — s86
- Hoisting a call out of a float argument expression — `{ float fr = (float)func_00122BB8();
  ... 1.0f + 19.0f * (4.656613e-10f * fr); }` — reproduces the target's `nop` in the `mtc1`/`mul.s`
  sequence, where the inline form fills the slot. Worth +0.66% on func_001C1A80.


### idiom-31 (nonzero integer-to-float argument staging) — 2026-09-22

Generalize idiom-24 beyond zero. A block-local integer converted to float can
restore the original argument-materialization order even when ordinary float
locals and assignment expressions leave the residual unchanged:

```c
int amplitude = 20;
float value = (float)amplitude;
func_0021B9A0(3, 1.0f, value);
```

`func_001EA240` needs this only at the second light-group call: 99.97924% ->
100%, zero raw instruction differences across 2312 bytes. Staging the first
argument or both arguments did not work. The compiler folds the conversion;
its effect on scheduling is inferred from emitted code, not inspected IR.

Scaled constants also work when their bits are unchanged: `func_0012CAA0`
stages `int degrees = 4; float rate = (float)degrees * 0.0174532925f;` for the
trailing steering-rate argument (99.97131% -> 100%, 1952 bytes). The resulting
binary32 bits are 0x3d8efa35, exactly the prior 0.06981317f literal.

`func_00135D00` combines staging 235 and two zero arguments with the correct
`func_00102948(void *, void *)` prototype and removal of the unnecessary
float-pointer cast: 99.00741% -> 100%, 1080 bytes. The prototype/cast cleanup
was measured together; do not attribute its effect to one change alone. This
function remains NEARMISS: its 36-byte local jump table triggers LOCALDATA_FORCED,
so a passing ELF currently proves assembly fallback, not compiled-C placement.

A separate prototype fix promotes `func_00180850`: forwarding the actor
address to `func_00180420(int)` resolves the saved-parameter move order
(99.76471% -> 100%, 340 bytes). Read the callee before declaring a compiler wall.

Always preserve exact float bits and use the full linked-ELF gate. Objdiff and
raw instruction checks alone cannot verify relocation targets.

Before claiming a new ELF-verified match, confirm `fill_unmatched.py` actually
selects its compiled object. Check the GPREL/SIZE_DRIFT force lists and allocated
local data. `func_00180850` needed removal from the stale GPREL_FORCE_ASM list;
`func_00135D00` stayed parked until its local table was pinned (it now links
from C; jr-table lane). A passing ELF while
the linker substitutes assembly is not evidence about the candidate C.


### idiom-32 (scratchpad literals as relocated externs, per address) — 2026-09-22

Many parked functions have one residual: the scratchpad upper-half load (0x7000) sits in a delay slot
that the target leaves as `nop`. This is often a conditional-branch slot or the
unconditional `b` of a switch default or join. The source spells a scratchpad
global as a literal, such as `*(unsigned char *)0x70003B92`. mwcc treats that
address as a constant, so it can CSE the `lui` and speculate it into a slot.
`tools/match/spad_symbolize.py` explains why the original likely referenced these
globals as externs with `%hi/%lo` relocations. It was applied only to 0x70003B6C
and 0x70003B8D.

The same fix works for other scratchpad addresses. Write the access as an
extern of the pointed-to type. Use a scalar extern (`extern unsigned char
D_70003B92;`) under `-sdatathreshold 0`. Under a nonzero threshold, use an
over-declared array (`extern unsigned char D_70003B92[16];` with `[0]`, idiom
#20), so the symbol does not become gp-relative. link.py defines any
`extern D_XXXXXXXX` in src automatically, so the linker needs no change.
objdiff needs the address in `build.py` `_SPAD_SYMS`, so the expected object
gets the same relocation. Without it, the file stays at 99.7-99.99%. The only
differences are literal-versus-relocation operands, and the linked bytes are
the same.

Measured with a private `_SPAD_SYMS` override (m1-firstlevel-matching lane):
twelve NEARMISS files reach 100.0%. The override applies only to files whose C
names the symbol. Those files are 0015AE20, 00182DF0, 001AFD70, 001A8840,
001607D0, 0017C440, 00191580, 001FC3C0, 001A8BE0 and 001AE6B0 (safe addresses),
plus 0019B7D0, 001C5C90 and 0012D580 (unsafe addresses, below).
func_001ACEC0 needed only 0x70003B6C and was promoted.

The address list is global, so check every compiled file that already names
`D_<addr>` before extending it. Measured collateral:

| address | effect on already-compiled files |
|---|---|
| 0x700031D4, 0x70003B7C, 0x70003B7E | fixes func_00198240, func_00223C70 (99.98 -> 100) |
| 0x70003B8A | fixes func_00138C20 (99.99 -> 100); func_001BF6B0 99.993 -> 99.922 (already below 100) |
| 0x700031D0 | fixes func_0013D220, func_0018A1F0; **breaks func_001787B0 (100 -> 99.88)** |
| 0x70003600, 0x7000360C, 0x70003610 | **breaks about 20 matched files**; their C names the symbol for `&` arguments but stores through literals |
| 0x700031D8, 0x70003A20/24, 0x70003B68/74/76/78/84/86/8F/91/92 | no compiled file names them; safe |

Measure each address against this collateral before landing it. For the
0x700036xx family, gate symbolization per file instead of globally.

Related findings from the same lane:
- **Element accesses of symbolized scratchpad arrays** (func_0018CE60) are a
  different case. There, `D_700039C0[1]` is a relocation against
  D_700039C0+4, but the expected object has a literal `0x39C4(at)`. Rewriting
  the accesses as literals breaks the load hoisting (-4%). This needs a
  normalizer that renders an in-range literal as `%hi(sym+off)`.
- **A callee that ignores an argument does not prove the caller passed none.**
  func_0020CDA0 is byte-matched as `(void)`, yet func_0020B210's target loads
  the value into $a0 before each call. Changing the caller to `()` dropped
  99.70 -> 99.33. For func_0021BD10 `(void)` and func_001FBC50 /
  func_001FF080 / func_001AEE10, the target did not set those registers, so
  the `(void)` / fewer-argument form is supported. Check the call site in the
  target before changing arity.
- **An unset argument register can come from the original.** func_0021D800
  never sets $a0 before its two func_001FBD50 sound calls. func_001A8970 reads
  callee-saved $f20 without writing it. Keep these as documented quirks. An
  uninitialized C local does not reproduce them (mwcc gives it a saved
  register).
- **Tentative definitions**: `unsigned char D_00275BD8;` without `extern`
  emits a local .sbss object. fill_unmatched's local-data guard does not check
  .sbss. Always declare globals `extern` (func_00225AC0).

### Per-file scratchpad opt-in: `// SPAD:` / `// NOSPAD:` (m2-matching lane) — 2026-09-23

`_symbolize_scratchpad` in `tools/decomp/build.py` now reads two directives
from a file's leading comment block, next to `// COMPILER:` and `// CFLAGS:`:

    // SPAD: 0x700031D0 0x700031D8     also symbolize these for this file
    // NOSPAD: 0x70003B8A              never symbolize these for this file

Use `// SPAD:` for an address that cannot go in the global `_SPAD_SYMS`
because another matched file names the symbol but still stores through
literals (0x700031D0 breaks func_001787B0; 0x700036xx breaks about 20 files).
The file's C must still name `D_<addr>` in code. A malformed address is a
hard error. The predicate now ignores comments. Before, a comment that quoted
`D_70003B8A` opted func_001BF6B0 in, and the file measured 99.929 instead of
99.993.

Applied in this lane: func_0019B7D0 (0x700031D0/D8), func_001C5C90
(0x70003600/0C), func_0012D580 (0x70003610), func_0013D220 and
func_0018A1F0 (0x700031D0; both 100% now), func_001AD010 (0x70003B93),
func_001A9000, func_001AA140 and func_001A9B10 (0x70003B88), func_00184BA0
(0x70003B98).

### idiom-33 (switch vs if/else controls the `b` delay slot and compare order) — m2-matching

- **A switch `break` leaves the delay slot of its unconditional branch to the join as nop.** An if/else join lets
  mwcc retarget that branch past the join's first instruction and copy that instruction
  into the slot. If the target branches to the join with a NOP slot where yours branches one
  instruction past it with the join's first instruction in the slot, write that tail as a `switch` (func_0018D7B0).
- **A sparse switch compares in REVERSE label order** and ends with
  an unconditional branch to the default with a NOP slot. Case bodies stay in label order and fall through. Target
  compares 2, 1, 4 means labels are written `case 4: case 1: ... case 2:`
  (func_001AB6A0). A one-case `switch (x) { case 7: ... }` gives
  a branch-if-equal to the case followed by an unconditional branch past it, where `if (x == 7)` gives a single branch-if-not-equal (func_001A9B10).
- **A long compare-per-constant chain over a type byte is a sparse switch, not an if/else
  cascade** (2026-10-16). One branch-if-equal per constant in descending order, each case's first
  value in its delay slot, bodies laid out in ascending order: write `case` labels ascending, a
  shared body under stacked labels, `default` last. func_001BA580 went 52% -> 100% and
  func_001BA8E0 70.6% -> 100% on the first try. An if/else chain merges adjacent constants
  into range tests, and switching on an `unsigned char` parameter adds a byte mask before
  the chain; switch on an `int` when the target compares the register unmasked.
- **`a == K1 || a == K2` with an if/else** lowers to a branch-if-equal on K1 to THEN and
  a branch-if-not-equal on K2 to ELSE, with both slots filled from the successors. It also leaves
  the ELSE block's first instruction as DEAD code after THEN's unconditional branch to the join. If the
  target shows that branch, an empty NOP slot, then an unreachable instruction,
  that is this shape (func_00188ED0, func_0015BCF0, func_001A8660). Adjacent
  constants (3 || 4) are merged into a range test, which the target does not
  do. Compare `(unsigned char)d` on an int `d` to keep two compares. The cast
  also produces the target's in-place mask-to-byte (AND with 0xff) of the value.
- **`if (v >= K || v == K2) return;` before a tail block** gives a skip branch on the
  range test, a branch-if-not-equal on K2 into the block, and an unconditional branch to
  the end with a NOP slot (func_00141C90). func_00152EB0's `if (v == K1 || v == K2)
  return;` is the equality-pair variant of the same early-return shape. An `if (v < K) { if (v != K2) ... }`
  nest gives a single inverted branch and is shorter.
- **Bare `case N: break;` labels change a sparse switch's chain.** Values that fall to
  the code after the switch must still be listed when the target compares them
  (func_0013D2D0: compares 4, 3, 2, 1, 0; cases 0 and 4 are bare breaks).
- **Ternary min/max** `a = (a < b) ? a : b` gives a float-false branch-likely with a float move in its likely slot,
  then an unconditional branch to the join with a NOP slot, then a dead float move (func_0015BF90). `if (a >= b) a = b` gives
  `bc1t`.

### idiom-34 (small levers found in the same round) — m2-matching

- **`idx << 2` instead of `idx * 4`** in hand-written address arithmetic
  reverses the `addu` operands (`v0 + base` vs `base + v0`)
  (func_001E2560).
- **Read a global before an unrelated guard** to get its load into the guard's
  own delay slot. mwcc never speculates a load from a successor, but it moves
  one from the branch's own block (func_001A8DA0: `n = count; q = base;
  if (n == 0) return;`).
- **Size stack objects by what the callee writes.** A frame 0x20 smaller than
  the target is usually an undersized local. func_001E2560's projectile block
  is 0x58 bytes (func_001CFA60 writes +0x40..+0x57), not 0x40.
- **GS register fields are 64-bit bitfields.** An unfolded and-immediate of zero
  with 0x1FF plus a 64-bit constant load next to a halfword load/and/or/store is
  `unsigned long long FBP : 9` style bitfield stores (func_001AB4E0,
  DISPFB). Declare the globals `volatile` if the target keeps the load after
  the previous store.
- **An argument the callee ignores can still be passed.** func_001AD010 keeps
  two bytes live in $a0/$a1 to func_001FBC50, which is byte-matched as `(void)`.
  An `unsigned char` two-argument prototype reproduces it; `int` adds `andi`.
- **A call with fewer arguments needs an unprototyped extern.**
  func_001AE5E0 calls func_001CB590 with four arguments and later with three,
  and the target leaves $a3 unset at the second call. `extern int f();` does
  this. A function-pointer cast becomes `jalr`.
- **Mixed volatility per scratchpad counter.** In nested countdown loops the
  target can reload one counter at every access (volatile) and cache the other
  (func_001AA140: 0x70003B86 volatile, 0x70003B88 not). Sweep both.
- **Literal-spelled scratchpad values are the most common "delay-slot wall"
  in the first-level NEARMISS set.** In this round, relocated externs fixed
  func_001AC480, func_001AD010, func_001A9000, func_001AA140, func_001A9B10,
  func_001A8DA0, func_001AE5E0 and func_001A8660. Under a nonzero
  `-sdatathreshold`, over-declare as an array (idiom #20).
- **`tools/match/spad_symbolize.py` bug:** when one file dereferences an
  address at a second type, it writes the value cast as `(T * *)D_x` instead
  of `(T *)D_x`, which does not compile. FIXED in the m3-matching round
  (see below); do not hand-fix new output.

### m3-matching round (first-level round 3) — 2026-09-23

- **spad_symbolize.py fixed.** A second-type use `*(T2 *)0xADDR` is a `T2`
  value, so it is now written `(T2)D_ADDR` (func_001AC480's old input gives
  `(unsigned short *)D_70003B6C[0]`, compiles, 97.33 -> 98.54%; the rest is
  the `(*pf)--` body fix). Only pointer-to-pointer rvalue uses are rewritten;
  a scalar of another width or a store/increment/address-of through the
  second type is left as the literal and reported as LEFTOVER. Unless the
  file's `// CFLAGS:` says `-sdatathreshold 0` explicitly, the symbol is
  over-declared `T D_ADDR[16]` and used as `D_ADDR[0]` (idiom #20).
- **mwcc's default `-sdatathreshold` is 8, not 0.** A file with no
  `// CFLAGS:` line (build.py's bare `-O4,p`) or with CFLAGS that omit the
  flag puts every object of 8 bytes or less in gp-relative small data.
  Measured on mwccmips 991202, mwccps2 2.3.3 and 2.4: extern char arrays of
  1/2/4/5/8 bytes got R_MIPS_GPREL16 and 9+ bytes got HI16/LO16, the same
  split as `-sdatathreshold 8`. A plain scalar extern is only %hi/%lo under
  an explicit `-sdatathreshold 0`. `spad_symbolize.py --selftest` covers
  this and the lvalue cases.
- **The EE byte-add with zero is a register move, not byte evidence.** mwcc uses
  it for any 64-bit-clean copy or zero (zeroing v0 this way returns an int
  0 in func_001B6E40). func_001B0C60 and func_0018A880 match with int or
  unsigned char parameters, so the width comes from callers; theirs pass
  constants and declare int, so both files now use int.
- **Volatile pins global store order.** mwcc moves a constant or zero-register
  store past neighbouring stores to other globals. Declaring the stored
  globals `volatile` keeps source order, which was the target's order in
  func_001B0C60 (77.6 -> 100), func_001AF5C0 (NEARMISS 81.55 -> 100; its
  "store-scheduling artifact" note was this) and func_002061B0 (98.25 -> 100,
  only the two gp-relative flags volatile; mwcc 2.3.3). Try it before calling a store
  permutation a wall. It did not fix func_001FC9B0 (90.48; the residual is
  one upper-half load into `$at` placed a store early).
- **Check the compiler before the source.** 85 ordinary-C units were
  force-listed in fill_unmatched at <100% under the default mwcc 991202.
  Recompiling their unchanged C found 29 exact matches: all 19 SDK-region
  ones (0x00101CC0..0x00120048) under `// COMPILER: eegcc` + `-O2`, and 10
  game functions under mwcc 2.3.3. Sweep compilers first on any old unit.
- **Force lists go stale.** 109 GPREL/SIZE_DRIFT entries covered ordinary
  C that was already objdiff 100%; removing them kept the boot ELF
  byte-identical (strip_sections now applies address-named GPREL16 fixups,
  so the original overflow no longer happens). Six gprel entries stay listed
  because their objects carry .rodata (local-data guard). Before listing a
  function, relink without the entry and check the ELF.
- **An old header is not proof.** func_001FD580's NEARMISS note said the
  instruction stream was 1:1 with the target, but the C read the entry id at
  +0. The target reads the signed halfword at +0x2 at 0x001FD654 and 0x001FD670. Diffing the
  mnemonic sequence (73 instructions against 70) showed the "register
  colouring only" diagnosis was wrong too. It is now 100% (fix round):
  - **Rotated scan loop.** The target branches straight to the loop test and
    computes the entry address only there. Write it as a `for` loop whose
    condition indexes the table. A pointer set before a `while` adds a copy.
  - **Unsigned parameter blocks a CSE.** The target recomputes `arg0 + n`
    after a call. With `int arg0`, mwcc 2.3.3 CSEs the loop's sum into a
    saved register (and `n + arg0` swaps the addu operands). `unsigned int
    arg0` keeps both the order and the recompute.
  - **Declaration-order sweep.** mwcc's register colouring follows local
    declaration order. Once the opcode stream matches, compile every
    permutation of the locals (720 for six, about 4 minutes in the
    container). Here 18 of 720 reached 100%.
  Diff the opcode sequence before calling a residual "colouring only".

### Local .rodata pinning (jump tables link from C) — 2026-09-23

**Mechanism.** A switch dispatcher or a function with a static const table
carries its own `.rodata`. The original keeps those bytes in the data region
(link units `func_00261544` and `func_00271DF8`), and the function's
%hi/%lo pair points there. link.py used to let `*(.rodata)` place compiled
tables after the whole image. That relocated the pair to the wrong address, so
fill_unmatched's local-data guard linked every such function from its .s.

Now `tools/decomp/rodata_pin.py` proves where each table belongs and link.py
places it there:

1. `plan_object()` reads each text HI16/LO16 pair against a `.rodata`
   section and the original instruction pair at the same offset. That gives
   the section's original base (original address minus the in-place addend).
   All pairs must agree.
2. The section's own relocations (jump-table `R_MIPS_32` entries against the
   function's `.text`) are resolved with the function's original vram. The
   result must equal the original bytes at the base. The base must honour the
   section alignment. With several `.rodata` sections, the linker lays them
   out in index order, each aligned. That layout must give every base, with
   only zero bytes in the gaps. The span must sit inside one data-region unit
   (a `dlabel` unit with no src/*.c, so it is always assembled).
3. fill_unmatched runs the planner before its worker pool. It copies a proven
   object instead of forcing it to the .s and writes `build/rodata_pins.json`.
   Unproven objects, and any object with `.data`/`.sdata`, keep the guard.
   The refusal reason prints in the fill log.
4. link.py proves every pin again against the object it is about to link
   (`build/filler/<f>.o`). It splits the data-region filler object into pieces
   around the pinned spans (`build/rodata_pin/<region>__lrodNN.o`: `.text`
   bytes, relocations and symbols rebased; the pieces keep the original
   length). The LCF lists `piece (.text)`, then `<func>.o (.rodata)`, then the
   next piece. Region symbols inside a carved span become absolute LCF
   definitions. After the link, `check_pinned_map()` confirms from the xMAP
   that each pinned `.rodata` is at its planned address. verify_all's boot-elf
   stage fails on that check as well as on the byte comparison.
5. audit_link_provenance counts pinned objects as `compiled_object_*`. It
   lists them in `pinned_local_rodata` and checks that the linked copy carries
   the compiled `.rodata` unchanged (`pinned_rodata_mismatches`). It maps the
   region pieces back to their slot for the link-order check.

**Result.** All 82 local-data-guarded functions and the six GPREL entries that
carried `.rodata` (func_00159210, func_00159620, func_001BC960,
func_001BDFC0, func_001BF6B0, func_00207350) now link from the compiled
object. The rebuilt boot ELF stays byte-identical and no local-data route is
left. `sub_image_buffer_needs_to_be_aligned` (ee-gcc) is the one explicit pin
(`EXPLICIT_SYMBOL_PINS`). Its `keep[]` table has no HI16/LO16 reference. Once
resolved, it equals the first 16 bytes of `jtbl_0026B650`, and the same
checks apply.

**For matching.** A jr-table dispatcher is no longer parked just because it
has a table. Once its C reaches objdiff 100% (the expected object already
carries the function's own table, `build/jtblrodata`), it links from C like
any other unit. The pin is proven at fill and link time, and fill prints a
reason if a table cannot be pinned. Run `rodata_pin.plan_object()` on a
scratch object to check a candidate before promotion.

### Round 4 on the first-level NEARMISS set (rodata-pin lane) — 2026-09-23

The scope was the 85 NEARMISS functions that the port's FIRST_LEVEL_AUDIT /
SCENE_COORDINATOR_DESIGN / ORIGINAL_FRAME_ORDER cite (17 are jr-table
dispatchers). They were worked from the highest similarity down. Eleven now
match byte for byte and link from C. Five of them are jr-table dispatchers,
which the local .rodata pin now places exactly (func_001B82D0, func_001A97B0,
func_001AD360, func_001B8FC0, func_001AEE70).

What worked, with the function that proved it:
- **Scratchpad symbolization (idiom-32), including addresses that never show
  in a diff row.** A literal-versus-relocated scratchpad access changes
  scheduling and colouring far from the access itself. func_001AD360's one
  store-order residual went away once 0x70003B6C was a relocated extern. Sweep
  every `0x7000xxxx` a file dereferences, alone, all together, and all-but-one:
  func_001B82D0, func_00188ED0, func_001A97B0, func_001AD360. func_0018C0D0
  needed all of them, 0x700038A0 included.
- **Cast away `volatile` only when the target needs it.** `(void *)&vol_float`
  in argument lists made mwcc schedule the other argument into the `jal` slot.
  Plain `&D_x` matched (func_001B8FC0).
- **Statement order inside a block.** Split declarations from initializers and
  put each computation where the target first uses it. func_001A8970 matched
  once `f21 = half + ...` moved after `d`, and func_0018C0D0 needed the same
  split. Our decl-order sweeper also permutes initialized locals this way.
- **Duplicated arms beat `||`.** A target with two copies of `mul.s` is
  `if (a) X; else if (b) X;`, not `if (a || b) X;` (func_0017ABA0). The
  reverse also happens: func_0018A6B0 wants `a || b` where the old C had an
  else-if.
- **int locals for re-narrowed bytes.** `unsigned char st2; band = st2 - 0x31`
  adds a mask-to-byte (AND with 0xff) that the target lacks, so declare `int st2`
  (func_0017ABA0).
- **`break`, not `return`, in switch cases.** With `return;` mwcc filled the
  compare-chain `beq` slots from the fall-through and turned some into
  `beql`. `break;` gives the target's unfilled `beq` slots (func_00187EE0, 90.0 -> 100).
  It also helped func_0018A6B0 (87.2 -> 95.6 before the other fixes).
- **Sparse switch label order (idiom-33).** func_0018A6B0's 0 -> 2
  fall-through put the labels out of order. Ascending labels plus a plain
  `break` restored the target's 3, 2, 1, 0 compare chain.
- **Write-pointer local.** func_001AEE70's a2/a3 swap was
  `char **w = (char **)(q + 0x14);` used for the four display-list stores.
  The permuter found it (score 10, not 0) and objdiff confirmed 100%. **Always
  objdiff-check permuter outputs; a non-zero permuter score can already be a
  match.**
- **Callee arity from the target's call site.** In the target,
  func_002160B0/func_002149F0 do not set `$a0` before func_0020CDA0, and
  func_0020CDA0 is `(void)`. Calling it with no argument moved func_002160B0
  from 99.22 to 99.98 (with `volatile` on D_002821B0/B4 and a decl swap).
  func_0018A6B0 passes `arg0`, which is still in `$a0`, to func_001AFC10.
- **Missing scratchpad stores are body bugs.** func_001936E0 stores the
  distance and angle to 0x70003A20/0x70003A24 and reloads the angle for the
  second trig call. The old C kept them in locals. With that fix plus switch
  label order and the if/else clamp, it went from 94.99 to 99.2. It is still a
  NEARMISS and was not re-filed; see below.

Measured but not landed (the source is unchanged; the best scratch variant
is in parentheses):
- func_00183EF0 99.975 (SPAD 3B98+3690). One float pair is swapped.
- func_002160B0 99.975 (void call, volatile, decl swap; MATCHED in the
  jr-table lane with `(p + 0x50)[j]`, see idiom-35). The scan loop's
  a0/v1 are swapped.
- func_001AD740 99.93 (SPAD). raw/next v0/v1 are swapped.
- func_001D1C50 99.93 (D_70003B8D array form, `flag = 0` moved inside the if,
  folded store). One `addu` operand order is left.
- func_001796C0 99.91. The float-constant temp is a0 vs v0 (byte-matched in
  decomp lane round 7 with a `// TU_STATIC:` callee).
- func_001D89D0 99.89 (ternary clamp `(c > 0.0f) ? c : 0.0f`, `<< 2`
  index). The flag byte is v1 vs v0.
- func_001AC070 99.85 (SPAD; MATCHED in the jr-table lane, idiom-35).
- func_00209280 99.82 (a declaration-order sweep found 1 of 240; spill slots
  remain).
- gs_readback_queue_drain 99.77. **Stale symbol:** it still calls
  `sub_D2_TADR_08x`, which is now `dma_wait_and_submit`. The same applies to
  gs_readback_queue_run. After the rename, the only residual is the index
  temp in a2 vs a0.
- func_001BC350 99.57 (MATCHED in the jr-table lane, idiom-35), func_0022EEF0 99.12 (a2 after a call with constant
  arguments), func_00157F60 96.75 (the target uses v0 for temps and our
  build never does, under any compiler or flag).

This round used a scratch harness under `build/lrod/m/`, since deleted. It
measured variants against the normalized target. The target was built per
variant so `// SPAD:` took effect. Its sweeps were decl (declaration order,
with initializers split out), uns, cc, spad, vol, cmp (idiom-21 compound
assignment) and brk. It also had an argscan that compared per-call-site
argument-register writes, target against ours. The unsigned-parameter,
compiler, volatile-toggle and compound-assignment sweeps found no new match
in this set.

### idiom-35 and the jr-table lane (NEARMISS dispatchers) — 2026-09-23

Scope: the 51 NEARMISS/INCLUDE_ASM functions whose `.s` references a
`jtbl_` symbol, first-level ones first. Re-measured against the normalized
target (build.py `normalize_asm`, table appended), most headers were current.
func_00135D00 was already 100% and only waited for the table pin. The
promotions are listed in docs/PROGRESS.md (2026-09-23 jr-table lane). Each
one below closed a real residual. Try them in this order before a
declaration-order sweep.

- **Scratchpad sweep first (idiom-32), including combinations.** Six
  promotions needed one or more relocated scratchpad externs. A greedy pass
  (best single address, then add each other address) found combinations the
  singles missed (func_0012A5D0 needed 0x700031F4 and 0x70003B8A together).
  When spad_symbolize leaves a second-type store as a literal, write it
  through the extern's own type. `D_700038A8 = 0.0f;` gives the target's
  upper-half address load into `$at` followed by a store of the zero register. `*(volatile int *)&D_700038A8 = 0` materializes the
  full address as a %hi/%lo pair (func_00130AB0). Under `-sdatathreshold 4` a new
  scalar extern must be over-declared as an array (idiom #20), or it goes
  gp-relative.
- **Float truthiness.** `if (x)` on a float emits the equality compare with the value
  as first operand and zero second. `x != 0.0f` and `0.0f != x` both emit it with zero first and the value second
  (func_0012A5D0).
- **Argument evaluation order follows expression weight, and a cast counts
  as weight.** mwcc computes the heavier argument first, and the
  last-computed one lands in the `jal` delay slot. The target computed
  `&D_700038A0` (a0) first and put `act + 0xB0` (a1) in the slot. The fix was
  to drop the `(float *)` cast on a1 and give the callee a `void *`
  parameter (func_0012E840, func_00130AB0). It also works the other way:
  `(int)D_00289BC0` or `(char *)D_00289BC0` in a func_00200780 call moves
  that argument earlier (func_00200360, func_001FF830, and the arg-order
  cluster of func_001FFCD0). Try it per callee: `void *` parameters with
  the argument casts dropped, then an explicit cast on one argument at a
  time.
- **Byte-table bit test.** `T[i] & (1U << n)` (table first, unsigned shift)
  gives the target's colouring. `(1 << n) & T[i]` swaps three registers
  (func_001BC350; func_00158EC0 goes from 99.67 to 99.95 with it).
- **Real array shapes instead of colouring devices.** func_00189730's old C
  called the two-argument copy_qw4 with four arguments to force `$a2`.
  Declaring the table `int D_0024A410[][6]` and indexing it `[row][col]`
  matched with the honest two-argument call.
- **`(p + K)[j]` vs `p[K + j]`.** They swap the address temporary and the
  compared byte (func_002160B0).
- **`flag = (a == b);`** matched where `flag = 1; if (a != b) flag = 0;`
  left a 15-row scheduling cluster (func_001B9CF0).
- **Chained assignment keeps an address in a register.** `D_00275C70 = m =
  D_00289BC0;` keeps `&D_00289BC0` in `$a2` for the following `m + 0x18`
  load. `m = D_00289BC0; D_00275C70 = m;` lets mwcc constant-fold every use
  to `D_00289BC0+off` (func_00200360).
- **Do not cache a volatile pointer for one store.** `q = GS; D_x = 1;
  q[8] = 4;` colours q into v1. Writing `D_x = 1; GS[8] = 4;` lets mwcc load
  the pointer above the unrelated store into v0, as the target does
  (func_001AC070).
- **Separate loop counters.** A second loop that reused the first loop's
  `i` coloured its counter, bound and cursor differently. A new `k`
  declared after the cursor (`q, cnt, e, k`) matched once the first loop's
  locals had been ordered by a declaration sweep (func_001FF830). The index
  `((h + (int)i) << 3)` keeps the target's per-iteration recompute instead
  of a strength-reduced induction register (func_001FF830, as in the
  already-matched func_00200360).

Classes found and not cracked (the source is unchanged unless noted):
- **Same-TU callee register knowledge.** func_001BD560 keeps `$a0`/`$a1`
  live across calls to func_001BD270/func_001BD180/func_001BD370, which sit
  just before it. func_0021B9A0 keeps `mode` in `$a2` across func_0021B920.
  The original compiler knew those callees preserve the registers because
  they were in the same file. One-function units cannot do that (see
  PS2_DECOMP_PLAYBOOK §6a). Cracked in decomp lane round 7 (below):
  `// TU_STATIC:` compiles the callee as a static function of the caller's
  file; func_0021B9A0 byte-matches that way, and func_001BD560 in round 8
  (its `$a1` "constant 1" was a wrong extra argument in the old C).
- **func_001B99F0 is two functions in one splat symbol.** It is a 4-instruction
  tail-jump stub, and func_001B9A00 starts at +0x10. Its "2.80%" is the
  objdiff of the stub against the merged symbol. The split needs
  symbol_addrs.txt plus a re-splat.
- **Constant argument materialized first.** The target builds the float
  constant and the plain register arguments before the load-dependent
  argument. func_0018BC20's default case (the identical case 0xA matches),
  func_001916C0, func_00128C10 and bone_root_pulse show this. No cast,
  prototype, temporary, double/int literal or operand order changed it.
- **func_001741D0.** The target leaves a `beqz` NOP slot where mwcc 2.3.3 fills
  the slot from the fall-through block (5 sites). No compiler/flag
  combination reproduced it.
- Near-misses measured but not landed: func_001AD740 99.98 (`next =
  D_0081070A[0]; if (next != 0xFF) next++;`; one register), func_00183EF0
  99.975 (SPAD 0x70003B98+0x70003690; one FP pair), func_00158EC0 99.95
  (constant 1 in `$a3`), func_001429D0 99.83 (decast of
  func_00102948/func_001028B8), func_001FFCD0 99.68 (cast + case-5 shape),
  func_002149F0 99.61 (void func_0020CDA0 call, volatile
  D_002821B0/B4, `(p + 0x50)[i]`), func_001FF590 97.89 (all scratchpad
  externs; the rest is colouring around two func_00200780 calls).

### Round 6 (first-level NEARMISS follow-up) — 2026-09-23

Promoted func_00158EC0, func_00183EF0 and func_001FF590 (details in
docs/PROGRESS.md). Levers, with the function that proved each:
- **Read the callee's arity before anything else.** func_00158EC0's last
  residual was the constant 1 in a3 feeding a `p[0] = 1` store. func_001C5570 takes four
  arguments; the file declared three. Grep the corpus for other calls
  (`func_001C5570(p, D_700038A0, 0x75, 1)`).
- **A `case K:` label on the default arm restores a dead `li`.** func_002149F0's
  sparse switch on `rec[0x34]` had a constant 2 loaded into v1 directly before the
  default's constant 8 in the target, and a lone `nop` at a scan-loop label. Adding
  `case 2:` to the default arm of both switches fixed all three rows. A dead
  constant load right before a default arm is a case value with the default's
  body.
- **Int round trip orders `addu` operands.** `q = (unsigned char *)((idx << 3)
  + (int)base);` gave the target's `q = idx8 + base` operand order; pointer arithmetic in
  either operand order, `&base[idx * 8]`, `idx * 8` and `idx << 3` all gave
  the reverse (func_001FF590). Comment such casts as matching devices.
- **Stage the right operand of a float subtraction first.** `dx = p[0]; dx = a
  - dx;` swapped the lwc1 colouring of `a - p[0]` (func_00183EF0, permuter).
- **Per-site zero staging (idiom-24).** Apply it only where the target moves
  zero into `$f13` first; staging every `anim_clip_init(e, n, 0.0f, 0.0f)` in
  func_001429D0 made it worse, the two right sites gained 0.07%.
- **Load order of call arguments.** Put the first argument in a block local
  declared before the second (`float tgt = d->x; cur = e->y; f(tgt, cur, ...)`)
  to load the second first (func_001429D0).
- **rand(void) and argument devices.** func_00122BB8 takes no argument. Where
  an argument only reproduced a value already in `$a0`, removing it left the
  bytes identical (five matched files). Keep one only when the file's score
  drops without it, declare the callee unprototyped in that file, and comment
  it (func_001F8350).
- **Permuter note.** tools/permuter/run_func.sh compiles with mwcc 991202 and
  assembles the raw splat .s, so it cannot score mwcc233 files, `// SPAD:`
  files or dispatchers correctly. This round ran the permuter with the file's
  own `// COMPILER:`/`// CFLAGS:` against the build.py-normalized target
  (scratchpad symbolization and the jump table applied). That found the
  func_00183EF0 shape and the staging half of func_001FF590. Dispatchers
  start at score 30 or more because of the anonymous local .rodata symbol
  name; always objdiff the outputs.

Measured but not landed (see the NEARMISS headers): func_001AD740 99.98 (the
saturating increment lands in place in v1; the target writes a fresh v0),
func_002149F0 99.97 (one compare's rec/p12 load colouring), func_001429D0 99.94
(parameter-copy order in the prologue), func_001FFCD0 99.67 (three colouring
clusters; permuter 99.72), func_001C2770 98.27, func_001D8C30 89.45.

### Decomp lane round 3: the unmarked `c_asm_fallback` set — 2026-10-20

All 50 `c_asm_fallback` units (ordinary C below objdiff 100, linked from the
`.s` through fill_unmatched's lists) were rewritten from the original
instructions: 45 byte-matched, 5 NEARMISS. Read the target before trusting an
old body: most were wrong, not near (wrong callee, wrong fallback string,
one dereference too many). Levers that decided them:

- **ee-gcc for the SDK range, then source order.** None of the 21 SDK-range
  units matched with mwcc. With ee-gcc, the residuals were the order of
  independent stores and the operand order of an address add. The scheduler
  reorders stores, so try store permutations mechanically (func_0010C9C8
  matched with the 0x4, 0xC, 0x8 order; func_0010DC78/90 with the natural order).
- **ee-gcc sibling calls need a void caller.** `j callee` instead of `jal` +
  frame appears only when the caller returns void (func_001098E0,
  func_00119400). A caller that returns the callee's int keeps the frame
  (func_00120B10 returns it, and the target has the frame).
- **ee-gcc keeps a `lui/addiu` + offset load for volatile state.** A single
  field read of a plain extern folds to `lw %lo+off`; the target's separate
  `addiu` came from `extern volatile int D_0027F740[]` (func_001152B0). The
  same volatile array gives the re-reads of the queue counter in
  func_001157F0 / func_00119400.
- **Struct member arrays fix ee-gcc's add operand order.** `D.b[i]` on
  `struct { int a[0x30]; int b[0x30]; ... } D` gave `base + i*4` with the
  member offset in the load; every pointer spelling gave `i*4 + base`
  (func_0011A730, func_0011A938). An `int[5]` struct copy gives ldl/ldr pairs plus
  one `lw`; `char[20]` gives lwl/lwr (func_00101EA8).
- **Reusing the parameter as the base register.** The target put the table
  base in `a0` after shifting it: `int *p = (int *)(a0 * 8); a0 = base; p =
  (int *)((char *)p + a0);` (func_0010DCA8 / func_0010DCD8).
- **DMA-list tag appends (mwcc 2.3.3).** The family appends 16-byte tags
  through `DmaTag **pp = &D_00275670->cur[ch]` (the list header is a struct
  with `DmaTag *cur[16]` at +0x10, `int ref[16]` at +0x50 and the frame word
  at +0x9C). Write `(*pp)->id`, `->addr`, `->qwc`, then `(*pp)++`; compute the
  address into a local before `pp` (func_001D1FF0 / 1F20 / 1F80 / 2040 /
  38A0 / 4E20 / 4EB0, vif_append_ref_tag). vif_append_ref_tag takes (ch,
  addr) and vif_build_unpack_const takes ch: several NEARMISS files still
  declare one argument. Correcting it made func_001D37D0 100 (it was 95.41
  NEARMISS) and lifted func_001D42E0 to 97.53 and func_001D4440 /
  func_001D40E0 to 98.41; with func_001D2910 and func_001D1F20 also given
  their one-argument prototypes all three matched in round 4 (below).
- **`||` chains for mwcc early returns.** `if (a || b) return 2;` reproduced
  the target's branch layout where nested ifs or a result variable did not
  (func_001ACE70, func_0016ADE0 with `!= 0` on both terms). `if (a && b && c)
  return 0; return 1;` did the same for func_0021C3F0.
- **Assignment inside the condition.** `if ((s0 = f(2)) == 0) return;` tests
  v0 in the branch and copies to s0 in the delay slot; a separate `s0 = f(2);
  if (s0 == 0)` tests s0 (func_0014D1E0). The same function needed the
  constant handler store last before the call (idiom-16).
- **A pointer local for a call argument computed early.** `unsigned char *p =
  D_008102B0; if (*p == 1 && func_0021BB00(p) == 0)` puts the address in a0
  before the test, as the target does (func_0021BD10).
- **The leaf-call a0 quirk.** In func_001899C0 the target passes self to
  func_00189730 without setting a0, because func_00189630 (a leaf) never
  writes a0. mwcc reloads a0, and no source spelling found reproduces the
  omission; an uninitialized local gets a saved register (78.26 NEARMISS).
  func_0021D800's unset a0 (idiom-32 notes) may be the same case; that is not
  checked. (Both are the same-TU case: byte-matched in decomp lane round 7
  with `// TU_STATIC:`.)

### Decomp lane round 4: the DMA-list NEARMISS family — 2026-10-20

Fourteen NEARMISS files that build VIF/DMA ref-tag lists on the D_00275670
channels were rewritten from the original instructions: 12 byte-matched
(func_001D3E40, 001D3F60, 001D40E0, 001D42E0, 001D4440, 001D4650, 001D4A90,
001D4DA0, 001D4F30, 001D4FC0, 001D5170, 001E1E60) and 2 still NEARMISS with a
corrected body (func_001D4B80 / func_001D4C30, 95.39). Levers:

- **Real prototypes first.** Most of these "register permutation" residuals
  were wrong calls: vif_append_ref_tag (ch, addr), func_001D2910 (id),
  func_001D1F20 (ch), func_001D1F80 (ch, a, b), block_copy (dst, src, len),
  func_001CAAC0 (pos, payload), func_001D4A90 (ch, src). Extra arguments in the
  old C were register leftovers. func_001D4B80 / func_001D4C30 passed 0 to
  vif_build_unpack_const; the target passes the channel (a0 is not set before
  the call). The port's oracle-checked em_aim_fire_lamp already passes a0.
- **Saved registers follow declaration order, parameters come last.** mwcc
  2.3.3 gives callee-saved registers to locals in declaration order (s0, s1,
  ...) and colors a parameter that stays live after them, so it takes the
  highest one. When the target keeps a parameter in s0 or s1, copy it into a
  local declared first, with an explicit cast: `int *m = (int *)param;` (an
  identity cast works; a plain `m = param` is coalesced into the parameter and
  changes nothing). func_001D3E40 (model in s0, packet pointer s1),
  func_001D4FC0 / func_001D5170 (model s0, run s1, saved list position s2) and
  func_001E1E60 (`i = (int)arg1; off = i * 4;`, i in s1 under off in s0) all
  matched this way. Check the target's s-register order against the
  declaration order before calling a saved-register permutation a wall.
- **The 0x1F8-chunk loop.** Store the tag's qwc (n * 0x82) before advancing the
  address by n * 0x820: the target forms the qwc shift first from the shared
  n * 65 (func_001D4A90 / func_001D4F30, identical twins; func_001D4FC0 /
  func_001D5170). `while (n > 0)` and `if (n > 0) do {} while (n > 0)` give the
  same code.
- **Two reads of the cursor for a split advance.** func_001D4FC0 reserves 0xF
  tags after a header and copies into the second: `dst = *pp + 1; *pp = *pp +
  0xF;` gives the target's order (new cursor formed last in the loaded
  register); `tmp = *pp; *pp = tmp + 0xF;` does not.
- **Open: func_001D4B80 / func_001D4C30.** Same instruction stream; only
  caller-saved temps differ. The target never uses v0 after the second call,
  so every temp is one register later than mwcc's (context a2, slot t0, base
  t1, frame t2). The matched func_001D4EB0 / func_001D4DA0 (no trailing call)
  also avoid v0 and match, so the trailing func_001D4A90(ch, src) call is what
  changes mwcc's choice. Declaration and order sweeps, an inline helper, return
  types and a 25-minute permuter run found no lever.
  Closed in decomp lane round 8: both byte-match with func_001D4A90 as a
  `// TU_STATIC:` callee (the original compiler's knowledge of that callee
  decided the temporaries).

### Decomp lane round 5: call-site checks on the on-route NEARMISS set — 2026-10-20

The 288 mwcc NEARMISS files on the first-level / AREA01 / AREA00 census lists
were compiled and compared call by call with the target (argument registers
set before each call, call order, callee names), and the arity of every call
was checked against the callee's own C definition. Three files byte-match now
(func_002134C0, func_00176390, func_001FA330) and 16 NEARMISS files have a
corrected or closer body. Levers:

- **A call with no argument setup passes the caller's own `a0`.** When the
  target sets no `a0` before a `jal` to a callee that reads its first
  parameter, the original passed the value still in `a0`, normally the
  caller's first parameter. The C must pass it; mwcc then also emits no move.
  The old C had these calls without arguments: func_00176390
  (func_001762E0(arg0), func_00176BE0(arg0)), func_0017F5F0
  (func_00174FD0(arg0); this was its "prologue move-order" residual),
  func_00158D30 and func_00156620 (func_001B0FD0, func_001AFC10),
  func_0015FDF0 (func_001AA4E0), func_001764E0 (func_00176C80). In
  func_001662D0, case 0x15 / 0x20 called func_001C68C0 with the flag word or
  the constant 0x20; it takes the actor.
- **Extra arguments are register leftovers.** func_00180460 / func_00180530
  take only the actor (the old C passed 1), func_0019F730 takes none (the old
  C passed the three loop cursors that happened to sit in a0/a1/a3), and
  func_001CB950's fourth parameter only forwards a3 (callers pass three).
- **Relational spelling.** `x > 1` gives the target's compare-and-branch
  through `$at` where `x >= 2` computed the result into a GPR; the operand
  order follows the source (`p[0x1C] < n - 1`, not `n - 1 > p[0x1C]`).
  func_002134C0 needed both. A plain float truth test (`if (v)`) gives the
  target's `c.eq.s` operand order where `v != 0.0f` swapped it (func_001662D0,
  func_001FA330).
- **Branch arm order.** func_001FA330's cap select is `if (i != 0) A; else if
  (D == 0) A; else B;`: the target falls through to the first arm and jumps to
  the last.
- **A stack copy of a word table indexed by a variable** is a word array:
  `struct { int w[8]; } buf = D_x; buf.w[i]` gives the target's
  `sp + 4 * i` load at the table offset (func_00176390); indexing through a
  cast of a two-quadword struct formed the base first.
- **Declaration order, swept.** For a saved-register permutation, compiling
  every order of the local declarations (up to 7 locals, a sample above)
  found the target order for func_001FA330 (with the two fixes above, 100),
  gs_readback_queue_drain, func_001E9280, func_0019AB20 and func_0019BA80.
- **Stale aliases.** gs_readback_queue_drain / gs_readback_queue_run called
  sub_D2_TADR_08x, the pre-rename name of dma_wait_and_submit (0x00100A60).
- **Measure against a fresh expected object.** build.py rebuilds
  build/expected only for compiled units, so a NEARMISS file's expected
  object can predate a relabel. Rebuilding the 716 NEARMISS expected objects
  changed 48 scores by up to 0.5 points, most of them SDK callers of the
  syscall stubs relabelled on 2026-09-27 (SignalSema measured against an
  expected "CreateSema"). integrate_nearmiss.py always rebuilds it
  from the normalized `.s`; do the same before diagnosing a NEARMISS.
- **Tested, no effect:** defining the leaf callee func_0021D600 before
  func_0021D800 in one file does not remove func_0021D800's `a0` reload at
  -O4,p, and mwcc 2.3.3's `-help all` lists no interprocedural option, so the
  "same-TU callee register knowledge" class stayed open (closed in round 7:
  mwcc 2.3.3 applies it only to a `static` callee, `// TU_STATIC:`). The float-constant
  argument order of func_001662D0's func_00180600 calls (and func_00162DB0's
  func_001B12B0 call) did not move with literal, local, statement or switch
  forms.
- **Next:** idiom-32 (scratchpad literal versus per-address extern) is the
  main residual left in func_00158D30, func_0019AB20 and func_0019BA80. Done
  in round 6 (below), per file with `// SPAD:`, so `_SPAD_SYMS` is unchanged.

### Decomp lane round 6: idiom-32 swept per address on the on-route NEARMISS set — 2026-10-20

`tools/match/spad_sweep.py` (new, measurement only) took the 129 on-route mwcc
NEARMISS files that dereference `*(T *)0x7000xxxx` literals and compiled, per
file, variants that spell a chosen set of those addresses as `extern T
D_7000xxxx;` (plain and volatile), opted in with `// SPAD:`. Each variant is
scored against an expected object symbolized from that variant's own text,
which is what integrate_nearmiss.py measures after promotion. Files with up to
8 such addresses get every subset; larger ones get none / all / singles /
all-but-one plus greedy toggles. Ten functions byte-match now (mwcc 2.3.3,
objdiff 100, compiled_object_ordinary_c), seven of them recorded as walls:

- **Directly by the sweep:** func_00175CF0 (99.15; 0x700031C0),
  func_00176180 (97.24; 0x700031C0 / C8 / F0), func_00177F40 (89.75;
  0x70003058 / 68), func_0019B2C0 (96.72; 0x700031D0 / D8), func_0019AB20
  (95.99; 0x70003194 / 31D8 required). Their headers called the residual an
  FP-load scheduling wall, an FP coloring permutation, or a delay-slot fill
  no source change fixes.
- **Not every literal is an extern.** In func_0019AB20 / func_0019BA80 /
  func_0019B8C0 the clears of 0x700031AC and 0x7000319C only match as literal
  stores: as externs (plain or volatile) mwcc moves them (88.9), while
  0x70003194 / 0x700031D8 only match as externs. func_0019B4C0 wants all of
  its addresses as externs. Measure every address, alone and in combination;
  the sweep does.
- **Sweep plus one more lever:** func_0019BA80 (95.40) also needed `i = 0`
  before `d` and `t` among its six setup assignments (an exhaustive
  permutation sweep). func_0019B8C0 (91.32, "register-allocation artifact")
  has func_0019BA80's exact instruction shape with other callees; BA80's body
  with those callees matched at once. func_0019B4C0 (96.56) needed
  `*(float *)(a0 + 0xB0) += p[12];`: both `= p[12] + f` and `= f + p[12]`
  give 99.84 with the wrong add operand order. func_00177CF0 (99.71) needed
  func_00102918's third parameter as `void *`, its real prototype: the
  `(float *)&sp20` cast is weight that makes mwcc set a2 before a1 (the
  argument-order rule above).
- **Volatile keeps the store order.** func_00158D30's four colour words at
  0x700038B0..BC: as plain externs mwcc schedules the zero stores first
  (88.2); the target keeps source order, as literals and volatile externs do.
- **`return` versus `break` decides an inner switch's delay slots.**
  func_00158D30's outer switch ended every case with `return`. With that, mwcc
  filled both conditional-branch slots of the inner switch with the
  scratchpad upper half in every spelling tried (literal, plain or volatile
  externs, all 256 subsets): 96.65 at best. With `break` (the switch is the
  last statement, so it means the same) and volatile externs: 100. The same
  inner switch compiled alone, or with a call after it, left the slots as nop.
  This qualifies the idiom-32 rule "a relocated `lui` is not speculated".
- **Left for later (NEARMISS files not changed this round):** the sweep's best
  sets raise func_0019B6C0 81.83 -> 91.37, func_0019A910 81.29 -> 91.36,
  func_0019A570 84.72 -> 89.19, func_0019AA80 90.51 -> 92.18 (the same
  scratchpad-transform family; their sizes still differ, so their bodies
  differ structurally), func_0015FDF0 91.01 -> 96.82, func_00177030
  92.32 -> 95.34, func_001A2370 80.63 -> 86.18, func_0015EC50
  96.50 -> 98.10, func_001921D0 96.32 -> 97.84, func_001916C0
  98.80 -> 99.31 (left: the order in which one func_0018C4B0 call
  materializes its 4.0f argument; casts, parentheses and a pointer cast did not
  move it). The full table is in the sweep's JSON output; rerun
  `spad_sweep.py` on them.

### Decomp lane round 7: same-TU static callees (`// TU_STATIC:`) — 2026-10-20

The original compiler saw several functions in one file. When a callee came
earlier in the same file, CodeWarrior knew which registers it uses and kept
values in caller-saved registers across the call. mwcc 2.3.3 does the same,
but only when the callee is a `static` function defined before the caller in
the same file (a non-static definition in the file had no effect, which is
why round 5's test of func_0021D600 before func_0021D800 found nothing).
Tells in the target, all at a call to a nearby lower-address callee:

- no `a0` (or `a1`, `a2`) setup before a call although the callee reads it:
  the value is still there from the previous call to the same-TU callee
  (func_001F6BB0's func_001F6640(p0) / func_001F66F0(desc); func_0021D800's
  func_001FBD50(arg0, ...));
- a value kept in an argument or temporary register across the call with a
  smaller frame, where a one-function file spills it to a saved register
  (func_0021B9A0 keeps `mode` in a2 across func_0021B920);
- a call with no `a0` setup after a leaf callee (func_001899C0 passes self to
  func_00189730 after func_00189630);
- a temporary coloured away from `a0`, or a branch-likely where the
  one-function file has a plain branch, because `a0` is still live after the
  call (func_001F6BB0's D_0081079E load into a1; func_001796C0's constant
  built in a0 instead of v0).

How (tools/decomp/tu_static.py; build.py and integrate_nearmiss.py run it
after compiling any file that has the directive):

    // TU_STATIC: func_0021D600
    static int func_0021D600(char *a0);   /* internal linkage */
    #include "src/func_0021D600.c"        /* path relative to the repo root */

The static prototype gives the included non-static definition internal
linkage (C99 6.2.2), so the callee's own unit file is unchanged and is still
the only source of its C. The callee's prototype and the caller's
declarations must agree (func_0021D600's parameter is now `char *`, as its
callers declare it). mwcc emits each function into its own `.text` section;
after compiling, tu_static.py checks that the static copy's bytes and
relocation offsets/types equal the original callee in
build/expected/<callee>.o, removes that section with its `.mwcats` catalog,
and turns the callee symbol into an undefined global. The object is then an
ordinary one-function unit whose calls link to the callee's own unit
(compiled_object_ordinary_c). A failed check deletes the object, so the
function links from its `.s` and is not counted. The callee must therefore be
a byte-matched C unit; a NEARMISS callee (func_001C7420 for func_001CABA0,
which reaches 99.26 with its static copy) does not qualify.

Byte-matched this round with mwcc 2.3.3: func_001796C0 (99.91; static
func_00179680), func_0021B9A0 (88.61; static func_0021B900 + func_0021B920),
func_0021D800 (99.15; static func_0021D600) and func_001F6BB0 (78.73; static
func_001F6AC0), all four on the first-level / AREA01 / AREA00 census lists,
and func_001899C0 (78.26, the "leaf-call a0 quirk" above; static
func_00189630). func_001796C0 and func_001899C0 also left fill_unmatched's
SIZE_DRIFT_FORCE_ASM list; audit_link_provenance shows all five as
compiled_object_ordinary_c.
func_001F6BB0 also needed two source fixes: its selector dispatch is a sparse
switch (labels 0, 0x1301, 1, 2, 0x100, 0x200, 0xE00, 0x1100 give the target's
reverse compare order and body layout, idiom-33), and its calls pass the
pointers the callees read (func_001F6640(p0) / (p1), func_001F66F0(desc));
the old C called both with no argument.

Measured, not landed (two sweeps, measurement only): pasting static copies
of every lower-address callee (within 0x8000 bytes) into the 84 on-route
NEARMISS files that have one, and including every lower-address byte-matched
callee unit the `// TU_STATIC:` way into all 149 mwcc NEARMISS files that
have one (`-relax_pointers` added for the measurement). No further file
reached 100. Gains: func_001916C0 98.80 -> 99.49 (its scratchpad externs then
lower it again; the rest is one unconditional-branch delay slot),
func_001CABA0 93.31 -> 99.26 (but its callee func_001C7420 is a NEARMISS, so
it cannot be a TU static), func_001854E0 88.66 -> 91.70, func_001936E0
94.99 -> 95.75, bone_root_pulse 98.25 -> 98.71, func_00214020
98.68 -> 98.74. Many variants did not compile because callers and callees declare
the same function or global with different types (an int where the callee
takes a pointer, `char[]` against `unsigned char *`); reconciling those
declarations is the next step before re-running the sweep. func_001BD560
(off-route; func_001BD180 / func_001BD270 / func_001BD370 with other
`-sdatathreshold` values and m2c prelude typedefs) is untested.


### Decomp lane round 8: declaration reconciliation and the TU-static sweep re-run — 2026-10-20

The round-7 sweep (every lower-address byte-matched callee within 0x8000 bytes
compiled as a `// TU_STATIC:` copy into each of the 148 mwcc NEARMISS files that
has one) was re-run with the declaration conflicts resolved for the measurement:

- the caller's own declaration of the callee is removed (only a file-scope
  declaration: a pattern that also matches indented call statements silently
  deletes calls, and the variant then scores against a different function);
- each argument of a call to a TU callee is cast to the callee's parameter type
  when both are integer or pointer types (`-relax_pointers` for the rest);
- file-scope names, typedefs and struct tags that the caller or an earlier callee
  also declares are renamed inside each included callee with a per-callee
  `#define`, and the static prototype uses the callee's resolved typedefs with a
  forward-declared struct tag (m2c prelude callees such as func_001BD180).

Compile failures fell from 167 to 84 variants; the rest are mostly callers whose
calls disagree with the callee's own parameter count (below). Measured this way,
func_001D4B80 and func_001D4C30 reached 100 with func_001D4A90 as a TU static,
which closes their "caller-saved colouring" residual. The renames are a
measurement device only: a landed file must declare the same types as its
callees (func_001D4A90's `VifLists` now carries the `ref` / `frame` fields its
callers use; func_001BD270 and func_001BD460 both declare
`char **D_00275B40`; all four still objdiff 100).

Byte-matched this round (mwcc 2.3.3, compiled_object_ordinary_c):

- **func_001D4B80, func_001D4C30** (95.39): `// TU_STATIC: func_001D4A90`.
- **func_001AA840** (84.27): `// TU_STATIC: func_001AA7A0`, and the call is
  func_001AA7A0(arg0, e). The callee tests its second parameter; the target sets
  only `$a1` and leaves this function's own argument in `$a0` (its caller
  func_0018A1F0 passes one). The old C passed the entry as the only argument.
- **func_0012B410** (99.09): func_00128640 takes one argument (the old C also
  passed 4, a value the target never sets), and `// TU_STATIC: func_00128390`
  gives state 4's `arg0[0] = 1` store from `$a1`, still the callee's argument 1.
- **func_001BD560** (99.08, off-route): its four mover polls take only the
  mover; the old C passed a second argument 1, which the old header explained
  as "CW kept the constant 1 in `$a1` across the leaf helpers". With
  `// TU_STATIC: func_001BD270 func_001BD460` it matches. The callees are
  compiled with `-sdatathreshold 8` (D_00275B40 gp-relative), and a static copy
  compiled at the caller's threshold 0 fails tu_static.py's original-callee
  guard, so the caller moves to threshold 8 and declares the four 0x0081xxxx
  bytes it addresses absolutely as arrays (they would otherwise become
  gp-relative). Check the guard, not only objdiff: objdiff scores the caller
  symbol alone and reads 100 either way.

A constant argument that the target never materializes is a register leftover,
not an argument; when the callee is a nearby lower-address function, the
leftover is the same-TU knowledge, and the TU static reproduces it.

Body corrections (NEARMISS, from the original instructions):

- **anim_matrix_player** 82.85 -> 98.09: func_0017B490 is (obj, cmd, idx, tbl)
  and gets (self, 1, +0x235, +0x25C); anim_clip_arbiter's third and fourth
  arguments are floats (0.0 in `$f12`, the position in `$f13`); func_001C61D0
  returns an int length that the caller converts; the next-clip position
  re-reads +0x3C after func_00179D20; D_00287F40 / D_00288D40 are absolute.
  Left: saved-register colouring only (1500 declaration orders, mwcc 2.4 / 991202
  and four placements of the clip-id copy tried).
- **func_0016EBA0** 93.89 -> 99.13: func_00179880(self, self + 0x2EC) (the old C
  passed only the pointer), func_001B0B50() with no argument, D_70003A20 named
  (idiom-32), and `break` for every `return` inside the switch (idiom-27). Left:
  one branch delay slot that mwcc fills with the upper half of a scratchpad
  store.
- **func_001BAD40** 91.09 -> 91.26: func_001C6120 is (table, index), and cases
  1/8 and 2 pass the message id still in `$a1`; case 5 calls func_001C5C90(obj).

Next (from the 84 failing variants): 36 are calls whose argument count differs
from the callee's own C. In most of them the callers are right and the callee's
definition takes fewer parameters than the original passes (func_001DF9D0(void)
called with a float, func_00204080(void) with `&D_00273490`, func_001D8130(void)
with two values, func_00206B00(char *) with five pointers, func_00206B10 with two)
or more (func_00213F30 takes five, of which it reads the last two only for an
unknown state byte; its callers pass one). Fixing those callee prototypes (each
must stay objdiff 100) unblocks func_001E7570, sub_pts_is_not_used,
func_001D89D0, func_00206BF0, func_00214020, sub_sceMpegGetPicture_decode_error,
func_002044F0 and others for the TU-static measurement. The remaining 16
redeclaration and 15 conversion failures are struct-typed or float/int
mismatches the automatic casts leave alone.


### Decomp2 lane round 1: callee prototypes from the original, then the TU-static sweep — 2026-10-20

The callee definitions that round 8's failing variants pointed at were fixed to
take what the original passes. Each stays objdiff 100 (the bytes are unchanged)
and now agrees with its callers:

- func_00204080 / func_00204090 (empty leaves): one pointer, never read. Every
  caller sets `$a0` to a message (D_00273450, D_00273470, D_00273490,
  D_002734A0, D_002734C0, D_002734E0) before the call: a compiled-out debug
  print, not a `(void)` function.
- func_001DF9D0(float amp): the caller's float in `$f12` is passed on to
  func_001DF600's fourth (float) parameter, the pass-through already recorded
  for its sibling func_001DF5A0 -> func_001DF180.
- func_001D8130(int flag, int p): called like its sibling stages
  func_001D8340 / func_001D8690 with the 0x20 flag and the record pointer
  (byte-matched func_001D88B0 sets both); this stage reads neither.
- func_00206B00 / func_00206B10 (thunks): forward `$a1..$t0` / `$a1`
  untouched to func_00204390 (ring, four out-pointers, read in its prologue)
  and func_00204490 (ring, count).
- func_00213F30(char *arg0): one parameter. For a state byte above 4 its switch
  assigns neither bound; uninitialized locals `int lo; int hi;` declared after
  `i` give the original's `$a3` / `$t0` and the same bytes. Round 8 had read
  them as parameters 4 and 5 that no caller passes. (The port's
  em_area01_ui_00213F30 already treats them as unknown registers.)
- func_00213A00 declares func_00207E40's last parameter `unsigned long long`
  like the definition (it said `long long`, which conflicts inside one TU).

Byte-matched (mwcc 2.3.3, compiled_object_ordinary_c):

- **func_00214020** (98.68; the port's AREA01 status page 3):
  `// TU_STATIC: func_00213A00 func_00213C50 func_00213F30` (func_002134C0 is
  compiled at `-sdatathreshold 8`, so its static copy fails the guard and is
  left out; the other callees made no difference), plus three spellings found
  against the target: the list scan reads `s0 + 0x50 + i`, the cursor pick is
  `(unsigned char)s0[func_0020BEF0(s0) + 0x50]` (the spelling of
  func_00218D90's same pick), case 3 clears D_008106B0 before it stores the
  state, and case 4 declares `count` before `tbl`.
- **func_001E7570** (98.11): written in the spelling of its matched twin
  func_001E7440 (store the eased value, re-read it for the test, `nx = -x` as
  a local, `break` out of case 1). Its "FP register wall" was the old
  spelling. **Lever: when a NEARMISS has a byte-matched structural twin, copy
  the twin's C and change only the constants before trying anything else.**

Body corrected (NEARMISS 92.40 -> 99.46): **func_002044F0**. func_002041A0 is
`void (unsigned long long *out, a, b, c)`; the old C used its "return value"
as the transfer count. The count is n = arg0[5] >> 11, which the original keeps
in `$v0` across both calls because func_002041A0 is a same-TU leaf
(`// TU_STATIC: func_002041A0`); the loop index / cursor stay in `$t1` / `$t0`
and the frame is 0x40. The old header's "whole-function register allocation"
residual was this. Left: register choice of three load pairs in the index
arithmetic (2500 declaration orders, 162 argument spellings, a struct form of
the ring, mwcc 2.4 and 991202 tried).

Re-measured with the fixed prototypes, no further gain: func_001D89D0,
func_00206BF0, sub_pts_is_not_used and sub_sceMpegGetPicture_decode_error (the
TU statics compile now but leave the score unchanged or lower).
