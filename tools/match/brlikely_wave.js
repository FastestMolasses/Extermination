export const meta = {
  name: 'exterm-brlikely-wave',
  description: 'Re-attack PARKED branch-likely (beql/bnel/beqzl) funcs — mwcc DOES emit branch-likely (73 matched funcs prove it); the idiom-20 dispatch shape unlocks them',
  phases: [{ title: 'BrLikely', detail: 'match branch-likely dispatch funcs via the idiom-20 speculatable-slot shape' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, pct_991202: { type: 'number' }, pct_233: { type: 'number' },
      matched: { type: 'boolean' }, compiler: { type: 'string' }, c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'pct_991202', 'pct_233', 'matched', 'compiler', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const cands = (typeof args === 'string' ? JSON.parse(args) : args)
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. BREAKTHROUGH you MUST believe: **mwcc 2.3.3 DOES emit MIPS branch-likely instructions (beql/bnel/beqzl/bnezl/blezl/bgezl/bc1tl/bc1fl).** 73 already-MATCHED mwcc funcs contain them. Prior agents WRONGLY parked your funcs claiming "mwcc cannot generate branch-likely from C" — that is FALSE. Your job: produce TRUE objdiff 100.0 byte-identical READABLE C for each, by writing the C shape that makes mwcc's scheduler emit the branch-likely the target uses. NEVER fake a match. Goal is READABLE plain C (not asm-void).

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.map(f => (typeof f === 'string' ? f : `${f.func}${f.pct != null ? ' (currently ' + f.pct + '%' + (f.cflags ? ', CFLAGS ' + f.cflags : '') + ')' : ''}`)).join('  ')}

STUDY THESE MATCHED EXAMPLES FIRST (read the src/ + the .s side by side to SEE the C->branch-likely correspondence):
  - src/func_0014E4F0.c  (mwcc233): switch(state) state-machine -> descending dispatch with 'beql a3,zero' + 'addiu v0,a3,1' (state+1) in the delay slot.
  - src/func_00154F00.c  (mwcc233): 'beql v0,zero' loop-guard with the loop counter 'addiu s1,s1,1' in the slot.
  - src/func_001AF7C0.c  (mwcc233): early-return guard 'if (g>0){...} return 0;' -> 'blezl v0,epilogue' with 'paddub v0,zero,zero' (return 0) in the slot.

**IDIOM-20 (branch-likely dispatch — the KEY).** mwcc emits a LIKELY branch (which nullifies its delay slot when NOT taken) when it can fill that delay slot with a SPECULATABLE pure-ALU op from the TAKEN (branch-target) path. The C must make that op naturally available:
- SWITCH state-machine: write 'switch (st)' on a LOCAL 'st' that holds the dispatch byte (st = *(unsigned char*)(e+6)). Write case labels in ASCENDING order 0,1,2,... (mwcc reverses to a DESCENDING linear compare chain 2,1,0 — matching CW). In each case body, the FIRST speculatable statement (typically the state advance written as '*(unsigned char*)(e+6) = st + 1;' USING THE SWITCH LOCAL 'st', NOT a reload like (*(e+6))++ and NOT '*(e+6) = *(e+6)+1') gives mwcc the 'addiu v0,st,1' to drop into the dispatch branch's delay slot via beql/beqzl. KEEP 'st' live; do not reload it.
- EARLY-RETURN GUARD: 'if (cond) { body } return X;' with X a simple value (0 / a field / a param) -> mwcc emits '<inverted-cond>l epilogue' with X's setup in the slot. 'if (g > 0) {...} return 0;' -> blezl + the zero-materialize in the slot.
- LOOP guard with a counter: the loop counter increment (addiu sN,sN,1) fills the loop-test likely-branch slot (see func_00154F00).
- && short-circuit: 'if (a==K1 && b==K2) ...' -> the second test becomes bnel/beql with the next speculatable op in the slot.
The PRINCIPLE: get the BODY byte-identical first; if the SOLE residual is "target has beql/beqzl/bnel where mine has plain beq/bne (+ maybe a nop)", re-shape the guarding C so the slot-filler op the target uses is the FIRST emitted op of the taken path. Re-read the target .s to see exactly which op sits in the likely-branch's delay slot, then make THAT op the natural first statement.

RECIPE PER FUNC:
1. DECODE to readable PLAIN C: m2c base — .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py — rewrite in committed-src convention (PLAIN C, externs, *(type*)(p+off), explicit casts; NO prelude/macros). The existing src/<F>.c (stub) + any prior MATCH note document the body; the body is usually already ~85-98% right — the residual is the dispatch shape.
2. Build build/agent_${id}/src/<F>.c, leading "// CFLAGS: -O4,p -sdatathreshold 0" (use 4/8 if it uses gp-rel/small globals).
3. Compile BOTH builds + assemble expected + objdiff (HOST, ONE container call):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/o991/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/agent_${id}/o233/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o233/<F>.o <F> -o - --format json  (and o991). Read each instructions[].diff_kind to LOCATE the beql/beqzl residual + which op the target puts in its delay slot.
4. Iterate the C dispatch/guard shape (cap ~12 attempts) until ONE build hits 100.0. Prefer mwcc233; if 991202 also 100.0 prefer "mwcc". Branch-likely is reproducible — keep reshaping until the likely-branch + its slot-filler match.
5. GENUINELY un-crackable (rare here — park only with proof): if after getting body-identical and trying the idiom-20 shapes mwcc still emits plain beq/bne, OR the residual is a DIFFERENT wall (jr-table, VU0, FP-coloring), report best % + precise wall. But DEFAULT ASSUMPTION: branch-likely IS matchable — exhaust the slot-shaping before parking.

On a TRUE 100.0: matched=true, compiler ("mwcc"/"mwcc233"), c_source = FULL committed-ready file (leading "// COMPILER: mwcc233" ONLY if 233, then "// CFLAGS: ...", a concise semantics comment, then the plain-C body). pct_991202 + pct_233 = both measured.
RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct_991202, pct_233, matched, compiler, c_source (full file ONLY if matched), wall (if not matched).`

phase('BrLikely')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`bl_${i}`, funcs), { label: `brlikely:${i}`, phase: 'BrLikely', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
