export const meta = {
  name: 'exterm-mwcc233-wave',
  description: 'Convert clean-store idiom-13 walls to READABLE byte-matched C via mwcc 2.3.3 (fixes the delay-slot nop 991202 cannot)',
  phases: [{ title: 'Match233', detail: 'decode to readable C, match with mwcc 2.3.3 (or 991202)' }],
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
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. PROVEN s84: mwcc 2.3.3 (mwcps2-2.3.3-000906) byte-matches the clean-store idiom-13 delay-slot-nop case that the pinned 991202 build CANNOT (it fills the beqz delay slot with the safe li/lui). Your funcs are walled (stub or asm-void) on that wall. Produce TRUE objdiff 100.0 byte-identical READABLE C — the goal is readable C (so it must NOT be asm-void; write plain C with normal control flow), matched with whichever mwcc build hits 100.0. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

THE PROVEN RECIPE (worked on func_001872C0):
1. DECODE to readable PLAIN C: m2c base — .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py — then rewrite in the COMMITTED-SRC convention: NO prelude/typedefs/M2C_ macros; use PLAIN C (int/char/unsigned char/float, explicit casts, \`extern\` decls, struct access as \`*(type *)(p + off)\` or array index). Apply MATCHING_GUIDE idioms to fix structure (struct field types, signatures, float vs int, the if(p){...}return 0; shape). Existing stub/asm-void file in src/<F>.c often documents SEMANTICS and the near-miss C — read it and PRESERVE the semantic comments.
2. Build .c in build/agent_${id}/src/<F>.c: leading "// CFLAGS: -O4,p -sdatathreshold 0" (or 4/8 if the func uses small/gp-rel globals — match what maximizes the score). PLAIN C body. (No "// COMPILER:" line during testing.)
3. Compile with BOTH builds + assemble expected + objdiff (HOST):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/o991/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/agent_${id}/o233/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff each: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o991/<F>.o <F> -o - --format json   (and o233). Read match_percent + instructions[].diff_kind.
4. Iterate the C until ONE build hits 100.0 (cap ~10 attempts). Read the per-instruction delta: get the BODY byte-identical first (idioms); if the SOLE residual is the clean-store delay-slot nop (991202 fills it, target+233 leave it), 2.3.3 should already be 100.0. If 991202 ALSO hits 100.0, prefer 991202 (use compiler="mwcc"). If only 233 hits 100.0, compiler="mwcc233".
5. KNOWN WALLS 2.3.3 does NOT fix (park, report wall): jr-table dispatch (PROVEN dead), CW branch-target alignment nop, mwcc-vs-CW branch lowering (dead trailing return-0 block), FPU-MAC, regalloc permutation, ee-gcc codegen. If after getting the body identical the residual is one of THESE (not the clean-store nop), 2.3.3 won't help — park.

On a 100.0: report matched=true, compiler ("mwcc" or "mwcc233"), c_source = the FULL committed-ready file: a leading "// COMPILER: mwcc233" line (ONLY if compiler==mwcc233), then "// CFLAGS: ...", then any preserved SEMANTIC comments, then the plain-C body. pct_991202 + pct_233 = the two measured percentages.

DO NOT run the decomp-permuter — it is slow and the orchestrator runs a dedicated permuter pass on parked regalloc/scheduling near-misses. If a func walls on a register-allocation-ORDER / scheduling / param-save-order / FP-coloring permutation (not the clean-store nop), get the best readable C, report best %, and PARK it — do not iterate past ~8 attempts.
RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct_991202, pct_233, matched, compiler, c_source (full file ONLY if matched), wall (if not matched).`

phase('Match233')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`m233_${i}`, funcs), { label: `mwcc233:${i}`, phase: 'Match233', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
