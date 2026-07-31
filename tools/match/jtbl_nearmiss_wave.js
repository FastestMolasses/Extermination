export const meta = {
  name: 'exterm-jtbl-nearmiss-wave',
  description: 'Match jump-table dispatchers to byte-identical readable C (s85: the old external-jtbl wall was a false premise; class is matchable)',
  phases: [{ title: 'JtblNearmiss', detail: 'decode jr-table dispatchers via jtbl_prep+m2c, expected object carries its own table, push to 100.0' }],
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

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination, jr-table NEARMISS lane. Your funcs are JUMP-TABLE DISPATCHERS (switch on a field via a jtbl_XXXX table). CORRECTED s85 — THIS CLASS IS MATCHABLE. The old 'proven unmatchable' verdict was based on a FALSE premise (that the original put all jump tables in one external rodata TU). In fact each table is .rodata of the SAME TU as its function, and our expected object was simply being assembled WITHOUT it. That is fixed (build/jtblrodata + normalize_asm), and 10 of the first 23 dispatchers now byte-match at 100.0. So DO chase 100.0 — treat the dispatch like any other code. Your goal is TRUE objdiff 100.0 byte-identical READABLE plain C — it will be committed as a documented // NEARMISS file (boot ELF stays byte-identical via the splat .s). Faithfulness of the decode is everything; NEVER guess semantics you have not verified against the .s.

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.map(f => (typeof f === 'string' ? f : `${f.func}${f.pct != null ? ' (currently ' + f.pct + '%' + (f.cflags ? ', CFLAGS ' + f.cflags : '') + ')' : ''}`)).join('  ')}

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
container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mkdir -p build/agent_${id}/exp build/agent_${id}/obj; cat build/.asmnorm/<F>.s > build/agent_${id}/exp/<F>.src.s 2>/dev/null || cat build/asm/matchings/main/code/<F>.s build/jtblrodata/<F>.s > build/agent_${id}/exp/<F>.src.s; mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/agent_${id}/exp/<F>.src.s -o build/agent_${id}/exp/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/obj/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
Diff on HOST: tools/bin/objdiff-cli diff -1 build/agent_${id}/exp/<F>.o -2 build/agent_${id}/obj/<F>.o <F> -o - --format json   (read match_percent + instructions[].diff_kind)

=== PUSH TO 100.0 (cap ~10 attempts; the dispatch is now comparable, so it must match too) ===
The dispatch reloc is NO LONGER a permanent residual (the expected object now carries the table). If you still see a jtbl reloc mismatch, report it — it means build/jtblrodata/<F>.s is missing for your func, which the orchestrator can regenerate. Push the case bodies AND the dispatch to 100.0 — apply the standard idioms:
A) ASCENDING case order (mwcc reverses to descending compare-chain / positional table).
B) Per-case beql vs beq: a case gets beql only if its body's first emitted instr is speculatable pure-ALU; body leading with load/store/call -> beq+nop.
C) Clean store of a live value leaves the slot nop (free); materialized-const store fills it.
D) Struct field types/signatures, float-vs-int, if(p){...}return 0; shape, $at compares, saved-reg direction (see docs/fanout/MATCHING_GUIDE.md "MWCC POST-RA SCHEDULER MODEL").
Target: 100.0 is achievable (10 of the first 23 got there). Below 70%, re-check your decode for LOGIC errors (wrong case mapping, wrong field width, missed fallthrough) before blaming the scheduler.

=== VERIFY THE DECODE (mandatory before reporting) ===
Walk EVERY case body against the .s one more time: field offsets, widths (lb/lbu/lh/lhu/lw), constants, call targets, fallthroughs, the default path, loop bounds. A NEARMISS file is port ground truth — a wrong decode is worse than no decode. If you are NOT confident the logic is correct, return c_source EMPTY with wall="decode not confident: <why>".

RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json, tools/; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, pct (best measured %), matched (true ONLY if verified objdiff 100.0 — now genuinely achievable, aim for it), c_source (FULL committed-ready file: // COMPILER: mwcc + // CFLAGS: + optional // SEMANTICS + plain-C body; EMPTY only if decode unconfident), wall (precise per-instruction reason if not 100.0; do NOT cite the old "external-dispatch wall", it is disproven).`

phase('JtblNearmiss')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`jn${i}`, funcs), { label: `jtblnm:${i}`, phase: 'JtblNearmiss', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
