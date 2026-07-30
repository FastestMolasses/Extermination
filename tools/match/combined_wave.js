export const meta = {
  name: 'exterm-combined-wave',
  description: 'One wave, two lanes: game-region funcs via mwcc 2.3.3, SDK-region funcs via ee-gcc 2.9 (avoids running two concurrent workflows against the container daemon)',
  phases: [{ title: 'Harvest', detail: 'mwcc233 game lane + eegcc SDK lane, both NEARMISS-aware' }],
}
const MWCC_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, pct_991202: { type: 'number' }, pct_233: { type: 'number' },
      matched: { type: 'boolean' }, compiler: { type: 'string' }, c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'pct_991202', 'pct_233', 'matched', 'compiler', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const EEGCC_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, pct: { type: 'number' }, matched: { type: 'boolean' },
      c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'pct', 'matched', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const input = typeof args === 'string' ? JSON.parse(args) : args
const gameFuncs = input.game || []
const sdkFuncs = input.sdk || []
const BATCH = 3
const chunk = (a) => { const o = []; for (let i = 0; i < a.length; i += BATCH) o.push(a.slice(i, i + BATCH)); return o }

const MWCC_PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. PROVEN s84: mwcc 2.3.3 (mwcps2-2.3.3-000906) byte-matches the clean-store idiom-13 delay-slot-nop case that the pinned 991202 build CANNOT (it fills the beqz delay slot with the safe li/lui). Your funcs are walled (stub or asm-void) on that wall. Produce TRUE objdiff 100.0 byte-identical READABLE C — the goal is readable C (so it must NOT be asm-void; write plain C with normal control flow), matched with whichever mwcc build hits 100.0. NEVER fake a match.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

