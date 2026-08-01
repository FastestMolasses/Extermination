export const meta = {
  name: 'vu0-readable-c',
  description: 'Turn the last opaque VU0/MMI/COP2 stubs into documented, body-correct NEARMISS C',
  phases: [{ title: 'Decode', detail: 'batches of stubs -> readable C + semantics' }],
}

const DECOMP = '/Users/abe/Documents/Extermination.nosync/Extermination'

const BACKGROUND = [
  'PROJECT: matching decompilation of Extermination (PS2, SCUS-97112). Repo: ' + DECOMP + '.',
  'Compilers: CodeWarrior mwcc (991202 / 2.3.3 / 2.4) and ee-gcc 2.9-991111-01, run via',
  'wibo/qemu inside an Apple `container`. Build: source .venv/bin/activate first.',
  '',
  'GOAL OF THIS WAVE — READABILITY, NOT BYTE-MATCHING.',
  'These are the LAST functions in the project with NO readable C at all: opaque',
  '`// INCLUDE_ASM` stubs using VU0 macro-mode / COP2, MMI (SIMD), or COP0. They are the',
  'only thing standing between us and a fully readable C codebase, which is what the',
  'native port needs as ground truth. A byte-match here is a bonus, not the objective.',
  '',
  'DELIVER a `// NEARMISS` file: readable, BODY-CORRECT C that faithfully expresses what',
  'the function computes, plus a precise SEMANTICS comment. NEARMISS files are not',
  'compiled and not objdiff units — the linker fills their bytes from the splat .s — so',
  'the boot ELF stays byte-identical no matter what the C compiles to. That is exactly',
  'what makes it safe to write honest readable C for VU0 code that no C compiler can',
  'reproduce instruction-for-instruction.',
  '',
  'HOW TO WRITE VU0/MMI IN C:',
  '  * VU0 macro mode is 4-wide float SIMD on vf00-vf31 with an accumulator ($ACC) and',
  '    a divide unit ($Q). Express it as plain float math on a 4-float struct/array —',
  '    e.g. vmula/vmadd chains are a dot product or a 4x4 matrix-vector multiply. Say so',
  '    in the C and write the scalar equivalent.',
  '  * PS2 floats are NOT IEEE-754 (flush-to-zero, saturation, round-toward-zero). Where',
  '    that matters for behaviour, note it in a comment rather than emulating it.',
  '  * MMI is 128-bit integer SIMD, BUT `paddub $rd,$rs,$zero` is mwcc\'s REGISTER MOVE',
  '    idiom, NOT SIMD — do not describe it as a vector add. Strip move idioms before',
  '    concluding a function is SIMD at all; that misreading previously inflated this',
  '    category badly.',
  '  * COP0 is privileged/cache control; describe the operation, do not invent C for it.',
  '',
  'HARD RULES (project-critical):',
  '  * NEVER copy the original disassembly into the C file and never print it in full.',
  '    Read it locally to understand the function; write only your own C and prose.',
  '  * Only touch YOUR assigned src/<func>.c files. Do not edit tooling or other sources.',
  '  * Do NOT claim a byte-match you have not measured. If you happen to reach 100%, say',
  '    what you measured and how; the integrator re-measures everything anyway and agent',
  '    overclaims have already been caught repeatedly this session.',
  '  * Callers\' `extern` declarations are frequently WRONG in this codebase and are a',
  '    known manufacturer of fake walls. Verify a callee\'s signature against BYTE-MATCHED',
  '    files before relying on it.',
  '',
  'REQUIRED FILE SHAPE (first line exactly, so the build treats it as a stub):',
  '  // NEARMISS <func>  (vram 0x<addr>) — readable decompilation, NOT byte-identical.',
  '  //',
  '  // <what it computes, concretely: inputs, outputs, side effects, units>',
  '  // <why it cannot byte-match: VU0 macro mode / MMI / COP0 has no C spelling>',
  '  //',
  '  // Boot ELF stays byte-identical: the linker fills this function from the splat .s.',
  '  // Registry: docs/NEARMISS.md.',
  '  //',
  '  // COMPILER: mwcc233',
  '  // CFLAGS: -O4,p -sdatathreshold 0',
  '  <the readable C>',
].join('\n')

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    results: {
      type: 'array',
      items: {
        type: 'object', additionalProperties: false,
        properties: {
          func: { type: 'string' },
          klass: { type: 'string', description: 'VU0 / MMI / COP0 / COP2 / mixed / plain' },
          what_it_does: { type: 'string' },
          c_source: { type: 'string', description: 'full NEARMISS file contents, or empty if genuinely undecodable' },
          confidence: { type: 'string', description: 'high / medium / low, and why' },
          measured_pct: { type: 'number', description: 'objdiff % if you measured one, else 0' },
          notes: { type: 'string' },
        },
        required: ['func', 'klass', 'what_it_does', 'c_source', 'confidence', 'measured_pct', 'notes'],
      },
    },
  },
  required: ['results'],
}

const stubs = (typeof args === 'string' ? JSON.parse(args) : args)

// Batch so each agent gets a comparable instruction budget rather than an equal
// function count — these range from ~20 to ~1350 instructions.
const BATCHES = []
let cur = [], budget = 0
for (const s of stubs) {
  cur.push(s); budget += s.instrs
  if (budget >= 900) { BATCHES.push(cur); cur = []; budget = 0 }
}
if (cur.length) BATCHES.push(cur)

log('decoding ' + stubs.length + ' stubs in ' + BATCHES.length + ' batches')

phase('Decode')
const out = await parallel(BATCHES.map((b, i) => () =>
  agent([BACKGROUND, '',
         'YOUR STUBS (' + b.length + ' functions, ' + b.reduce((a, s) => a + s.instrs, 0) + ' instructions):',
         ...b.map(s => '  ' + s.func + '   (' + s.instrs + ' instructions, src/' + s.func + '.c,' +
                        ' asm at build/asm/matchings/main/code/' + s.func + '.s)'),
         '',
         'Work through them one at a time. Return one entry per function. If a function is',
         'genuinely undecodable, return an empty c_source and explain why in notes — an',
         'honest gap is worth more than invented C that the port would then trust.',
        ].join('\n'),
    { label: 'decode:b' + (i + 1), phase: 'Decode', schema: SCHEMA, effort: 'high' })
))

const all = out.filter(Boolean).flatMap(r => r.results || [])
const withC = all.filter(r => (r.c_source || '').trim())
log('decoded ' + withC.length + '/' + all.length + ' with readable C')
for (const r of all.filter(r => !(r.c_source || '').trim())) log('  NO C: ' + r.func + ' — ' + r.notes.slice(0, 90))
return { results: all }
