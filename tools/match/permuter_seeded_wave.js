export const meta = {
  name: 'exterm-permuter-seeded-wave',
  description: 'Seeded tuned 2.3.3 decomp-permuter on body-identical near-misses: each agent gets the exact prior residual + SDT, decodes straight to it, then permutes to objdiff 100.0',
  phases: [{ title: 'Permute', detail: 'decode to known near-miss base.c, run tuned 2.3.3 permuter to score 0' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: { func: { type: 'string' }, matched: { type: 'boolean' }, best_score: { type: 'number' },
      compiler: { type: 'string' }, c_source: { type: 'string' }, notes: { type: 'string' } },
    required: ['func', 'matched', 'best_score', 'compiler', 'c_source', 'notes'],
  } } }, required: ['results'],
}
const cands = (typeof args === 'string' ? JSON.parse(args) : args)
// BATCH=1: each func gets a full agent time budget for permute + reseeds (BATCH=2 starved
// the second func's sweep in s84 wave1). Cap is 8 concurrent agents -> <=8 funcs/wave.
const BATCH = 1
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, items) => `Matching-decomp subagent for PS2 game Extermination. Your funcs are body-byte-identical near-misses on mwcc 2.3.3 (>=99% typical): the C BODY is already correct, the SOLE residual is a register-allocation / FP-coloring / instruction-scheduling / commutative-operand-canonicalization PERMUTATION that NO source idiom flips (prior agents exhausted the idioms). The decomp-permuter is the ONLY remaining lever: it randomly perturbs the C structure (temp-for-expr, decl/stmt reorder, split-assignment, self-assignment) to shift the allocator/scheduler state until the emit order matches. Get each to TRUE objdiff score 0 (100.0). NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}
READ FIRST: docs/fanout/PERMUTER_GUIDE.md (permuter workflow).

YOUR FUNCS (each with prior measured pct, the recommended -sdatathreshold, and the EXACT residual):
${items.map(o => `  ### ${o.func}  (prior 233=${o.pct}%, use -sdatathreshold ${o.sdt})\n  RESIDUAL: ${o.wall}`).join('\n')}

PER-FUNCTION:
1. DECODE to the known near-miss as PLAIN, pycparser-parseable C (this is the permuter's base.c, NOT a committed file): m2c base (.venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py), then rewrite the readable body to REPRODUCE the residual described above (the body should be byte-identical EXCEPT that one permutation). MUST be pycparser-clean: externs + the one function, NO "// CFLAGS:" line, NO __attribute__, NO inline asm{}, NO prelude include, plain int/char/float/short + casts.
2. CONFIRM the near-miss: compile with 2.3.3 at the recommended SDT and objdiff:
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o /tmp/${id}_<F>_exp.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c -O4,p -sdatathreshold <SDT> -o /tmp/${id}_<F>.o build/agent_${id}/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   tools/bin/objdiff-cli diff -1 /tmp/${id}_<F>_exp.o -2 /tmp/${id}_<F>.o <F> -o - --format json  -> confirm match_percent matches the prior pct (>=97, ideally the residual is exactly as described). If your decode is WORSE, fix the body FIRST (the permuter only fixes the register/schedule perm, not a wrong body).
3. Write that C to tools/permuter/work/<F>/base.c, then run the TUNED 2.3.3 permuter (writes regalloc-weighted settings.toml + 2.3.3 compile.sh, then sweeps):
   container run --rm -v "$PWD:/work" -w /work exterm-permuter sh -c 'bash tools/permuter/run_func233.sh <F> <SDT> 1500 2'
   (If the prior note says 991202 is actually closer, override the compiler: append ' tools/mwccps2/mwccmips.exe' as the 5th arg.)
4. SEED-AND-DEEPEN if it plateaus >0: copy the best tools/permuter/work/<F>/output-<n>-*/source.c back to base.c and re-run (up to 2 reseeds). The permuter prints the lowest score reached.
5. A score-0 hit lands at tools/permuter/work/<F>/output-0-*/source.c. VERIFY independently (compile that source with 2.3.3 at SDT + objdiff == 100.0). Only then report matched=true with c_source = the FULL committed-ready file: leading "// COMPILER: mwcc233" line, then "// CFLAGS: -O4,p -sdatathreshold <SDT>", a one-line semantics comment, then the permuter's source.c body cleaned of any permuter scaffolding/markers (no pragma/comment noise; readable plain C).

RULES: edit ONLY build/agent_${id}/ and tools/permuter/work/<your funcs>/ ; never canonical build/obj|expected, src/, objdiff.json, other tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, matched (true ONLY at verified objdiff 100.0), best_score (0=match, else lowest reached), compiler ("mwcc233" or "mwcc"), c_source (full file ONLY if matched), notes (what the permuter found / why it plateaued — be specific about the residual that survived).`

phase('Permute')
const results = await parallel(batches.map((items, i) => () =>
  agent(PROTO(`ps_${i}`, items), { label: `permute:${i}`, phase: 'Permute', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
