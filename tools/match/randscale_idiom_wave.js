export const meta = {
  name: 'randscale-idiom',
  description: 'Crack the random-scaling regalloc tie-break, then apply it to the whole family',
  phases: [
    { title: 'Attack',  detail: 'five independent lenses on the exemplar func_001C1A80' },
    { title: 'Apply',   detail: 'roll the winning spelling out across the family' },
  ],
}

const DECOMP = '/Users/abe/Documents/Extermination.nosync/Extermination'
const EXEMPLAR = 'func_001C1A80'
const FAMILY = ['func_00128C10', 'func_001549C0', 'func_001551B0', 'func_001BF6B0',
                'func_001C1A80', 'func_001D0720', 'func_001DE920']

const BACKGROUND = [
  'PROJECT: matching decompilation of Extermination (PS2, SCUS-97112), CodeWarrior',
  'mwcc via wibo/qemu in an Apple `container`. Repo: ' + DECOMP + '.',
  '',
  'THE TARGET TIE-BREAK. Seven functions share one unmatched idiom:',
  '    (func_00122BB8() >> 16) * K >> 15        (a random-scaling expression)',
  'mwcc strength-reduces the multiply. For the FINAL shift of that sequence the',
  'ORIGINAL build allocates a FRESH register and then adds into the other register:',
  '    sll v1, v0, 3   /   sra v1, v1, 15   /   addiu v0, v1, 0x12c',
  'All three installed mwcc builds (991202 / 2.3.3 / 2.4) instead reuse the dying',
  'source register IN PLACE. That single difference is the only residual in',
  'func_001BF6B0 (99.96%) and part of the residual in the other six.',
  '',
  'WHY THIS IS WORTH REAL EFFORT: there is NO matched exemplar of this idiom',
  'anywhere in the corpus. Cracking the tie-break once unlocks about seven',
  'functions at a stroke — the largest single unlock currently identified.',
  '',
  'HARD RULES (project-critical):',
  '  * NEVER copy original disassembly into any committed file, and never print',
  '    it in full. Read it locally to reason; write only your own C.',
  '  * Do NOT edit src/*.c in this phase. Work in a scratch dir and REPORT.',
  '  * Report measured numbers only. An unverified claim of 100% is worse than',
  '    an honest 99.9 — every claim here is re-measured by the integrator, and',
  '    agent overclaims have already been caught in two consecutive batches.',
  '',
  'HOW TO MEASURE (canonical, do not invent your own):',
  '  cd ' + DECOMP + ' && source .venv/bin/activate',
  '  python3 tools/match/baseline.py         # m2c -> mwcc -> objdiff harness',
  '  Per-file compiler/flags come from the "// COMPILER:" / "// CFLAGS:" lines.',
  '  ALWAYS establish the CURRENT baseline % yourself before claiming a delta,',
  '  and measure the control through the SAME harness as the experiment.',
].join('\n')

const LENSES = [
  { key: 'corpus',
    task: [
      'LENS 1 — CORPUS MINING (historically the highest-yield lens on this project).',
      'Do NOT try to out-think the compiler. Find where it already did what we want.',
      'Search the BYTE-MATCHED corpus for any function where mwcc allocated a FRESH',
      'register for the final shift of a strength-reduced multiply, or more generally',
      'where a `sll`/`sra` pair writes a register other than its source and the result',
      'is then added into the original. Then read that function\'s C and extract the',
      'source shape responsible. A single matched exemplar settles this outright.',
      'Report the exemplar, its exact source spelling, and whether transplanting that',
      'spelling into the exemplar function reproduces the fresh-register allocation.',
    ].join('\n') },
  { key: 'spelling',
    task: [
      'LENS 2 — SOURCE-SPELLING SWEEP. A previous agent already tried 11 respellings',
      '(constant on the left, named temps per stage, *45*8, *45<<3, (short) cast,',
      '/0x10000 + /0x8000, chained compound assignments) and every one produced a',
      'BIT-IDENTICAL object. Do not repeat those. Go after shapes that change the',
      'LIVE RANGE rather than the arithmetic: force the scaled value to stay live',
      'past the add (use it twice, or after the add), introduce a second consumer,',
      'assign through a struct/array member, take its address, make the accumulator',
      'a different storage class, or reorder so the add reads the temp first.',
      'The goal is to make the source register still live at the shift, which is the',
      'only reason a compiler must pick a fresh destination.',
    ].join('\n') },
  { key: 'permuter',
    task: [
      'LENS 3 — PERMUTER. This is a pure REGISTER permutation with the instruction',
      'sequence already exact, which is precisely the case decomp-permuter handles',
      'well. (Note the project rule that the permuter is ANTI-correlated with objdiff',
      'on DELAY-SLOT residuals — that warning does NOT apply to register colouring.)',
      'Use tools/match/permute.py / tools/match/permuter. Run a substantial number of',
      'iterations on the exemplar and report the best scoring source found, its',
      'measured objdiff %, and whether the fresh-register colouring was reproduced.',
    ].join('\n') },
  { key: 'flags',
    task: [
      'LENS 4 — BUILD AND FLAG SWEEP. Only three CodeWarrior builds are installed',
      '(991202, 2.3.3, 2.4); 3.0/3.0.1 are NOT available — do not report results for',
      'them. Sweep optimisation levels and the register-allocation-adjacent flags',
      'across all three builds on the exemplar: -O4,p / -O3,p / -O4 / -O2,p, each',
      '-sdatathreshold in {0,4,8}, -inline off/auto/all, -opt nointrinsics, and any',
      'other flag plausibly affecting register allocation or the strength-reduction',
      'of a multiply. Report a full matrix of measured percentages.',
    ].join('\n') },
  { key: 'pressure',
    task: [
      'LENS 5 — REGISTER PRESSURE AND CALL BOUNDARIES. The idiom sits right after a',
      'call to func_00122BB8. A related finding on a sibling family showed the',
      'ORIGINAL colours the temp differently depending on what precedes the site:',
      'after `jal X / paddub a0,s0,zero / bnez v0` it uses $a1; after a call with an',
      'EMPTY delay slot it uses $v1/$v0; with no preceding call, $a0. So the caller',
      'context is what drives the colouring. Investigate whether changing what is',
      'live ACROSS the func_00122BB8 call — extra live locals, the declared prototype',
      'of func_00122BB8, whether its result is consumed immediately, argument setup —',
      'shifts the allocation. Verify the prototype of func_00122BB8 against the',
      'byte-matched corpus first; a wrong extern is a known manufacturer of fake walls.',
    ].join('\n') },
]