THE PROVEN RECIPE:
1. DECODE to readable PLAIN C: m2c base — .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py — then rewrite in the COMMITTED-SRC convention: NO prelude/typedefs/M2C_ macros; use PLAIN C (int/char/unsigned char/float, explicit casts, \`extern\` decls, struct access as \`*(type *)(p + off)\` or array index). Apply docs/fanout/MATCHING_GUIDE.md idioms. Existing stub/asm-void file in src/<F>.c often documents SEMANTICS and the near-miss C — read it and PRESERVE the semantic comments.
2. Build .c in build/agent_${id}/src/<F>.c: leading "// CFLAGS: -O4,p -sdatathreshold 0" (or 4/8 if the func uses small/gp-rel globals — match what maximizes the score). PLAIN C body.
3. Compile with BOTH builds + assemble expected + objdiff (HOST):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/o991/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/agent_${id}/o233/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff each: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o991/<F>.o <F> -o - --format json   (and o233). Read match_percent + instructions[].diff_kind.
4. Iterate the C until ONE build hits 100.0 (cap ~10 attempts). If 991202 ALSO hits 100.0, prefer it (compiler="mwcc"). If only 233 hits 100.0, compiler="mwcc233".
5. ALSO verify SIZE: the compiled .text size must EQUAL the expected .text size (objdiff ignores extra trailing instructions and will false-positive an oversized compile).

KEY IDIOMS (full catalogue in docs/fanout/MATCHING_GUIDE.md — READ IT):
- idiom-21 FLOAT COMPOUND-ASSIGN: \`x = x + y\` loads LHS first (add.s f0,f2,f0); \`x += y\` loads RHS first (add.s f0,f0,f1) = the CW form. Flip the spelling when the residual is add.s operand order / FP numbering. (Cracked func_0016D130: 9 sites, 99.87 -> 100.0.)
- idiom-22 VOLATILE SCRATCHPAD: the original treats 0x7000xxxx as volatile — accesses never reorder or CSE. When the residual is scratchpad load/store ORDER, declare them \`*(volatile int *)0x700038A0\`. (func_00144040: 94.6 -> 96.8; also load-bearing in func_00206030's D_00275C7C/D_00275C80.)
- idiom-20 BRANCH-LIKELY: mwcc DOES emit beql/bnel/beqzl — never park "mwcc can't do branch-likely". It emits one when the delay slot can hold a speculatable pure-ALU op from the TAKEN path. Switch state machines: \`switch(st)\` on a LOCAL \`st = *(unsigned char*)(e+6)\`, ASCENDING case labels, advance written as \`*(unsigned char*)(e+6) = st + 1;\` USING the local.
- SWITCH vs IF-CHAIN: mwcc lowers \`switch\` to a DESCENDING beq chain. If the target has an ASCENDING bnez/bne chain, write if/else-if, NOT a switch.
- idiom-19 INVERSE-CSE: if the target recomputes a subexpression at each use but mwcc CSEs it into a callee-saved reg (frame grows 0x90->0xa0), INLINE the expression at every use site INCLUDING inside the guarding branch condition.
- FP-ARG-ORDER: when the target emits \`mtc1 zero,f13\` (trailing 0.0f) BEFORE \`mtc1 v0,f12\`, write the trailing 0.0f as an assign-in-arg temp: \`f(p, idx, A, (z = 0.0f));\`. (Does NOT work for lui-materialized nonzero constants.)
- saved-reg direction, $at compares, field-address CSE, FP odd/even coloring: see the guide.

KNOWN WALLS 2.3.3 does NOT fix (park, report wall): jr-table dispatch (PROVEN dead), CW branch-target alignment nop, FPU-MAC, regalloc/coloring permutation, list-scheduler adjacent-op swap, ee-gcc codegen.
DO NOT run the decomp-permuter — it is slow and the orchestrator runs a dedicated pass. Cap ~8 attempts on a regalloc/scheduling permutation, then report the best readable C.
RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.

NEARMISS HARVEST (readable near-miss C IS committable as port ground truth): if you canNOT reach 100.0 but you FULLY RECOVERED THE LOGIC and the sole residual is a genuine compiler artifact (register coloring / scheduling / param-save order / FP-coloring / branch-likely-sense / frame-stride / sibling-call — body/structure correct), STILL return your best READABLE c_source (plain C, NOT permuter-mangled) with matched=false, the real measured pct, and a precise wall. Leave c_source EMPTY only for: jr-table/VU0 (no valid C), or a decode you are NOT confident is logically correct. Better faithful readable C at 70% than nothing.
STRUCTURAL NOTE: if objdiff caps below 100 because splat gave the symbol TWO entry points (a second \`alabel\`/entry inside the same symbol, so a C TU must emit two symbols), say so explicitly in \`wall\` with the byte ranges — that is a splat symbol-table issue, not a codegen wall, and the orchestrator can fix it.

Report per func: func, pct_991202, pct_233, matched, compiler, c_source (full committed-ready file: "// COMPILER: mwcc233" line ONLY if compiler==mwcc233, then "// CFLAGS: ...", then semantic comments, then plain-C body), wall (precise reason if not 100.0).`

const EEGCC_PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination, ee-gcc/SDK lane. These funcs are Sony PS2 SDK / crt0 / libkernel / libc / libgcc code built with **ee-gcc 2.9-991111-01**, NOT CodeWarrior — tell-tales: sd/ld $ra (64-bit saves), daddu rd,rs,zero register moves, unfilled jal;nop slots, move s8,sp frame pointers. mwcc CANNOT match them; you MUST use ee-gcc. Produce TRUE objdiff 100.0 byte-identical READABLE C. NEVER fake a match.

READ FIRST: docs/fanout/EEGCC_GUIDE.md (full workflow + ee-gcc codegen notes).
DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

PER-FUNCTION LOOP:
1. Read build/asm/matchings/main/code/<F>.s — decode as normal gcc-2.x MIPS. Many SDK funcs are small syscall stubs / thin wrappers / list ops / bignum-math / libgcc soft-float — very matchable. IDENTIFY THE ORIGINAL SOURCE: much of this region is stock newlib / libgcc2.c / fp-bit.c (recent wins: __divdi3, __udivdi3, __mulsf3, _malloc_trim_r, sprintf). If you recognize the routine, reconstruct the canonical implementation — that is usually an exact match, not an approximation.
2. Write build/agent_${id}/src/<F>.c with FIRST TWO LINES:
   // COMPILER: eegcc
   // CFLAGS: -O2
   then plain readable C (externs, *(type*)(p+off), explicit casts). NO prelude/macros. long=64-bit; __SIZE_TYPE__=unsigned int.
3. Compile + assemble expected + diff (ONE container call, image exterm-permuter):
   container run --rm -v "$PWD:/work" -w /work exterm-permuter sh -c 'mkdir -p build/agent_${id}; mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/<F>.exp.o 2>/dev/null; tools/eegcc/ee-compile.sh build/agent_${id}/src/<F>.c build/agent_${id}/<F>.our.o -O2 2>&1 | grep -viE "warning|^$"'
   Diff on HOST (NOT in container): tools/bin/objdiff-cli diff -1 build/agent_${id}/<F>.exp.o -2 build/agent_${id}/<F>.our.o <F> -o - --format json 2>/dev/null | .venv/bin/python3 -c "import sys,json;d=json.load(sys.stdin);print([s['match_percent'] for s in d['left']['symbols'] if s['name']=='<F>'])"
   ALSO confirm .our.o .text SIZE == .exp.o .text size (objdiff false-positives an oversize compile).
4. Iterate to 100.0 (cap ~6 attempts). Most SDK leaves match at plain -O2. If it resists, try -O1 or -O0. Notes: 64-bit long arithmetic uses dsll32/dsra32; globals via lui/addiu (la) — plain externs first, rarely gp-rel; static helpers in the same TU inline (e.g. __udivmoddi4 static-inlined into __divdi3).

FAST-PARK (<=2 tries — CONFIRMED uncrackable s84, do NOT burn the budget, but DO return the readable C as NEARMISS):
- **Forward branch-likely**: expected has bgezl/blezl/bgtzl/bltzl/beql/bnel/beqzl/bnezl on a NON-loop conditional, yours compiles the same op WITHOUT the trailing 'l' at ~75-92%. Our ee-gcc only emits branch-likely for LOOP BACK-EDGES. PARK "eegcc forward-branch-likely wall (confirmed s84)".
- **Frame-size stride**: body byte-identical, only the frame immediate / an unstored reserved slot differs. PARK "eegcc frame-stride wall".
- **List-scheduler adjacent-op swap**: two adjacent independent ops swapped. PARK "eegcc list-scheduler wall".
- **Sibling/tail-call**: final discarded call emitted as \`j func_\` (gcc 2.9 has no -fno-optimize-sibling-calls). PARK "eegcc sibling-call wall".
- **GPR coloring** (v0-vs-v1, s0<->s1, a0-vs-a2): the ee-gcc permuter NEVER reaches 0 on coloring (tested s84). PARK directly.
- **"o32-vs-eabi" is a MISDIAGNOSIS**: o32 is NOT available in ee-gcc 2.9-991111-01; eabi is the only ABI and 211 matched funcs prove it correct. The t0/t1-vs-a4/a5 naming objdiff shows is COSMETIC (identical physical regs). If you think you see "o32 arg passing", it is really a list-scheduler/regalloc wall.
Do NOT run the permuter on any of the above.

On a TRUE 100.0 (objdiff AND .text size == expected): matched=true, c_source = FULL committed-ready file, pct=100.
NEARMISS HARVEST: if you canNOT reach 100.0 but FULLY RECOVERED THE LOGIC and the residual is one of the walls above (body/structure correct), STILL return your best READABLE c_source (// COMPILER: eegcc + // CFLAGS: + plain-C body) with matched=false, the real measured pct, and a precise wall. Leave c_source EMPTY only for MMI/SIMD asm (no valid C) or a decode you are NOT confident in.
RULES: touch ONLY build/agent_${id}/ ; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct, matched, c_source, wall.`

phase('Harvest')
const jobs = [
  ...chunk(gameFuncs).map((funcs, i) => () =>
    agent(MWCC_PROTO(`cw_g${i}`, funcs), { label: `mwcc:g${i}`, phase: 'Harvest', schema: MWCC_SCHEMA, agentType: 'general-purpose', effort: 'high' })),
  ...chunk(sdkFuncs).map((funcs, i) => () =>
    agent(EEGCC_PROTO(`cw_s${i}`, funcs), { label: `eegcc:s${i}`, phase: 'Harvest', schema: EEGCC_SCHEMA, agentType: 'general-purpose', effort: 'high' })),
]
const results = await parallel(jobs)
return { results: results.filter(Boolean).flatMap(r => r.results) }
