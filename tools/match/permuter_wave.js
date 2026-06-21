export const meta = {
  name: 'exterm-permuter-wave',
  description: 'Brute-force the regalloc/scheduling near-misses (>=93% stubs) with decomp-permuter — the last unexhausted matching lever',
  phases: [{ title: 'Permute', detail: 'decode to near-miss C, triage wall, run permuter to objdiff score 0' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, baseline_pct: { type: 'number' }, best_score: { type: 'number' },
      matched: { type: 'boolean' }, c_source: { type: 'string' }, wall: { type: 'string' }, permuted: { type: 'boolean' },
    }, required: ['func', 'baseline_pct', 'best_score', 'matched', 'c_source', 'wall', 'permuted'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 2
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. These are >=93% near-miss STUBS where the function BODY largely matches. Your job: re-derive a near-matching C, then BRUTE-FORCE the last register/scheduling delta with the decomp-permuter to TRUE objdiff score 0 (100.0). NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}
YOUR FUNCS: ${funcs.join(' ')}
READ FIRST: docs/fanout/PERMUTER_GUIDE.md (the exact workflow) and docs/fanout/MATCHING_GUIDE.md (idioms + KNOWN WALLS).

PER-FUNCTION:
1. DECODE to near-miss C: m2c base (.venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py), apply MATCHING_GUIDE idioms to reach >=90%. Verify by compiling in a scratch dir (build/agent_${id}/) + objdiff (use -O4,p -sdatathreshold 8, or 0/4 per the func's small-global use).
2. TRIAGE THE WALL from the per-instruction diff. The permuter ONLY helps register-allocation-ORDER and instruction-ARRANGEMENT/scheduling walls (body matches, only register choice or order differs). It does NOT help: instruction-SELECTION (FMA/madd chains, bnel branch-likely, sd-vs-sq), CW branch-target alignment nop, a DEAD trailing return-0 block mwcc eliminates, clean-const-store idiom-13 delay-slot fill, ee-gcc-codegen, or the jr-table dispatch wall (PROVEN dead s84). If the wall is one of THOSE, set permuted=false, report the wall, and SKIP the permuter (do not waste it).
3. If it IS a regalloc/scheduling wall: write the near-miss C to tools/permuter/work/<F>/base.c as PLAIN C — externs + the one function, NO "// CFLAGS:" line, pycparser-parseable (no __attribute__, no asm{}, plain casts). Then run the permuter in its OWN container image (exterm-permuter, NOT exterm-toolchain):
     container run --rm -v "$PWD:/work" -w /work exterm-permuter sh -c 'tools/permuter/run_func.sh <F> <SDT> 1200 2'
   (SDT = sdatathreshold: 0 default, 4/8 if the func uses small/gp-rel globals — match what gave your best baseline.) The base score prints; "found new best score!" = improving; score 0 writes tools/permuter/work/<F>/output-0-*/source.c.
4. SEED-AND-DEEPEN if it plateaus >0: copy the best tools/permuter/work/<F>/output-<n>-1/source.c back to base.c and re-run (1-2 reseeds).
5. On score 0: read tools/permuter/work/<F>/output-0-*/source.c — that IS the match. Report matched=true with c_source = that source PLUS a leading "// CFLAGS: -O4,p -sdatathreshold <SDT>" line and the exterm prelude prepended (so it compiles standalone). best_score=0.

RULES: edit ONLY build/agent_${id}/ and tools/permuter/work/<your funcs>/ ; never canonical build/obj|expected, src/, objdiff.json, FUNCTIONS.csv, or other tools/. Never run build.py/verify_all/git. ONE permuter per work dir. No disc disasm to external context.
Report per func: func, baseline_pct, permuted (did you run the permuter?), best_score (permuter score; lower=better, 0=match; use -1 if not permuted), matched (true ONLY at verified score 0), c_source (full standalone file ONLY if matched), wall (precise wall if not matched).`

phase('Permute')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`pm${i}`, funcs), { label: `permute:pm${i}`, phase: 'Permute', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
