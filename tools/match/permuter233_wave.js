export const meta = {
  name: 'exterm-permuter233-wave',
  description: 'Tuned decomp-permuter (2.3.3, regalloc-weighted) on pure register-permutation near-misses (>=97%, one reg-swap from 100%)',
  phases: [{ title: 'Permute', detail: 'decode near-miss to base.c, run tuned 2.3.3 permuter to objdiff score 0' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: { func: { type: 'string' }, matched: { type: 'boolean' }, best_score: { type: 'number' },
      c_source: { type: 'string' }, notes: { type: 'string' } },
    required: ['func', 'matched', 'best_score', 'c_source', 'notes'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. These funcs are PURE register-allocation-permutation near-misses: the function BODY already byte-matches at 97-99.8% on mwcc 2.3.3, and the ONLY residual is a saved-register numbering / coloring permutation (e.g. target s1/s0/s2 vs mwcc s0/s1/s2). The decomp-permuter is built for exactly this. Your job: get each to TRUE objdiff score 0 (100.0) by running the TUNED 2.3.3 permuter. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}
YOUR FUNCS: ${funcs.map(f => (typeof f === 'string' ? f : `${f.func}${f.pct != null ? ' (currently ' + f.pct + '%' + (f.cflags ? ', CFLAGS ' + f.cflags : '') + ')' : ''}`)).join('  ')}
READ FIRST: docs/fanout/PERMUTER_GUIDE.md (workflow) and the prior MATCH-ATTEMPT comments in each src/<F>.c / earlier wall notes (they describe the exact near-miss body + which sdatathreshold + the register permutation).

PER-FUNCTION:
1. DECODE the near-miss to PLAIN, pycparser-parseable C (this is the permuter's base, NOT a committed file): m2c base (.venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py), then write the readable body. MUST be pycparser-clean: externs + the one function, NO "// CFLAGS:" line, NO __attribute__, NO inline asm{}, NO prelude include, plain int/char/float/short + casts. First CONFIRM your decode is a true high-% near-miss: compile with 2.3.3 and objdiff (qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c -O4,p -sdatathreshold <SDT> -o /tmp/<F>.o build/agent_${id}/<F>.c ; tools/bin/objdiff-cli diff -1 <expected> -2 /tmp/<F>.o <F> ...). If it's <97%, fix the body FIRST (the permuter only fixes register/schedule perms, not a wrong body). Pick the SDT that maximizes the score (0/4/8).
2. Write that C to tools/permuter/work/<F>/base.c.
3. Run the tuned 2.3.3 permuter (writes a 2.3.3 compile.sh + regalloc-weighted settings.toml, then sweeps) inside the exterm-permuter image:
   container run --rm -v "$PWD:/work" -w /work exterm-permuter sh -c 'bash tools/permuter/run_func233.sh <F> <SDT> 1500 2'
   (5th arg overrides the compiler if a func is actually best on 991202: ...run_func233.sh <F> <SDT> 1500 2 tools/mwccps2/mwccmips.exe)
4. A score-0 hit lands at tools/permuter/work/<F>/output-0-*/source.c. If it plateaus >0, SEED-AND-DEEPEN: copy the best output-<n>-*/source.c back to base.c and re-run (1-2 reseeds). ONE permuter per work dir.
5. On score 0: read tools/permuter/work/<F>/output-0-*/source.c, VERIFY it independently (compile with 2.3.3 + objdiff == 100.0), and report matched=true with c_source = the FULL committed-ready file: leading "// COMPILER: mwcc233" line, then "// CFLAGS: -O4,p -sdatathreshold <SDT>", a one-line semantics comment, then the permuter's source.c body (cleaned of any permuter scaffolding/markers).

RULES: edit ONLY build/agent_${id}/ and tools/permuter/work/<your funcs>/ ; never canonical build/obj|expected, src/, objdiff.json, other tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, matched (true ONLY at verified objdiff 100.0), best_score (permuter score; 0=match, else lowest reached), c_source (full file ONLY if matched), notes (what the permuter found / why it plateaued).`

phase('Permute')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`pm233_${i}`, funcs), { label: `permute233:${i}`, phase: 'Permute', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