const ATTACK_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    lens: { type: 'string' },
    baseline_pct: { type: 'number', description: 'measured current % of the exemplar' },
    best_pct: { type: 'number', description: 'best measured % this lens achieved' },
    cracked: { type: 'boolean', description: 'true ONLY if the fresh-register colouring was reproduced' },
    recipe: { type: 'string', description: 'exact reproducible source/flag change, or empty' },
    c_source: { type: 'string', description: 'full C for the exemplar if improved, else empty' },
    evidence: { type: 'string', description: 'what was measured, and what was ruled out' },
  },
  required: ['lens', 'baseline_pct', 'best_pct', 'cracked', 'recipe', 'c_source', 'evidence'],
}

phase('Attack')
const attacks = await parallel(LENSES.map(l => () =>
  agent([BACKGROUND, '', 'EXEMPLAR: ' + EXEMPLAR + ' (159 instructions, currently 97.83%).',
         'It is the smallest and cleanest member of the family — no confounding artifacts.',
         '', l.task, '',
         'Return the schema. Set cracked=true ONLY with a measured object that shows the',
         'fresh-register colouring; otherwise false, and say precisely what you ruled out.',
        ].join('\n'),
    { label: 'attack:' + l.key, phase: 'Attack', schema: ATTACK_SCHEMA, effort: 'high' })
))

const ok = attacks.filter(Boolean)
for (const a of ok) log(a.lens + ': ' + a.baseline_pct + ' -> ' + a.best_pct + (a.cracked ? '  CRACKED' : ''))

const winners = ok.filter(a => a.cracked && a.recipe)
const improved = ok.filter(a => !a.cracked && a.best_pct > a.baseline_pct + 0.01)

if (!winners.length) {
  log('tie-break NOT cracked by any lens — reporting negative evidence, no rollout')
  return { cracked: false, attacks: ok, improved_only: improved.map(a => a.lens) }
}

const best = winners.sort((a, b) => b.best_pct - a.best_pct)[0]
log('WINNER (' + best.lens + '): ' + best.recipe.slice(0, 160))

phase('Apply')
const APPLY_SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    func: { type: 'string' },
    before_pct: { type: 'number' },
    after_pct: { type: 'number' },
    c_source: { type: 'string', description: 'full improved C, or empty if no improvement' },
    notes: { type: 'string' },
  },
  required: ['func', 'before_pct', 'after_pct', 'c_source', 'notes'],
}

const rollout = await parallel(FAMILY.filter(f => f !== EXEMPLAR).map(f => () =>
  agent([BACKGROUND, '',
         'A tie-break recipe for the random-scaling idiom has just been PROVEN on ' + EXEMPLAR + ':',
         '', 'RECIPE: ' + best.recipe,
         '', 'EVIDENCE: ' + best.evidence,
         '', 'YOUR FUNCTION: ' + f + '  (src/' + f + '.c)',
         'Apply the recipe to every occurrence of the idiom in this function. Measure',
         'before and after through the canonical harness. Some of these have OTHER',
         'residuals besides this idiom — fix what the recipe fixes, report the rest',
         'honestly, and return the improved C even if it does not reach 100%.',
        ].join('\n'),
    { label: 'apply:' + f, phase: 'Apply', schema: APPLY_SCHEMA, effort: 'high' })
))

const done = rollout.filter(Boolean)
const gained = done.filter(r => r.after_pct > r.before_pct + 0.01)
log('rollout: ' + gained.length + '/' + done.length + ' improved')
return { cracked: true, winner: best, results: [{ func: EXEMPLAR, before_pct: best.baseline_pct,
         after_pct: best.best_pct, c_source: best.c_source, notes: best.recipe }].concat(done) }
