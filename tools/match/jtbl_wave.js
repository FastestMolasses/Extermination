export const meta = {
  name: 'exterm-jtbl-wave',
  description: 'Match jump-table dispatcher functions (newly decodable via jtbl_prep resymbolization)',
  phases: [{ title: 'JtblMatch', detail: 'decode jtbl dispatchers from jtbl-aware m2c base, push to objdiff 100%' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, baseline_pct: { type: 'number' }, final_pct: { type: 'number' },
      matched: { type: 'boolean' }, c_source: { type: 'string' }, idiomUsed: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'baseline_pct', 'final_pct', 'matched', 'c_source', 'idiomUsed', 'wall'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 4
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. These functions are JUMP-TABLE DISPATCHERS (they switch on a field via a jtbl_XXXX jump table). They were previously un-decodable because splat stored the jump tables as raw \`.word 0xADDR\` blobs; we built tools/match/jtbl_prep.py which re-symbolizes them so m2c can now decode the switch. Your job: turn the m2c base into TRUE objdiff 100% byte-identical C. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (use ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

=== STEP 0 — GET THE JTBL-AWARE m2c BASE (critical, do this FIRST per func) ===
Run jtbl_prep to resymbolize the jump table, then feed BOTH files to m2c:
  PAIR=$(.venv/bin/python3 tools/match/jtbl_prep.py <F>)          # prints two paths: "<F>.s <F>.jtbl.s"
  .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax \${=PAIR} 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py
  (zsh: use \${=PAIR} to word-split; bash: \$PAIR is fine. If jtbl_prep prints NOJTBL/ERROR, fall back to plain m2c on build/asm/matchings/main/code/<F>.s)
The decoded output will contain a real \`switch (x) { case 0: ... }\`. If m2c instead leaves a raw \`*((idx*4)+&jtbl_XXXX)\` expression, recover the switch BY HAND from the target .s: the dispatch is \`lbu idx; sltiu \$at,idx,N; ...; lw \$v0,jtbl; jr \$v0\`; read the N jtbl entries (jtbl_prep printed the .jtbl.s with the \`.word .L<addr>\` targets in case order) and write \`switch(idx){case 0..N-1}\`.

=== SWITCH / JUMP-TABLE IDIOMS (apply these first — they govern dispatcher codegen) ===
A) ASCENDING CASE ORDER (idiom-3): write cases in ASCENDING source order (case 0; case 1; ... case N) using a REAL \`switch\`, NOT if-else-if. mwcc tests the compare-chain in REVERSE of source order, matching CW's jr-table dispatch. A default that falls through goes last.
B) PER-CASE beql vs beq (idiom-13/14): a case dispatch slot gets beql ONLY if that case-body's FIRST emitted instr is a speculatable pure-ALU op (li/lui/addiu/addu/sll/sra/mtc1-feeder). A case whose body leads with a load/store/call -> beq+nop. Order statements so the first instr matches the target's slot.
C) idiom-14 STORE-OF-LIVE-VALUE de-wall: a clean store of an already-live value (\`p->f=0\`, \`p->f=<param>\`, \`p->f=<live local>\`) leaves a cond-branch delay slot a NOP at zero cost (beqz;nop;sw). Only a MATERIALIZED-const store (\`p->f=3\`->li;sb) fills the slot = genuine wall.
D) JTBL ENTRY VALUE/ORDER: do not reorder cases to "optimize"; the jr-table is positional — case k must be the k-th entry. Keep the exact case count (including empty/fallthrough cases) the target dispatches.

=== FULL IDIOM CATALOGUE (apply whichever matches the per-instruction delta) ===
1) FP odd/even coloring: a single-prec producer (cvt.s.w/lwc1/mtc1) colors dest to the ODD companion (f1) when it's the FIRST/longer-lived operand of a float binop. Chain conversion into a binop. Only ONE float const live at a time.
2) float-arg prototype hygiene: declare EVERY called function's FPU args as single \`float\`, exact arity, no K&R/unprototyped extern (promotes float->double->GPR via dsll32), no stray double.
4) saved-reg direction: raw params used across a call -> mwcc assigns saved regs TOP-DOWN (3 params a0=s2,a1=s1,a2=s0). Copy each param into a surviving local (used in arithmetic) -> BOTTOM-UP (a0=s0..). Match target polarity.
5) float early-return-0 two-exit: write float early \`return 0\` as the FALL-THROUGH after a positive-condition if; duplicate \`return 0\` at each nesting level.
6/13) RMW fills delay slot only for read-modify-write (\`p->f|=1\`->lb;ori;sb); clean const store always fills slot = wall.
7) \$at compare: keep a branch-feeding compare in a NAMED gpr only if the boolean is ALSO stored (\`c=v<N; p->flag=c; if(c)\`). Pure branch = \$at, no lever. Cache unsigned-byte field into int before compare for signed slti.
8) field-address CSE: access \`p->field\` DIRECTLY in every read+write (no pointer-typed local alias \`u8 *q=&p->f\`) so mwcc keeps only the base in a saved reg.
9) jal-delay saved-reg copy: mwcc fills a call's delay slot with the LAST callee-saved param copy when 2+ params live across the call and result is discarded/stored. saved-reg number = first-use-after-call order.
10) address-escaped loop counter: if target reloads counter from stack each iter, escape its address once (\`int i; h(&i);\`), use \`do{...i++;}while(i<K);\`.
11) FP-load coloring = operand position: LEFT operand of single-prec binop -> ODD f1/f3; RIGHT -> EVEN f0. Put the f1-value on the LEFT.
12) FP-param pairing POSITIONAL: N float params live across a call save TOP-DOWN, param k -> f(20+(N-1)-k). Inline \`0.0f\` literal for c.eq.s f0,f21.
13b) dead-const re-materialization: force a dead-dup via if/else-if arms all passing the SAME large lui+ori const to a call, or two stores of the same nonzero const straddling a branch with no store for the slot.
15) reload vs CSE across a call: read \`base->field\` AFTER the call for a fresh reload-per-call; into a surviving local BEFORE for a kept value.
16) const-in-delay-slot: write a const store / last const call-arg as the LAST stmt before a call -> sinks into the j/jal slot, materialization hoisted early.
17) paddub timing: with >=2 params live across a call, ONE saved-reg copy fills the following branch/jal slot; rest hoist before. saved-reg number = first-use-after-call order.
18) s64-param store: declare a 64-bit-stored param \`long long\` for \`sd sN,off\` (no dsll32/dsra32).
(Full detail in docs/fanout/MATCHING_GUIDE.md "MWCC POST-RA SCHEDULER MODEL".)

PER-FUNCTION LOOP (S=build/agent_${id}):
1. Get jtbl-aware m2c base (STEP 0). Decode the switch + body.
2. Build .c: first line "// CFLAGS: -O4,p -sdatathreshold 0" then the contents of tools/m2c/exterm_prelude.h then your decoded body. Use -sdatathreshold 4 if the func uses %gp_rel small globals.
3. Compile + diff (the jtbl reloc is external; objdiff compares it symbolically — the jr-table itself lives in another .s, you don't emit it):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/expected/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/obj/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Then diff on HOST: tools/bin/objdiff-cli diff -1 build/agent_${id}/expected/<F>.o -2 build/agent_${id}/obj/<F>.o <F> -o - --format json   (read match_percent + per-instruction deltas).
4. Iterate to 100.0 (cap ~12 attempts — these have real switch structure, push hard). Read the per-instruction delta each round to pick the idiom.
RULES: touch ONLY build/agent_${id}/ ; never canonical build/obj|expected, src/, objdiff.json; never run build.py/verify_all or git. Do not paste original disc disassembly to any external context.
Report per func: func, baseline_pct (jtbl-aware m2c-only baseline if known else 0), final_pct, matched (true ONLY at verified objdiff 100.0), c_source (FULL file incl // CFLAGS line, ONLY if matched), idiomUsed, wall (if not matched, name the precise per-instruction wall).`

phase('JtblMatch')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`jt${i}`, funcs), { label: `jtbl:jt${i}`, phase: 'JtblMatch', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
