export const meta = {
  name: 'exterm-multicc-wave',
  description: 'Sweep the INSTALLED CodeWarrior builds (991202/2.3.3/2.4) over parked regalloc/branch-lowering near-misses — find which build byte-matches each',
  phases: [{ title: 'MultiCC', detail: 'decode near-miss C, compile with all 5 builds, keep the winner' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, best_compiler: { type: 'string' }, best_pct: { type: 'number' },
      matched: { type: 'boolean' }, c_source: { type: 'string' }, pcts: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'best_compiler', 'best_pct', 'matched', 'c_source', 'pcts', 'wall'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. RESEARCH ROUND: these funcs are PARKED near-misses (90-99.8% on mwcc 2.3.3) walled on REGISTER-ALLOCATION-ORDER or mwcc-vs-CW BRANCH-LOWERING — the function BODY already matches; only a register-numbering or scheduling/branch tie-break differs. We just integrated 3 additional CodeWarrior builds; a DIFFERENT build's allocator/scheduler may resolve that tie-break our way and hit TRUE objdiff 100.0. Your job: per func, decode the near-miss C, then sweep ALL FIVE builds and find which (if any) byte-matches. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.map(f => (typeof f === 'string' ? f : `${f.func}${f.pct != null ? ' (currently ' + f.pct + '%' + (f.cflags ? ', CFLAGS ' + f.cflags : '') + ')' : ''}`)).join('  ')}

THE FIVE BUILDS (all run: qemu-i386 tools/bin/wibo32 <exe> -c <FLAGS> -o <out> <src>):
  mwcc      = tools/mwccps2/mwccmips.exe       (991202, 2.3.1.01 core — default)
  mwcc233   = tools/mwccps2-233/mwccps2.exe    (000906, 2.3.1.01 core — idiom-13 fix)
  mwcc24    = tools/mwccps2-24/mwccps2.exe     (001213, 2.4.1.01 core)
  mwcc30    = tools/mwccps2-30/mwccps2.exe     (3.0-011126)  -- NOT INSTALLED on this machine
  mwcc301   = tools/mwccps2-301/mwccps2.exe    (3.0.1-020123) -- NOT INSTALLED on this machine
ONLY THREE BUILDS EXIST HERE. Check with 'ls -d tools/mwccps2*' before you start, and
never write 'invariant across every build' when you only swept the three that exist —
say 'invariant across the three installed builds'. 3.0/3.0.1 share the 2.4.1.01 core,
so they are unlikely to break a tie 2.4 does not, but that is an inference, not a test.

LEVERS PROVEN THIS SESSION (try these BEFORE concluding a build tie-break is a wall):
* COMPOUND ASSIGNMENT is not cosmetic. mwcc's canonical shape for 'x = a + b' is load a,
  load b, add f0,f1,f0. If the target loads the RHS first and emits add f0,f0,f1 (LHS as the
  FIRST add operand), the source is 'x += y;'. Neither 'x + y' nor 'y + x' nor an explicit
  temp reproduces it. Cracked func_00153540.
* SCRATCHPAD: literal address vs symbol changes codegen. '*(int*)0x700038A0 = 0' emits
  'lui at,0x7000' + sw; '&D_700038A0' emits a %hi/%lo reloc pair. One function can need the
  LITERAL form for its stores and the SYMBOL form for pointer arguments simultaneously —
  match each site to what the target actually does. Cracked func_00153540.
* COMPARISON RESPELLING (idiom-28 extension) flips mwcc's compare temp between $v0 and $at:
  'if (t > 0x96)' instead of 'if (t >= 0x97)'. 'if (0x96 < t)' is equivalent; the negated
  form 'if (!(t < 0x97))' does NOT work. Cracked a residual on func_00138C20.
* FLOAT LOCALS colour in DECLARATION order ($f0,$f1,$f2). If the target colours them
  $f2/$f1/$f0, declare the temporaries in REVERSE use order. Cracked func_001C3BE0.
* An UNREFERENCED stack local can be load-bearing: ee-gcc 2.9 keeps a dead array's slot, which
  is what produces a 0x40 frame instead of 0x30 and keeps every save offset in place.
  Cracked func_0010F7D8.

KNOWN-GENUINE class, do not burn budget on it: an access through an ABSOLUTE 0x7000xxxx
scratchpad global always exposes a speculatable address 'lui' that CodeWarrior speculates into
a branch delay slot and mwcc leaves as nop. Marking the access volatile does NOT help — the
lui hoists independently of the store. If that is your only residual, report it and move on.

If the residual is a pure register PERMUTATION at >99% with the instruction sequence already
exact, say so and recommend tools/permuter — more hand idioms will not move it.

PER-FUNCTION LOOP (S=build/agent_${id}):
1. DECODE to plain readable C: m2c base (.venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py) then rewrite in committed-src convention (PLAIN C, externs, *(type*)(p+off), NO prelude/macros). The existing src/<F>.c (stub or asm-void) and prior MATCH-ATTEMPT comments often document the exact body + the wall — read & reuse. Apply MATCHING_GUIDE idioms to get the BODY byte-identical first.
2. Build build/agent_${id}/src/<F>.c with a leading "// CFLAGS: -O4,p -sdatathreshold 0" (or 4/8 if it uses gp-rel/small globals — match what the prior near-miss used).
3. SWEEP all 5 builds + assemble expected + objdiff each (HOST):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; for CC in tools/mwccps2/mwccmips.exe tools/mwccps2-233/mwccps2.exe tools/mwccps2-24/mwccps2.exe tools/mwccps2-30/mwccps2.exe tools/mwccps2-301/mwccps2.exe; do n=$(echo $CC|sed "s#.*/##;s#\\..*##"); qemu-i386 tools/bin/wibo32 $CC -c <FLAGS> -o build/agent_${id}/o_$n.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; done'
   Diff each o_*.o on HOST: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o_<build>.o <F> -o - --format json  -> match_percent. (Map the o_ name back to the // COMPILER tag: mwccmips->mwcc, mwccps2 from -233 dir->mwcc233, etc. — easiest is to compile to clearly-named outputs o_991/o_233/o_24/o_30/o_301.)
4. Iterate the C body to push the best build to 100.0 (cap ~10 attempts). The body should match on ALL builds; the differentiator is the regalloc/schedule tie-break, so once the body is right, just read which build hits 100.0.
5. If NONE hits 100.0 (all stuck on the same tie-break), park and report the best build+pct and the precise wall.

On a 100.0: matched=true, best_compiler = the winning tag ("mwcc"/"mwcc233"/"mwcc24"/"mwcc30"/"mwcc301"), best_pct=100, pcts = "991=..,233=..,24=..,30=..,301=.." (all five), c_source = FULL committed-ready file: leading "// COMPILER: <tag>" line ONLY if the winner is NOT plain mwcc (991202 needs no tag), then "// CFLAGS: ...", concise semantics comment, then the plain-C body.

RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, best_compiler, best_pct, matched (true ONLY at verified 100.0), c_source (full file ONLY if matched), pcts (all five), wall (if not matched).`

phase('MultiCC')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`mcc${i}`, funcs), { label: `multicc:${i}`, phase: 'MultiCC', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
