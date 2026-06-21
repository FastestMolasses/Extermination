export const meta = {
  name: 'exterm-jtbl-research',
  description: 'Crack the dense-switch jump-table dispatch scheduling wall (mwcc lui-in-slot vs CW nop+external-table) — keystone for ~146 dispatcher functions',
  phases: [
    { title: 'Research', detail: '5 parallel hypotheses to make mwcc match the CW jtbl dispatch schedule' },
    { title: 'Verify', detail: 'adversarially reproduce any claimed crack to true objdiff 100.0' },
  ],
}
const RES_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    hypothesis: { type: 'string' },
    cracked: { type: 'boolean' },
    best_pct: { type: 'number' },
    recipe: { type: 'string', description: 'EXACT reproduction: full src + flags + commands if cracked (or best attempt)' },
    finding: { type: 'string', description: 'what was learned — what changed the schedule, what did not, and why' },
    sole_wall_funcs: { type: 'array', items: { type: 'string' }, description: 'funcs where dispatch schedule is the ONLY remaining wall (chars agent only)' },
  }, required: ['hypothesis', 'cracked', 'best_pct', 'recipe', 'finding'],
}
const VER_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: { confirmed: { type: 'boolean' }, reproduced_pct: { type: 'number' }, notes: { type: 'string' } },
  required: ['confirmed', 'reproduced_pct', 'notes'],
}

const WALL = `THE WALL (dense-switch jump-table dispatch — open since session 23, now blocks ~146 functions):
The original (CodeWarrior, target) emits the switch dispatch as:
    beqz \$at, <default>          # bounds check (index >= ncases)
    nop                          # <-- DELAY SLOT LEFT AS NOP
    lui  \$aN, %hi(jtbl_XXXX)     # table base, lui/addiu ADJACENT
    addiu \$aN, \$aN, %lo(jtbl_XXXX)
    sll  \$vN, \$idx, 2           # index*4  (sll comes AFTER addiu)
    addu \$vN, \$vN, \$aN
    lw   \$vN, 0(\$vN)
    jr   \$vN
The jtbl_XXXX is referenced as an EXTERNAL symbol (in the real game ALL jump tables are
consolidated in one rodata region at 0x0026xxxx, separate from per-function rodata — strong
evidence they were emitted to a dedicated section or a shared translation unit).
OUR mwcc, compiling a C \`switch\`, emits its OWN LOCAL jtbl (.rodata local object @NN) and
the post-RA scheduler FREELY reorders it:
    beqz \$at, <default>
    lui  \$aN, %hi(jtbl)          # <-- LUI SPECULATED INTO THE DELAY SLOT
    sll  \$vN, \$idx, 2           # sll BEFORE addiu
    addiu \$aN, \$aN, %lo(jtbl)
    addu ... ; lw ... ; jr
=> two differences, both rooted in mwcc treating its LOCAL table as freely-schedulable:
(a) the lui fills the beqz delay slot (CW leaves nop); (b) addiu/sll order swapped.
THEORY (testable): mwcc will NOT speculate the %hi of an EXTERNAL / separate-section symbol
into a delay slot, but DOES for a local same-section symbol. If we make the dispatch
reference jtbl_XXXX as external (or a separate section mwcc won't reorder around), the
schedule should match. Confirmed NOT fixable by -O0..4 / -opt level / -inline / sdatathreshold
(already swept). The fix, if any, is a pragma / model flag / inline-asm / external-table form.`

