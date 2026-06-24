export const meta = {
  name: 'exterm-player-wave',
  description: 'Decode player gameplay functions (movement/health/combat) to READABLE byte-matched C — the form the port effort needs',
  phases: [{ title: 'PlayerMatch', detail: 'decode + idiom-finish + dual-compile (991202/2.3.3) to objdiff 100%' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, role: { type: 'string' }, pct_991202: { type: 'number' }, pct_233: { type: 'number' },
      matched: { type: 'boolean' }, compiler: { type: 'string' }, c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'role', 'pct_991202', 'pct_233', 'matched', 'compiler', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args  // [{func, role}]
const BATCH = 2
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, items) => `Matching-decomp subagent for PS2 game Extermination. GOAL: turn PLAYER GAMEPLAY functions into TRUE objdiff 100.0 byte-identical READABLE C. These drive the native-port effort, so the output MUST be readable plain C (NOT asm-void / .word blocks) with the decoded behavior captured in comments. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS (with their gameplay role):
${items.map(x => `  ${x.func} — ${x.role}`).join('\n')}

TWO COMPILERS AVAILABLE (per-file): the pinned 991202 (tools/mwccps2/mwccmips.exe) and 2.3.3 (tools/mwccps2-233/mwccps2.exe). 2.3.3 byte-matches the clean-store idiom-13 delay-slot-nop case 991202 cannot (proven s84 on func_001872C0). Try 991202 first; if the SOLE residual is the clean-store delay-slot nop (991202 fills beqz slot with li/lui, target leaves nop), try 2.3.3 — it likely hits 100.0.

PER-FUNCTION LOOP (S=build/agent_${id}):
1. DECODE: m2c base — .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py. The existing src/<F>.c (stub or asm-void) usually documents SEMANTICS — read & PRESERVE them. Rewrite in COMMITTED-SRC convention: PLAIN C (int/char/unsigned char/short/float, explicit casts, \`extern\` decls, fields as \`*(type *)(p + off)\`), NO prelude/M2C_ macros. Apply MATCHING_GUIDE idioms (struct field types, exact callee prototypes incl float args, the if(p){...}return 0 shape, etc.).
2. Build build/agent_${id}/src/<F>.c: leading "// CFLAGS: -O4,p -sdatathreshold 0" (try 4/8 if it uses small/gp-rel globals). PLAIN C body.
3. Compile BOTH + assemble expected + objdiff (HOST):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/o991/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/agent_${id}/o233/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff each: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o991/<F>.o <F> -o - --format json  (and o233). Read match_percent + instructions[].diff_kind.
4. Iterate to 100.0 (cap ~12 attempts — push HARD, these are high value). Get the BODY byte-identical first via idioms, then resolve the tail/delay-slot. Prefer 991202 if both hit 100.0.
KNOWN WALLS (park, report precise wall): jr-table dispatch (PROVEN dead — recognize lui %hi(jtbl);sll;addu;lw;jr), CW branch-target alignment nop, mwcc-vs-CW branch lowering (dead trailing return-0 block; 2.3.3 does NOT fix this), FPU-MAC association, register-allocation permutation, ee-gcc codegen. For these, leave readable C at its best % but matched=false.

On 100.0: matched=true, compiler ("mwcc" or "mwcc233"), c_source = FULL committed-ready file: leading "// COMPILER: mwcc233" ONLY if compiler==mwcc233, then "// CFLAGS: ...", then concise SEMANTIC comments (what the function does — for the port), then the plain-C body. role = the gameplay role.

RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, role, pct_991202, pct_233, matched, compiler, c_source (full file ONLY if matched), wall.`

phase('PlayerMatch')
const results = await parallel(batches.map((items, i) => () =>
  agent(PROTO(`pl${i}`, items), { label: `player:${items.map(x=>x.func.slice(5)).join(',')}`, phase: 'PlayerMatch', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
