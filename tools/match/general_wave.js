export const meta = {
  name: 'exterm-general-wave',
  description: 'Match fresh high-baseline game stubs (never-measured vein) via m2c base + full idiom catalogue',
  phases: [{ title: 'Match', detail: 'decode + idiom-finish fresh candidates to objdiff 100%' }],
}
const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { results: { type: 'array', items: {
    type: 'object', additionalProperties: false,
    properties: {
      func: { type: 'string' }, baseline_pct: { type: 'number' }, final_pct: { type: 'number' },
      matched: { type: 'boolean' }, c_source: { type: 'string' }, idiomUsed: { type: 'string' }, wall: { type: 'string' },
    }, required: ['func', 'baseline_pct', 'final_pct', 'matched', 'c_source', 'idiomUsed', 'wall'],
  } } }, required: ['results'],
}
const cands = typeof args === 'string' ? JSON.parse(args) : args
const BATCH = 3
const batches = []
for (let i = 0; i < cands.length; i += BATCH) batches.push(cands.slice(i, i + BATCH))

const PROTO = (id, funcs) => `Matching-decomp subagent for PS2 game Extermination. These are FRESH high-baseline (70-97% m2c) game stubs never before attempted. Turn each into TRUE objdiff 100.0 byte-identical C. NEVER fake a match. Several look like MPEG-2 FMV decoder / streaming library functions (names like pictureSpatialScalableExtension, load_chroma_non_intra_quantizer) — if one walls like SDK/ee-gcc codegen (EABI prologue, branch-likely you can't reproduce), PARK it and say so (it may be ee-gcc library code, not mwcc).

DIR: /Users/abe/Documents/Extermination.nosync/Extermination   AGENT: ${id}   SCRATCH: build/agent_${id}/ (ONLY this)
YOUR FUNCS: ${funcs.map(f => (typeof f === 'string' ? f : `${f.func}${f.pct != null ? ' (currently ' + f.pct + '%' + (f.cflags ? ', CFLAGS ' + f.cflags : '') + ')' : ''}`)).join('  ')}

PER-FUNCTION LOOP (S=build/agent_${id}):
1. m2c base: PAIR=\$(.venv/bin/python3 tools/match/jtbl_prep.py <F>); if it prints two paths use \${=PAIR} (jtbl dispatcher — but note jr-table dispatchers are a PROVEN WALL, park them); else m2c on the plain .s:
   .venv/bin/python3 tools/m2c/m2c.py --target mipsee-mwcc-c --valid-syntax build/asm/matchings/main/code/<F>.s 2>/dev/null | .venv/bin/python3 tools/match/m2c_clean.py
2. Build .c: first line "// CFLAGS: -O4,p -sdatathreshold 8" then contents of tools/m2c/exterm_prelude.h then the decoded body. (Use -sdatathreshold 8 — the mwcc default; it keeps 8-byte s64 globals gp-rel addressable. Use 0 only if the func has no small-global access; 4 is usually wrong for 8-byte objects.)
3. Compile + assemble + diff:
   container run --rm -v "$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc build/asm/matchings/main/code/<F>.s -o build/agent_${id}/expected/<F>.o 2>/dev/null; qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/obj/<F>.o build/agent_${id}/src/<F>.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
   Diff on HOST: tools/bin/objdiff-cli diff -1 build/agent_${id}/expected/<F>.o -2 build/agent_${id}/obj/<F>.o <F> -o - --format json  (read match_percent + instructions[].diff_kind).
4. Iterate to 100.0 (cap ~10 attempts). Read the per-instruction delta each round; apply the idiom that matches the wall.

=== IDIOM CATALOGUE (full detail in docs/fanout/MATCHING_GUIDE.md "MWCC POST-RA SCHEDULER MODEL") ===
1) FP odd/even coloring: single-prec producer (cvt.s.w/lwc1/mtc1) colors dest to ODD companion f1 when it's the FIRST/longer-lived operand of a float binop; chain conversion into a binop; only ONE float const live at a time.
2) float-arg prototype hygiene: declare EVERY called fn's FPU args as single float, exact arity, no K&R/unprototyped extern (promotes float->double->GPR via dsll32), no stray double.
4) saved-reg direction: raw params across a call -> regs TOP-DOWN (3 params a0=s2,a1=s1,a2=s0); copy each param into a surviving local (used in arithmetic) -> BOTTOM-UP. Match target polarity.
5) float early-return-0 two-exit: write float early return 0 as the FALL-THROUGH after a positive-condition if; duplicate return 0 at each nesting level.
6/13) RMW fills delay slot (p->f|=1 -> lb;ori;sb); CLEAN const store (p->f=1 -> li;sb) ALWAYS fills slot = genuine wall.
7) \$at compare: keep a branch-feeding compare in a NAMED gpr only if the bool is ALSO stored (c=v<N; p->flag=c; if(c)). Pure branch=\$at. Cache unsigned-byte field into int before compare for signed slti.
8) field-address CSE: access p->field DIRECTLY everywhere (no pointer-typed local alias) so mwcc keeps only the base in a saved reg.
9) jal-delay saved-reg copy: mwcc fills a call's delay slot with the LAST callee-saved param copy when 2+ params live across the call and result is discarded/stored.
10) address-escaped loop counter: if target reloads counter from stack each iter, escape its address (int i; h(&i);), use do{...i++;}while(i<K);.
11) FP-load coloring=operand position: LEFT operand of single-prec binop -> ODD f1/f3; RIGHT -> EVEN f0.
12) FP-param pairing POSITIONAL: N float params across a call save TOP-DOWN, param k -> f(20+(N-1)-k). Inline 0.0f literal for c.eq.s f0,f21.
13b) dead-const re-materialization: if/else-if arms all passing the SAME large lui+ori const to a call, or two stores of the same nonzero const straddling a slot-less branch.
14) idiom-13 nop STORE-OF-LIVE-VALUE (DE-WALLS many): clean store of an already-live value (p->f=0, p->f=<param>, p->f=<precomputed local>) leaves a cond-branch slot a NOP at zero cost. Only a materialized-const store fills it.
15) reload vs CSE across a call: read base->field AFTER the call for fresh reload-per-call; into a surviving local BEFORE for a kept value.
16) const-in-delay-slot: write a const store / last const call-arg as the LAST stmt before a call -> sinks into the j/jal slot, materialization hoisted early.
17) paddub timing: with >=2 params across a call, ONE saved-reg copy fills the following branch/jal slot; saved-reg number = first-use-after-call order.
18) s64-param store: declare a 64-bit-stored param long long for sd sN,off (no dsll32/dsra32).
KNOWN HARD WALLS (recognize & PARK fast): dense-switch jr-table dispatch (PROVEN unmatchable s84); CW branch-target alignment nop; clean-const-store idiom-13; register-allocation permutation (try ~3 decl-order tricks then park); mwcc-vs-CW branch lowering; FPU latency nops.

RULES: touch ONLY build/agent_${id}/; never canonical build/obj|expected, src/, objdiff.json; never run build.py/verify_all/git. No disc disasm to external context.
Report per func: func, baseline_pct, final_pct, matched (true ONLY at verified objdiff 100.0), c_source (FULL file incl // CFLAGS line, ONLY if matched), idiomUsed, wall (precise per-instruction wall if not matched).`

phase('Match')
const results = await parallel(batches.map((funcs, i) => () =>
  agent(PROTO(`gw${i}`, funcs), { label: `gw:gw${i}`, phase: 'Match', schema: SCHEMA, agentType: 'general-purpose', effort: 'high' })
))
return { results: results.filter(Boolean).flatMap(r => r.results) }
