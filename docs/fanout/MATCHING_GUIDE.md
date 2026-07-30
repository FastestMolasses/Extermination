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
- `&D_xxxx` extern + `-sdatathreshold 0` → `lui/addiu` reloc pair.
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
tail call = last statement → `j target`. 20 array over-decl forces absolute addressing.

## Recently discovered levers (rounds 1-2 — try these)
- **slti vs sltiu split**: `if (p[5] < 3)` on an unsigned-byte field yields `sltiu`;
  to get the target's SIGNED `slti`, cache into an int first: `int v=p[5]; if(v<3)`.
- **per-access global recompute**: `*(volatile int *)0x700031F4 = x;` reproduces CW's
  per-store `lui hi; sw lo(at)` recompute (full-constant address only — `volatile`
  on `base+offset` FAILS, mwcc splits the offset into lui/ori).
- **return-value-before-void-call**: compute a call's result into a local BEFORE a
  following void call, so the computation emits before the call (matches CW).
- **interleaved lq/sq copy**: assign 128-bit elements (`__attribute__((mode(TI)))`)
  one at a time to get CW's `lq v1,X; sq v1,X` per-slot (vs mwcc's batch load/store).
- **idiom 12c (const call-arg)**: `n=K; f(p,0,n);` — materialize a constant arg as its
  own statement to fix "a2-before-a1" / const-vs-forwarded call-arg ordering.
- **sq-frame call-wrappers** (return a comparison of a callee result, e.g.
  `return f() < 0;`) match cleanly; `sd`-frame SDK functions do NOT (mwcc always
  emits `sq $ra`) — skip those, they're a toolchain wall.
- **branch-arm store duplication**: when CW duplicates a common store block into
  BOTH if/else arms (keeping a shared const live), write the stores INSIDE each arm
  in the C source, not once after the branch.
- **named-symbol call args**: pass a global as a named symbol (`extern T D_xxxx[];`
  then pass `D_xxxx`/`&D_xxxx`) so the call-arg gets a `%hi/%lo` reloc (`lui;addiu`);
  a raw `(void*)0xADDR` cast lowers to `lui;ori` and won't match. (Stores can still
  be raw `*(volatile T*)0xADDR`; only pointer ARGS need the named symbol.)
- **quadword-store offset folding**: `*(u128*)(base+off)=0` does NOT fold (emits
  `addiu;sq`); use a struct member (`p->q10=0`) or array index (`((u128*)p)[1]=0`)
  to get `sq zero,16(p)`. (`typedef int u128 __attribute__((mode(TI)));`)
- **mixed-type vec4 stack buffer**: to get `swc1...; sw 0x3F800000` without an extra
  pointer reg, use `struct{float x,y,z; int w;}` not `float buf[4]`+`*(int*)&buf[3]`.
- **large struct-member offset (>0x7FFF)**: declare the field at its exact offset in a
  struct (`struct{char _p[0xA0B8]; char *pA0B8;}`) to reproduce CW's
  `lui at,hi; addu at,base,at; lw lo(at)` (raw `*(int*)((char*)p+0xA0B8)` gives `addiu`).
- **0.0% diff = addressing-mode divergence**: if objdiff reads 0.0% but the disasm
  looks structurally right, the FIRST instruction's addressing mode is wrong (gp-rel
  vs absolute) — fix `-sdatathreshold` or the array-overdecl size first.
- **64-bit bit-packer idiom**: GS-register packers using `dsll32/dsra32` (signed) or
  `dsll32/dsrl32` (unsigned) + `dsll`/`or` are matched by a `long long` expression
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
  constant in a `float` LOCAL: `float zero=0.0f; if (x==zero)` → `c.eq.s value,zero`
  (matches); `x+one` with `float one=1.0f;` → `add.s value,one`. Source operand-swap of
  a literal does NOT work; the local does.
- **Float-compare branch shape (controls f0/f1 + polarity)**: write the body as the
  if-TRUE arm with the condition NEGATED — `if (x!=zero || ... || (flags&bit)) { body;
  return 1; } return 0;` — gives `c.eq.s f0,f1; bc1f→body; beqz→end`. The `&&`-early-
  return form gives the wrong `bnez` polarity.
- **VERIFY CALL ARITY first (diagnostic)**: a "swapped prologue saved-reg move" near-miss
  (99%+) is very often a WRONG call argument count, not a backend wall. Read the callee's
  prologue (how many of a0..a3 it consumes) and forward ALL the real params — when a
  function saves both params and its first call forwards both, mwcc emits `paddub s1,a0`
  before the jal and `paddub s0,a1` in the delay slot, matching CW. (Cracked func_001914A0.)
