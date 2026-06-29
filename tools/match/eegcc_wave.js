export const meta = {
  name: 'exterm-eegcc-wave',
  description: 'Match Sony-SDK/lowmem funcs to readable byte-identical C via ee-gcc 2.9-991111-01 (sd/ld 64-bit saves, daddu moves — mwcc can NEVER match these)',
  phases: [{ title: 'EEGCC', detail: 'decode SDK .s, match with ee-gcc 2.9 -O2' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, pct: { type: 'number' }, matched: { type: 'boolean' },
      c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'pct', 'matched', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const cands = (typeof args === 'string' ? JSON.parse(args) : args)
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination, ee-gcc/SDK lane. These funcs are Sony PS2 SDK / crt0 / libkernel / libc code built with **ee-gcc 2.9-991111-01**, NOT CodeWarrior — tell-tales: sd/ld $ra (64-bit saves), daddu rd,rs,zero register moves, unfilled jal;nop slots, move s8,sp frame pointers. mwcc CANNOT match them; you MUST use ee-gcc. Produce TRUE objdiff 100.0 byte-identical READABLE C. NEVER fake a match.

READ FIRST: docs/fanout/EEGCC_GUIDE.md (full workflow + ee-gcc codegen notes).
DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

PER-FUNCTION LOOP:
1. Read build/asm/matchings/main/code/<F>.s — decode as normal gcc-2.x MIPS (sd/ld 64-bit saves, daddu moves, unfilled jal;nop are NORMAL). Many SDK funcs are small syscall stubs / thin wrappers / list ops / bignum-math — very matchable. The existing src/<F>.c (stub) may document semantics.
2. Write src/<F>.c in build/agent_${id}/src/<F>.c with FIRST TWO LINES:
   // COMPILER: eegcc
   // CFLAGS: -O2
   then plain readable C (externs, *(type*)(p+off), explicit casts). NO prelude/macros. long=64-bit; __SIZE_TYPE__=unsigned int.
3. Compile (ee-gcc) + assemble expected + diff (ONE container call, image exterm-permuter):
   container run --rm -v "$PWD:/work" -w /work exterm-permuter sh -c 'mkdir -p build/agent_${id}; mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/<F>.exp.o 2>/dev/null; tools/eegcc/ee-compile.sh build/agent_${id}/src/<F>.c build/agent_${id}/<F>.our.o -O2 2>&1 | grep -viE "warning|^$"'
   Diff on HOST (NOT in container): tools/bin/objdiff-cli diff -1 build/agent_${id}/<F>.exp.o -2 build/agent_${id}/<F>.our.o <F> -o - --format json 2>/dev/null | .venv/bin/python3 -c "import sys,json;d=json.load(sys.stdin);print([s['match_percent'] for s in d['left']['symbols'] if s['name']=='<F>'])"
   Also confirm the .our.o .text SIZE matches the .exp.o .text size (objdiff scores expected instrs and can false-positive an oversize compile — verify size equal).
4. Iterate to 100.0 (cap ~6 attempts). Most SDK leaves match at plain -O2. If it resists, try -O1 or -O0 in the CFLAGS line. ee-gcc notes: tail-call wrappers (j func) may need a non-tail-call rewrite (gcc 2.9 has no -fno-optimize-sibling-calls); 64-bit long arithmetic uses dsll32/dsra32; globals via lui/addiu (la) — plain externs first, rarely gp-rel.
5. PARK (report wall) only after ~6 tries: genuine ee-gcc regalloc/coloring permutation (note best%), MMI 128-bit SIMD (asm-void), or a different SDK compiler.

FAST-PARK (≤2 tries — these are CONFIRMED uncrackable s84, do NOT burn the full budget):
- **Forward branch-likely**: expected has bgezl/blezl/bgtzl/bltzl/beql/bnel/beqzl/bnezl on a NON-loop conditional (classic: the \`func_0010E8A8(...) < 0\` / syscall-return success check), your clean C compiles to the same op WITHOUT the trailing 'l' (non-annul) at ~75-92%. Our ee-gcc 2.9-991111-01 only emits branch-likely for LOOP BACK-EDGES, never forward annul — proven not flag/ISA/source-crackable. PARK as "eegcc forward-branch-likely wall (confirmed s84)". (Loop back-edge branch-likely IS reproducible — only forward is walled.)
- **Frame-size stride**: body byte-identical, only the frame immediate / an unstored reserved stack slot differs. PARK "eegcc frame-stride wall".
- **List-scheduler adjacent-op swap**: two adjacent independent ops swapped (e.g. const-materialize-first vs address-first); ee-gcc 2.9 deterministic. PARK "eegcc list-scheduler wall".
- **Sibling/tail-call**: final discarded call emitted as \`j func_\` (gcc 2.9 has no -fno-optimize-sibling-calls). PARK "eegcc sibling-call wall".
Do NOT run the permuter on any of the four above (scorer normalizes the diff / plateaus).

On a TRUE 100.0 (objdiff AND .text size == expected): matched=true, c_source = FULL committed-ready file (the two markers + readable C body), pct=100. Else matched=false, pct=best, wall=precise reason.
RULES: touch ONLY build/agent_${id}/ ; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct, matched, c_source (full file ONLY if matched), wall (if not).`

phase('EEGCC')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`eg_${i}`, funcs), { label: `eegcc:${i}`, phase: 'EEGCC', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
