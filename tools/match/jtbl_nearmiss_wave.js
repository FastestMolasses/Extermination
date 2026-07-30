export const meta = {
  name: 'exterm-jtbl-nearmiss-wave',
  description: 'Harvest jump-table dispatchers as READABLE NEARMISS C (class is proven byte-unmatchable: external jtbl reloc wall)',
  phases: [{ title: 'JtblNearmiss', detail: 'decode jr-table dispatchers via jtbl_prep+m2c, push body-identical, harvest as NEARMISS' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, pct: { type: 'number' },
      matched: { type: 'boolean' }, c_source: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'pct', 'matched', 'c_source', 'wall'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination, jr-table NEARMISS lane. Your funcs are JUMP-TABLE DISPATCHERS (switch on a field via a jtbl_XXXX table). This class is PROVEN BYTE-UNMATCHABLE (s84, 5-hypothesis research): the original consolidated ALL jump tables into an external rodata TU, so mwcc's local @NN table counts as a reloc mismatch no matter what — there is NO source/flag lever; 100.0 is IMPOSSIBLE. Do NOT chase 100.0. Your goal is the best READABLE, LOGICALLY-FAITHFUL plain C — it will be committed as a documented // NEARMISS file (boot ELF stays byte-identical via the splat .s). Faithfulness of the decode is everything; NEVER guess semantics you have not verified against the .s.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.join(' ')}

=== STEP 0 — GET THE JTBL-AWARE m2c BASE (critical, do this FIRST per func) ===
  PAIR=$(.venv/bin/python3 tools/match/jtbl_prep.py <F>)          # prints two paths: "<F>.s <F>.jtbl.s"
  .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax \${=PAIR} 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py
  (zsh: \${=PAIR} to word-split; bash: \$PAIR is fine. If jtbl_prep prints NOJTBL/ERROR, recover the switch BY HAND from the .s: dispatch is lbu idx; sltiu $at,idx,N; ...; lw $v0,jtbl; jr $v0 — read the N entries from the .jtbl.s / data .s in case order.)
The output should contain a real switch(x){case 0: ...}. Case k MUST be the k-th positional table entry — keep exact case count including empty/fallthrough cases.

=== REWRITE IN THE COMMITTED-SRC CONVENTION (this is what gets committed) ===
NO prelude/typedefs/M2C_ macros. PLAIN C: int/char/short/unsigned variants/float, explicit casts, extern decls (int f();), struct access as *(type *)(p + off) or array index. The existing src/<F>.c stub may document SEMANTICS — read it and PRESERVE any semantic comments as a // SEMANTICS: block. File shape:
  // COMPILER: mwcc
  // CFLAGS: -O4,p -sdatathreshold 0
  <optional // SEMANTICS: block>
  <plain C body>
(-sdatathreshold 4 or 8 if the func uses %gp_rel small globals — pick what scores best.)

=== MEASURE (compile + diff each iteration) ===
container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mkdir -p build/agent_${id}/exp build/agent_${id}/obj; mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/obj/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
Diff on HOST: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/obj/<F>.o <F> -o - --format json   (read match_percent + instructions[].diff_kind)

=== PUSH THE BODY (cap ~6 attempts; the dispatch reloc diff is EXPECTED and permanent) ===
Expected permanent residuals (do not fight): the local-@NN-vs-external-jtbl_XXXX reloc on the lw, and possibly a lui/addiu/sll dispatch-sequence reorder by the post-RA scheduler. Everything ELSE (the case bodies) should converge — apply the standard idioms:
A) ASCENDING case order (mwcc reverses to descending compare-chain / positional table).
B) Per-case beql vs beq: a case gets beql only if its body's first emitted instr is speculatable pure-ALU; body leading with load/store/call -> beq+nop.
C) Clean store of a live value leaves the slot nop (free); materialized-const store fills it.
D) Struct field types/signatures, float-vs-int, if(p){...}return 0; shape, $at compares, saved-reg direction (see docs/fanout/MATCHING_GUIDE.md "MWCC POST-RA SCHEDULER MODEL").
Target: 85%+ typical (prior wave hit 89-98%). Below 70%, re-check your decode for LOGIC errors (wrong case mapping, wrong field width, missed fallthrough) before blaming the scheduler.

=== VERIFY THE DECODE (mandatory before reporting) ===
Walk EVERY case body against the .s one more time: field offsets, widths (lb/lbu/lh/lhu/lw), constants, call targets, fallthroughs, the default path, loop bounds. A NEARMISS file is port ground truth — a wrong decode is worse than no decode. If you are NOT confident the logic is correct, return c_source EMPTY with wall="decode not confident: <why>".

RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct (best measured %), matched (true ONLY if verified objdiff 100.0 — not expected here), c_source (FULL committed-ready file: // COMPILER: mwcc + // CFLAGS: + optional // SEMANTICS + plain-C body; EMPTY only if decode unconfident), wall (always include "jr-table external-dispatch wall (proven s84)" plus any other residual you saw).`

phase('JtblNearmiss')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`jn${i}`, funcs), { label: `jtblnm:${i}`, phase: 'JtblNearmiss', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