- **idiom-13 REFINED**: mwcc fills a conditional-branch delay slot with the first
  SPECULATABLE (pure-ALU: lui/li/addiu/sll/addu/mtc1-feeder) instruction from EITHER
  successor; loads/stores are never speculated. Matchable from C ONLY when every
  candidate first-instruction in both successors is a memory op off an ALREADY-LIVE base
  register (no separate `lui` exposed). Globals via lui/lo are NEVER matchable (volatile
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
- **mwcc-vs-CW branch lowering**: CW two-exit (two `jr ra`, useful delay slots); mwcc
  merges via `b`, uses `$at` compare + `paddub` zero. No known C lever. Park.
- **saved-reg-arg-in-jal-delay-slot**: CW puts `paddub aN,s0` (a saved-reg arg move) in
  a call's delay slot; mwcc hoists it out. Park.
- **FPU div/madd latency nops** and **dead-`paddub`/`b` coalescing**: backend. Park.
- **DENSE-SWITCH JUMP-TABLE DISPATCH (the jtbl wall — ~146 funcs, PROVEN unmatchable s84)**:
  any function whose target `.s` does `lui %hi(jtbl_XXXX); addiu %lo; sll idx,2; addu; lw;
  jr` (a real jr-table) is UNMATCHABLE as a C `switch`. Root cause: the original emitted ALL
  jump tables to a consolidated EXTERNAL rodata TU (0x0026xxxx–0x0027xxxx; `jtbl_XXXX` is an
  *undefined external* in the expected .o, defined in a splat data .s). A C `switch` makes
  mwcc emit its OWN LOCAL `@NN` table → (a) the post-RA scheduler freely reorders it (`lui`
  into the `beqz` delay slot, `addiu`/`sll` swapped), and (b) objdiff counts the local-vs-
  external reloc as a mismatch *even when the schedule is byte-identical* (func_001B9C10:
  schedule matched, still <100% purely on `@13` vs external `jtbl_0026E0E0`). EXHAUSTED s84:
  no -O/-opt/-sdatathreshold/-model/-gpopt lever (all illegal or no effect; `-sdatathreshold 8`
  is the right default — fixes an incidental 8-byte gp-rel s64 access, NOT the wall);
  ALL section/scheduling pragmas illegal in mwcc 2.3; `#pragma schedule off` reproduces the
  EXACT dispatch but is function-global (wrecks prologue → 68%); inline-asm `la jtbl,EXTERNAL`
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
  return a/256.0f;` → `cvt.s.w f1,f0; add.s f1,f1,f1; div.s f1,f1,f0`. CONSTRAINT: only ONE float
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
  `paddub v0,zero,zero` directly in the `bc1t` delay slot, landing on the shared `lq ra` epilogue —
  the target form. The `if (x <= K) return 0;` form instead gives `bc1f` + an extra `b` + shared
  paddub (the near-miss). Duplicate `return 0` at EACH nesting level to reproduce CW's two-exit /
  duplicated-`lq ra` epilogue. (Float-domain analog of the int work-as-if-TRUE-arm idiom.)
- **idiom-13 nop, RMW subset**: the conditional-branch delay-slot NOP is reproducible at zero regalloc
  cost ONLY when the success block's first instr is non-speculatable. A read-modify-write field write
  (`p->f |= 1` → `lb; ori; sb`) leads with a `lb`, giving `bc1f; nop; lb; ori; sb`. So this matches
  ONLY when the target's success block is genuinely RMW. For a CLEAN constant store (`p->f = 1` →
  `li; sb`) mwcc always fills the slot with the `li` — GENUINE WALL (the `int one=1;` hoist makes the
  nop but burns a saved reg + grows the frame; only use it if the target pays that same cost).
- **$at-vs-GPR compare, stored-boolean subset**: mwcc keeps a branch-feeding compare in a NAMED GPR
  (`slti vN,..; bnez vN`) only when the boolean is ALSO stored to memory: `c = v < N; p->flag = c;
  if (c) {...}` → `slti v1,v1,N; bnez v1; sw v1,...(delay)`. For a PURE branch (boolean never stored)
  there is NO lever — mwcc always uses $at. Secondary: cache an unsigned-byte field into an int
  before comparing (`int v=(u8)x; if (v < K)`) to flip mwcc's `sltiu` to the target's signed `slti`.

## RESIDUAL-WALL IDIOMS (s83 RE round 3 — 3 more cracked)
- **Field-address CSE hoist (anim state-machine family)**: a POINTER-TYPED LOCAL aliasing a field
  (`u8 *q = &p->f6; ... *q = *q+1;`) makes mwcc hoist `&p->f6` into a saved reg + keep the base
  separately = extra saved reg + bigger frame (the wall). CURE: access `p->field` DIRECTLY (member/
  index off the single base pointer) in every read AND write — mwcc copies the BASE into the saved
  reg (`paddub sN,base,zero`) and recomputes `off(sN)` inline (target form, smaller frame). An INT
  value-local (`int s=p->st; ...; p->st=s+1;`) is SAFE (does not hoist) — only a pointer alias does.
- **jal-delay-slot saved-reg copy (cracks "saved-reg-arg-in-jal-delay-slot")**: mwcc fills a call's
  delay slot with the LAST callee-saved param copy when (a) 2+ params live across the call, (b) the
  call forwards the ORIGINAL aN (not the saved copy), and (c) the call RESULT is DISCARDED/STORED
  (not kept in a saved reg — keeping it makes the s0 `sq` spill fill the slot instead). saved-reg
  NUMBER = first-use-after-call order; before-jal copy order is locked ascending-source-param (a
  residual if the target's order differs, e.g. func_001B1020 a3-then-a0).
- **Address-escaped loop counter (cracks counter inc/store/reload/compare scheduling)**: when the
  target reloads the counter from the stack each iter (`lw;addiu;sw;lw;slti;bnez`), make the counter
  memory-pinned by letting its ADDRESS ESCAPE once (`int i; h(&i);`). Use `do{ body; i++; }while(i<K);`
  (matches a fall-through init `sw zero,off(sp)`; a for/while top-test instead emits a leading `b`).
  A trailing body pointer-advance (`p+=K;`) lands in the bnez DELAY SLOT. `i<K` signed -> slti, unsigned
  -> sltiu. NON-lever: a struct-FIELD counter with a NOP branch slot (func_0014BB10) is idiom-13 — park.

## FP-LOAD-COLORING IDIOM (s83 RE round 4) — operand position
- **FP odd-companion coloring on a plain lwc1 LOAD = OPERAND POSITION** (generalizes idiom-1 beyond
  cvt.s.w producers). The LEFT (first) operand of a single-precision binop (sub.s/add.s/mul.s) whose
  result targets f12 colors to the ODD companion f1 (f3 for a nested pair); the RIGHT operand -> EVEN
  f0. So to match `lwc1 f1,off(p); lwc1 f0,off(q); sub.s f12,f1,f0`, put the value that must land in
  f1 on the LEFT of the binop: `fabsf(p->field - q->field)`. No materialized const / 2nd-float needed.
  Holds even when the loaded value is the syntactic right operand (rule is positional in the EMITTED
  binop). The li-hoist-above-store wall (func_00179680/001CB950) is CONFIRMED GENUINE — mwcc can't
  hoist a `li` past an unrelated store while keeping it in the same caller-saved reg (mutually
  exclusive live-range constraints); those stay asm-void.

## MWCC POST-RA SCHEDULER MODEL + idioms 12-18 (s83 RE — comprehensive)
### Conditional-branch delay-slot fill (refines/partly de-walls idiom-13)
mwcc fills a cond-branch (beq/bne/beqz/bnez/bc1f/bc1t/blez/bgtz) delay slot with the FIRST
EMITTED instr of a successor IFF it is a speculatable pure-ALU/immediate op (li, lui, addiu,
addu/subu, sll/sra/srl, mtc1-feeder, global/float-const address lui). LOADS (lw/lb/lwc1),
STORES (sw/sb/sh/swc1, incl `sw zero`), and CALLS (jal) are NEVER speculated -> slot stays NOP.
The decider is the FIRST EMITTED instr (after value materialization), not the C source order.
- **idiom-14 (store-of-LIVE-value = nop, the big de-wall)**: a clean store whose VALUE is already
  live needs no materializer, so the STORE is emitted first -> NOP at zero cost. `p->f = 0`,
  `p->f = <live param/saved reg>`, or `p->f = <precomputed local>` all give `beqz; nop; sw...`.
  Only a store whose value must be MATERIALIZED (`p->f = 3` -> `li;sb`) lets the `li` fill the slot
  = GENUINE wall. Also: if the success block genuinely begins with a load/store/call, write THAT
  memory op as the FIRST if-body statement -> nop (multi-stmt blocks fine; const stores emit after).
- Unconditional `b`/`j` ASYMMETRY: a `b`/`j` slot CAN take a STORE (and arg addiu). So a store in a
  b/j delay slot is normal+matchable; a store in a beqz/bc1f slot never happens.
- mwcc does NOT duplicate the slot filler at the target label, and NEVER emits an 8-byte
  branch-target alignment nop (that CW-only nop = genuine wall). The filler may come from either
  successor (whichever leads with a speculatable op); branch polarity is irrelevant.
- GENUINE (no lever): clean-CONSTANT-store idiom-13 (materialized value, no leading mem op);
  global-store-first still fills via its `lui at,hi`.

### List-scheduling / instruction order
- **idiom-15 (reload-vs-CSE across a call)**: a call is an aliasing barrier — a field read placed
  textually AFTER a call is never CSE'd with one before it. To match a reload-per-call target, keep
  the BASE ptr in a saved reg and re-read `base->field` AFTER each call. To match a kept value, read
  the field into a surviving local BEFORE the call (mwcc loads the value into a saved reg, keeps it).
- **idiom-16 (const-in-delay-slot)**: write a const store / const call-arg as the LAST statement
  before a call -> mwcc sinks the store/li into the j/jal delay slot and hoists the value
  materialization (li / lui / lui+ori, width-independent) to the earliest free slot. The address
  half of a global store (`lui at,%hi`) is formed per-store, not hoisted. For multi-const call args,
  the LAST arg's li/addiu fills the jal slot; earlier ones materialize before in arg order.
- **idiom-17 (paddub timing)**: with >=2 params live across a call, exactly ONE saved-reg param copy
  (`paddub sX,aY`) fills the following branch/jal delay slot, the rest hoist before it; saved-reg
  NUMBER follows first-use-after-call order. A single param forwarded as a tail-call arg needs no paddub.
- **idiom-18 (s64-param direct store)**: declare a 64-bit-stored param `long long` to get `sd sN,off`
  with no `dsll32/dsra32` sign-extension (an `int` widened to 64 adds the spurious extend).
- **idiom-19 (inverse-CSE / anti-frame-growth)**: when the TARGET recomputes a repeated subexpression
  at each use (e.g. two identical `subu s0,s6,s3` emissions) but mwcc CSEs it into a callee-saved reg —
  adding a save and GROWING the frame (the classic `0x90 -> 0xa0` tell) — do NOT share a temp: INLINE
  the expression literally at every use site, including inside the branch CONDITION that guards it.
  mwcc then recomputes it per-path, matching the target's separate emissions and keeping the frame
  small. Pairs with the no-prototype call trick (`int f();`) when the arms pass a varying arg count /
  carry a leftover param. (Cracked func_00203F40 s84: inlined `(a1-t1)` into its `if` condition + both
  use sites; was 99.87% with the shared temp, 100.0 inlined.)
- **idiom-20 (BRANCH-LIKELY dispatch — mwcc DOES emit beql/bnel/beqzl; NOT a wall)**: the s84
  "mwcc cannot generate branch-likely from C" parks were WRONG — 73 matched mwcc funcs contain
  branch-likely (e.g. src/func_0014E4F0.c switch-dispatch `beql a3,zero`+`addiu v0,a3,1`;
  src/func_00154F00.c loop `beql v0,zero`+counter `addiu s1,s1,1`; src/func_001AF7C0.c early-return
  `if(g>0){...}return 0;` -> `blezl`+`paddub v0,zero,zero`). mwcc emits a LIKELY branch (nullifies its
  delay slot when NOT taken) whenever it can fill that slot with a SPECULATABLE pure-ALU op from the
  TAKEN (branch-target) path. The C must make that op the FIRST emitted op of the taken path:
  - SWITCH state-machine: `switch(st)` on a LOCAL `st = *(unsigned char*)(e+6)`; case labels ASCENDING
    0,1,2,… (mwcc reverses to descending compare chain 2,1,0 = CW). Write the state advance as
    `*(unsigned char*)(e+6) = st + 1;` USING THE SWITCH LOCAL `st` (NOT `(*(e+6))++`, NOT
    `*(e+6)=*(e+6)+1` which RELOADS) -> mwcc drops `addiu v0,st,1` into the dispatch branch slot via
    beql/beqzl. Keep `st` live. (Verified synthetically + on matched siblings.)
  - EARLY-RETURN GUARD: `if(cond){body} return X;` with X simple -> `<inv-cond>l epilogue` + X-setup slot.
  - LOOP counter: the `i++` fills the loop-test likely-branch slot.
  - PRINCIPLE: get the body byte-identical; if the SOLE residual is "target beql/beqzl where mine has
    plain beq/bne (+nop)", read the target .s to see which op sits in the likely slot, then make THAT op
    the natural first statement of the taken path. DEFAULT: branch-likely IS matchable — don't park it.
- **idiom-21 (FLOAT COMPOUND-ASSIGN picks the add.s operand order)**: `x = x + y` makes mwcc load the
  LHS first into `$f2` and emit `add.s $f0,$f2,$f0`; `x += y` makes it load the RHS first into `$f1`
  and emit `add.s $f0,$f0,$f1` — the CW form. Whenever the sole residual on a float accumulate is the
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
  - `x != 0.0f` / `0.0f != x` / `!(x == 0.0f)` / `x == 0.0f` → `mtc1 zero,$f1` ; `lwc1 $f0` ;
    `c.eq.s $f1,$f0` (zero is `fs`).
  - `float z; z = 0.0f; ... x != z` (zero assigned in a SEPARATE statement) → `mtc1 zero,$f0` ;
    `lwc1 $f1` — the loaded value moves to the second FPR.
  - `x != (z = 0.0f)` (assignment INLINE in the condition) → flips both dimensions, giving the
    CodeWarrior form `lwc1 $f0` ; `mtc1 zero,$f1` ; `c.eq.s $f0,$f1`.
  Reach for this whenever the sole residual is an `mtc1`/`lwc1` ordering or a `c.eq.s` operand swap
  around a compare with zero. (Cracked func_0021F330 s85 — analytically, after ~thousands of
  permuter iterations had failed on the same function; the permuter cannot reach it because mwcc
  folds the temp back before scheduling.)
- **idiom-24 (FP-ARG-ZERO-STAGING — the f13-before-f12 wall, CRACKED)**: when the target emits the
  trailing `0.0f` argument's `mtc1 zero,$f13` BEFORE `mtc1 <r>,$f12`, stage the zero through an
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
  `if (x)` / `if (!x)` on a float lvalue emits `mtc1 zero,$f0 ; c.eq.s $f1,$f0` (the VALUE as `fs`).
  Every explicit spelling — `x != 0.0f`, `0.0f != x`, `!(x == 0.0f)`, `x != 0`, `x != (float)0`, and
  a hoisted `float fv = x; fv != 0.0f` — instead emits `c.eq.s $f0,$f1` (zero as `fs`). Note this is
  a *different* dimension from idiom-23: idiom-23's `x != (z = 0.0f)` moves the compare-operand order
  but also swaps the FPRs, trading one two-instruction diff for another; truthiness moves the operand
  order alone. (Cracked func_002236F0 s85 via a 49-cell variant sweep.)
- **idiom-26 (COMPOUND-ASSIGN steers add.s operand order)**: `*p += -0.2f;` emits
  `add.s $f0,$f1,$f0` (loaded value as `fs`); the expanded `*p = *p + -0.2f;` and `*p = -0.2f + *p;`
  both emit `add.s $f0,$f0,$f1`. A `float cv = -0.2f;` temp also gives the correct order. Beware
  `*p = *p - 0.2f;` — that emits `sub.s` and is a different instruction. (Generalizes idiom-21.)
- GENUINE (no lever): LI-HOIST — a plain-literal `li`/`lui` hoisted above an unrelated store while
  kept in the immediate-scratch $v1. $v1 is rewritten per literal; a value living across a store gets
  a DISTINCT value reg (a0/a1), so "hoisted order" and "$v1 coloring" are mutually exclusive. (A
  surviving const-local reproduces the ORDER but colors to a1 = the func_00179680/001CB950 residual.)

### idioms 12-13
- **idiom-12 (FP-param companion pairing = positional)**: NOT a wall. N single-`float` params live
  across a call are saved TOP-DOWN by decl order: param k (incoming f(12+k)) -> f(20 + (N-1) - k).
  N=2 -> f12:f21(odd), f13:f20(even). Just write natural code in true param order; inline `0.0f`
  literal in a compare for `c.eq.s f0,f21` (a `float zero` local reverses it).
- **idiom-13b (dead-const re-materialization)**: when mwcc speculates a const into a cond-branch
  delay slot it ALSO re-emits it DEAD at the target label (no cross-branch CSE of the const). Force
  it with: an if/else-if chain where every arm passes the SAME large lui+ori const to a call (dead
  lui); or two stores of the SAME nonzero const straddling a branch whose delay slot has NO real
  store available (dead li). A store schedulable into the branch slot OUTRANKS const speculation.
