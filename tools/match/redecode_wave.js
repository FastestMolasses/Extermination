export const meta = {
  name: 'exterm-redecode-wave',
  description: 'Re-decode PARKED 99%+ near-misses to TRUE 100% via the precise last-1% idiom levers (idiom-14/16/13b, FP-arg-order, operand-reorder) — the "genuine" park label may be wrong',
  phases: [{ title: 'Redecode', detail: 'crack the last 1% of body-identical near-misses' }],
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

const PROTO = (id, items) => `Matching-decomp subagent for PS2 game Extermination. These are PARKED near-misses: each already compiles to ${'`'}>=99%${'`'} body-byte-identical C on mwcc 2.3.3 (tools/mwccps2-233/mwccps2.exe). The function BODY is essentially CORRECT — your ONLY job is to crack the LAST ~1% to TRUE objdiff 100.0 byte-identical READABLE C, by applying the PRECISE idiom lever for the residual. NEVER fake a match. The goal is READABLE plain C (NOT asm-void).

CRITICAL MINDSET: the prior "wall" note for each func (below) was written by an earlier agent who PARKED it — its "genuine, cannot fix" judgement is OFTEN WRONG. Re-derive the residual yourself from the per-instruction objdiff and try the matching lever before believing it is genuine.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)

YOUR FUNCS (with their prior measured pct + prior park note):
${items.map(o => `  ### ${o.func}  (prior: 233=${o.pct}%)\n  PRIOR NOTE: ${o.wall}`).join('\n')}

RECIPE PER FUNC:
1. DECODE to readable PLAIN C: m2c base — .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py — rewrite in committed-src convention (PLAIN C, externs, *(type*)(p+off), explicit casts; NO prelude/typedefs/M2C_ macros). The existing src/<F>.c (a stub) + the PRIOR NOTE describe the near-miss body — reconstruct it, do not start from zero.
2. Build build/agent_${id}/src/<F>.c with leading "// CFLAGS: -O4,p -sdatathreshold 0" (use 4/8 only if the func uses gp-rel/small globals — match what the prior note used; FP-absolute-addr funcs need 0).
3. Compile BOTH builds + assemble expected + objdiff (HOST, ONE container call):
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/o991/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"; qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/agent_${id}/o233/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/o233/<F>.o <F> -o - --format json  (and o991). Read match_percent + each instructions[].diff_kind to LOCATE the exact residual instruction(s).

THE LAST-1% LEVERS (apply the one matching the residual — these are PROVEN, MATCHING_GUIDE-confirmed):
- idiom-14 (clean-store delay-slot NOP — the "genuine cleanstore" parks are USUALLY this): a cond-branch (beqz/bnez/bc1f/bc1t/beq/bne) whose delay slot is a NOP in the target. mwcc leaves the slot NOP for FREE iff the FIRST EMITTED instr of the success path is a STORE/LOAD/CALL (never speculated). So: write the success block's FIRST statement as a store of an ALREADY-LIVE value — \`p->f = 0\`, \`p->f = <live param/saved reg>\`, or \`p->f = <precomputed local>\` (NO materialization needed) — and the nop appears at zero cost. ONLY a store of a value that must be MATERIALIZED (\`p->f = 3\` -> li;sb fills the slot) is a GENUINE wall. If the success block really begins with a load/store/call, make THAT the first if-body statement. Multi-statement blocks are fine; const stores emit after.
- idiom-16 (const-in-delay-slot): write a const store / const call-arg as the LAST statement before a call -> mwcc sinks the store/li into the jal/j delay slot, hoists the materialization earlier. For multi-const call args the LAST arg's li fills the slot.
- idiom-13b (dead-const re-materialization): if mwcc speculates a const into a cond-branch slot AND must re-emit it dead at the target, force it via an if/else-if chain where every arm passes the SAME large const to a call, or two stores of the same const straddling the branch.
- FP-ARG-ORDER idiom (the fparg parks): a call like f(p, idx, A, 0.0f) where the target emits 'mtc1 zero,f13' (trailing 0.0f) BEFORE 'mtc1 v0,f12' (A), but mwcc emits f12 then f13 — flip mwcc's order by writing the trailing arg as an assign-in-arg temp: \`float z; ... f(p, idx, A, (z = 0.0f));\`. Also idiom-12: a single \`float\` param live across a call colors by decl order; an inline \`0.0f\` literal in a compare gives c.eq.s f0,f21 (a \`float zero\` local reverses it).
- operand/commutative reorder (the operand parks): for a commutative op whose operands are swapped vs target, swap the C operands (a+b vs b+a, a*b vs b*a); for \`x = x + d\` emitted as add with wrong operand order, write \`x += d\`. For a CSE/operand-order diff across a call, see idiom-15 (read field AFTER call to force reload, or into a local BEFORE to keep).
- float-compare operand-order (fcmp parks): flip the comparison sense — \`if(!(x <= K)) ...\` vs \`if(x > K) ...\` changes which c.le.s/c.lt.s + branch polarity mwcc emits; try both senses and the constant on either side.
4. Iterate (cap ~10 attempts). Get the SOLE residual to vanish. If ONLY 233 hits 100.0 -> compiler="mwcc233"; if 991202 ALSO hits 100.0 -> prefer compiler="mwcc".
5. GENUINELY un-crackable after applying the right lever (park, report precise wall): CW branch-target 8-byte ALIGNMENT nop (never emitted by mwcc); mwcc-vs-CW two-exit/dead-trailing-block branch lowering; PURE FP-register COLORING (target uses f1 odd-pair where mwcc uses f2/f4 across ALL sites, control flow identical — this needs the permuter, not an idiom); pure saved-register NUMBERING permutation (s1/s0/s2 vs s0/s1/s2 — permuter); jr-table; clean-CONSTANT-store (materialized value, genuine idiom-13). If the residual is one of THESE, report best % + the precise wall — do NOT burn >8 attempts.

On a TRUE 100.0: matched=true, compiler ("mwcc"/"mwcc233"), c_source = FULL committed-ready file: leading "// COMPILER: mwcc233" (ONLY if 233), then "// CFLAGS: ...", a concise semantics comment, then the plain-C body. pct_991202 + pct_233 = both measured.
RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct_991202, pct_233, matched, compiler, c_source (full file ONLY if matched), wall (if not matched).`

phase('Redecode')
const results = await parallel(batches.map((items, i) => () =>
  agent(PROTO(`rd_${i}`, items), { label: `redecode:${i}`, phase: 'Redecode', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