const HARNESS = (id) => `HARNESS (DIR /Users/abe/Documents/Extermination.nosync/Extermination, SCRATCH build/agent_${id}/ — use ONLY this):
TEST FUNC: func_0022EC30 — a near-match at 97.6% where the dispatch schedule is the ONLY wall
(1 nop + addiu/sll reorder; fixing it => 100.0). A known-good base C source is at
build/match/jtbl_research/base_func_0022EC30.c (copy it to build/agent_${id}/func_0022EC30.c).
EXPECTED object (do not modify): build/match/jtbl_research/expected_func_0022EC30.o
COMPILE (container; never docker):
  container run --rm -v "\$PWD:/work" -w /work exterm-toolchain sh -c 'qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c <FLAGS> -o build/agent_${id}/x.o build/agent_${id}/func_0022EC30.c 2>&1 | grep -viE "MWCIncludes|Usage|environment"'
  (default FLAGS: -O4,p -sdatathreshold 4)
DIFF (host):
  tools/bin/objdiff-cli diff -1 build/match/jtbl_research/expected_func_0022EC30.o -2 build/agent_${id}/x.o func_0022EC30 -o - --format json
  -> parse left.symbols[name==func_0022EC30].match_percent ; instructions[].diff_kind shows which rows differ.
DISASSEMBLE to inspect schedule:
  container run --rm -v "\$PWD:/work" -w /work exterm-toolchain sh -c 'mipsel-linux-gnu-objdump -dr build/agent_${id}/x.o 2>/dev/null'
mwcc full help (for flags/pragmas):
  container run --rm -v "\$PWD:/work" -w /work exterm-toolchain sh -c 'qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -help 2>&1' ; also try '-help usage'.
RULES: touch ONLY build/agent_${id}/ ; never canonical build/obj|expected, src/, objdiff.json, tools/mwccps2/; never run build.py/verify_all/git. Do NOT paste original disc disassembly to any external service (web searches must be GENERIC: "CodeWarrior MIPS jump table matching", never paste game bytes).`

const HYP = [
  { id: 'pragma', effort: 'high', label: 'pragma+flag', task:
`HYPOTHESIS 1 — PRAGMA & FLAG EXHAUSTION. Dump the FULL mwcc -help and '-help usage'. Then
EXHAUSTIVELY test every plausible CodeWarrior pragma and flag that could change the jtbl
dispatch schedule (leave the beqz delay slot a nop AND/OR keep lui/addiu adjacent with sll
after). Candidates to actually TRY (compile+diff each, record the match% delta):
  #pragma directives: optimization_level 0..4 (wrapped around just the function),
    #pragma optimize_for_size on/off, #pragma scheduling on/off, #pragma opt_* ,
    #pragma section / #pragma push_section / #pragma force_active, any data-section pragma.
  flags: -model relative | -model absolute, -gpopt / -gprel, -common on|off, -sdata*,
    -sconst*, -volatileasm, -schedule (if it exists), -peephole on|off, -ipa off, -char ...,
    any flag that affects data placement or the post-RA scheduler.
Goal: find ONE recipe that takes func_0022EC30 to objdiff 100.0. If a pragma changes the
schedule even partially, report it precisely. cracked=true ONLY at verified 100.0.` },
  { id: 'asm', effort: 'high', label: 'inline-asm', task:
`HYPOTHESIS 2 — INLINE-ASM DISPATCH. Replace the C \`switch\` dispatch with mwcc inline
assembly that reproduces the CW sequence EXACTLY (beqz \$at bounds-check + nop + lui/addiu
%hi/%lo(jtbl_00275240) external + sll + addu + lw + jr), referencing jtbl_00275240 as an
EXTERNAL symbol (declare it; do NOT define it — at the .o level it stays undefined-external,
exactly like the expected object). Keep the case bodies as ordinary C code blocks that mwcc
emits in source order after the dispatch. THE HARD PART: mwcc must NOT dead-code-eliminate
the case bodies (they look unreachable to its control-flow analysis once the jr is opaque
asm). Techniques to try: (a) keep a real \`switch\`/if-chain that mwcc compiles (so bodies are
reachable) but whose generated dispatch you SUPPRESS/override — e.g. wrap the asm jr so it
jumps before mwcc's own table; (b) use asm labels for each case and a computed jr; (c) a
\`volatile\` asm + a dummy use of each case label. Check mwcc's inline-asm syntax first (asm{}
vs __asm). Verify the produced .o is byte-identical (objdiff 100.0). cracked=true ONLY at 100.0.` },
  { id: 'extern', effort: 'high', label: 'external-table', task:
`HYPOTHESIS 3 — EXTERNAL-TABLE / SEPARATE-TU FORM. Find a pure-C construct that makes mwcc
reference the jump table as an EXTERNAL (undefined) symbol instead of emitting its own local
one, which (per the theory) should stop the scheduler speculating the lui into the delay slot.
Try: (a) declaring \`extern <type> jtbl_00275240[];\` and writing the dispatch as an indexed
jump via a form mwcc lowers to jr (function-pointer table with a tail-call? a goto via array?);
(b) a function-pointer array \`extern Fn tbl[]; tbl[i](args);\` — note this emits jalr(call)
not jr(jump), so it will NOT match a jr-dispatch; confirm/deny and explain; (c) any #pragma
that forces the compiler's switch table to a named/extern section. Determine definitively
whether mwcc can be made to reference an external jr-table from C. Report the exact construct
+ resulting schedule. cracked=true ONLY at verified objdiff 100.0 on func_0022EC30.` },
  { id: 'chars', effort: 'high', label: 'characterize', task:
`HYPOTHESIS 4 — CHARACTERIZE & SIZE THE PRIZE (no fix needed; measurement). For EACH of these
jtbl funcs: func_0022EC30 func_001B6D70 func_001B7840 func_00181BA0 func_0015B770 func_00151770
func_0015CBA0 func_00147700 func_001B9C10 func_001386E0 — get the jtbl-aware m2c base
(PAIR=\$(.venv/bin/python3 tools/match/jtbl_prep.py <F>); m2c --target mipsee-mwcc-c
--valid-syntax \${=PAIR} | tools/match/m2c_clean.py), do a quick best-effort decode, compile
(-O4,p -sdatathreshold 4) and diff against the freshly-assembled expected
(mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc
build/asm/matchings/main/code/<F>.s -o build/agent_chars/exp/<F>.o). For each, classify the
REMAINING walls from the per-instruction diff + objdump: is the dense-switch DISPATCH SCHEDULE
the SOLE remaining wall (everything else byte-identical), or are there OTHER walls too (idiom-13,
regalloc, FP, etc.)? Return sole_wall_funcs = the list where dispatch-schedule is the ONLY
blocker (these all flip to 100.0 the moment the wall is cracked). Also state whether the
external->nop rule holds on every one. This sizes the prize. cracked=false (you are measuring).` },
  { id: 'precedent', effort: 'medium', label: 'precedent', task:
`HYPOTHESIS 5 — REFERENCE-PROJECT PRECEDENT (web research). Investigate how other CodeWarrior /
mwccps2 (PS2) matching-decomp projects handle NON-MATCHING switch jump tables / jr-dispatch
scheduling. Use WebSearch/WebFetch (GENERIC queries only — never paste game bytes): e.g.
"CodeWarrior mwcc jump table matching decomp", "mwccps2 switch jr dispatch nonmatching",
"decomp.me mwcc jump table", "metrowerks #pragma section jump table", recvx-decomp / other
mwcc decomp repos, the m2c / decomp-toolkit docs on jump tables, sotn/melee/CW decomp lore.
Determine: is there a KNOWN flag/pragma/section trick that makes mwcc match an external
jr-table? Or do these projects accept it as INCLUDE_ASM / a documented nonmatch? Report concrete
findings with sources (URLs). cracked=false unless you find an exact reproducible recipe; if you
find a candidate flag/pragma, NAME it precisely so the verify stage can test it.` },
]

phase('Research')
const results = await parallel(HYP.map(h => () =>
  agent(`${WALL}\n\n${HARNESS('h_' + h.id)}\n\n${h.task}`,
    { label: `research:${h.label}`, phase: 'Research', schema: RES_SCHEMA, agentType: 'general-purpose', effort: h.effort })
))
const res = results.filter(Boolean)

phase('Verify')
const claims = res.filter(r => r.cracked && r.best_pct >= 99.9)
let verifications = []
if (claims.length) {
  verifications = await parallel(claims.map((c, i) => () =>
    agent(`${WALL}\n\n${HARNESS('v_' + i)}\n\nVERIFY a claimed crack of the jtbl-dispatch wall. Another agent reports it reached objdiff 100.0 on func_0022EC30 with this recipe:\n\n--- RECIPE ---\n${c.recipe}\n--- END RECIPE ---\n\nReproduce it from scratch in build/agent_v_${i}/: apply the recipe EXACTLY, compile, and diff against build/match/jtbl_research/expected_func_0022EC30.o. Report confirmed=true ONLY if you independently measure objdiff match_percent == 100.0 for func_0022EC30. Be adversarial: if the recipe is underspecified, the .o doesn't actually hit 100.0, or it only "matches" by emitting a wrong/placeholder table, confirmed=false. Report the exact reproduced_pct you measured.`,
      { label: `verify:${c.hypothesis.slice(0,16)}`, phase: 'Verify', schema: VER_SCHEMA, agentType: 'general-purpose', effort: 'high' })
  ))
}
return { research: res, verifications: verifications.filter(Boolean) }
