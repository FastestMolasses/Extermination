# PS2 Matching-Decomp Playbook (portable)

Hard-won, **project-agnostic** lessons from the Extermination (SCUS-97112) matching
decompilation. Written to be lifted wholesale into the next PS2 decomp. Nothing here is
specific to one game except where an example is labelled as such.

Companion docs in this repo (Extermination-specific): `PROGRESS.md` (state),
`FINDINGS.md` (file formats), `fanout/MATCHING_GUIDE.md` (the full compiler-idiom
catalogue), `NEARMISS.md` (the near-miss registry).

---

## 0. The single most important lesson

> **One failed source shape is not a proof of impossibility.**

We twice recorded a codegen behaviour as a "confirmed uncrackable wall", told every agent
to stop trying, and were **wrong both times**. Each time the real story was "the compiler
needs a specific precondition, and the one experiment we ran didn't supply it."

- *mwcc branch-likely* (`beql`/`bnel`): declared impossible from C. Actually emitted
  whenever the delay slot can be filled with a speculatable pure-ALU op from the taken path.
  73 already-matched functions contained it — the evidence was sitting in our own tree.
- *ee-gcc forward branch-likely* (`bgezl` on a non-loop conditional): declared a
  "build-version reorg wall", estimated at "10%+ of all SDK stubs", fast-park mandated.
  Actually emitted when the branch-target path's first instruction is a cheap speculatable
  load. Retesting the 7 functions named as walled: all 7 improved, one went straight to 100%.

**Process rules that follow:**
1. Before recording a wall, **grep your own matched corpus** for the instruction/pattern you
   are about to call impossible. If a matched function contains it, it is possible.
2. Write walls as *mechanisms with preconditions* ("annul requires a speculatable op in the
   slot; a `lui` rematerialization is not speculatable"), never as *capability denials*
   ("the compiler cannot emit annulled branches").
3. Re-test parked classes periodically. Park lists rot; they are the cheapest place to find
   free matches.
4. A wall description that cannot explain *why the original compiler did it* is not finished.

---

## 1. Trust nothing about your metric until you have audited it

Matching decomps live on one number. Ours was quietly wrong in three separate ways.

### 1a. objdiff ignores extra trailing instructions
objdiff scores the *expected* symbol's instructions. A compiled function that is **oversized**
can therefore read **100%** while emitting garbage past the end — which shifts every
subsequent function and breaks the link.
**Fix:** gate every "match" on `objdiff == 100.0` **AND** compiled `.text` size == expected
`.text` size. Read the size straight out of the ELF section header; do not trust the tool.

### 1b. Renames leave phantom units behind
When you rename symbols and re-run splat, it emits `real_name.s` — but the old
`func_ADDR.s` **stays on disk** (the asm dir is normally git-ignored, so nothing cleans it).
Any `src/func_ADDR.c` next to it still looks like a valid unit. We accumulated **47** of
these; **9 pairs were compiled on both sides**, so the same function was counted twice.
**Fix:** after any rename pass, reconcile `src/*.c` against the symbol table and delete
orphans. Watch for the case where the *phantom* holds the better work (real decompiled C)
and the canonical name is still a stub — migrate, don't delete. Verify each migration keeps
its 100% before removing the original.

### 1c. Percentages vs counts measure different things
`matched_code%` is byte-weighted; `matched/total functions` is a count. They will not agree
and dividing one into the other is meaningless. Report both, label both.

---

## 2. splat invents symbols that do not exist

This is the highest-yield bug class we found, and it is **generic to splat**, not to one
game. splat symbolizes `lui`/`addiu` pairs into `%hi`/`%lo` references. When it guesses
wrong, the assembled *target* object carries a **relocation** where the real code has a
**plain immediate**. No C source can ever reproduce a relocation that isn't real, so the
function is permanently unmatchable for a reason that has nothing to do with the compiler.

Three variants we hit:

| Variant | What splat did | Tell |
|---|---|---|
| **Bogus address** | Paired a delay loop's `lui rX,0x10` with the loop-*body* `addiu rX,rX,-1` (a decrement executed 0x100000 times, past a branch label) because the values combine to `0xFFFFF` | The "symbol" is **below the load address** — it cannot be real |
| **Offset constant** | Rendered `addiu a3,a2,0x10` as `%lo(D_20000010)` | The "symbol" is an offset in the uncached mirror (`0x20000000 |` addr), not a global |
| **Split data label** | Minted `D_00279448` for what the compiler addresses as `D_00279440 + 8` | Both are legitimate addresses — see the caution below |

**How to detect:** list every symbol referenced via `%hi`/`%lo` in your disassembly and
sanity-check each against the memory map. Anything below the load address, inside a
hardware/mirror region, or suspiciously equal to `<round number> - 1` is a fabrication.

**How to fix (without re-running splat):** normalize at assemble time. Substituting the
literal immediate produces **byte-identical output** — only the spurious relocation
disappears. `%hi(0xFFFFF)` is `0x10`, `%lo(0xFFFFF)` is `-1`, and so on. Anchor every
substitution narrowly; addresses and labels routinely contain the same character sequences.

**Caution — know when to stop.** The third variant is *not* clearly a bug: both addresses
are legitimate, so whether `0x279448` is a separate object or a struct field is a genuine
reverse-engineering question. We deliberately left it unfixed. **Rewriting a symbolization
you cannot justify is gaming the metric, not decompiling.** The test: can you state, from
evidence, why the original compiler saw it your way?

**Yield, reported honestly:** the bogus-address fix removed a real false-mismatch class but
promoted **no** function to 100% by itself (three near-misses moved <0.2%). The
offset-constant fix promoted exactly one function 99.72 → 100.0. Fix these because they
make the target *correct*, not because they are a match faucet.

---

## 3. Build fragility that hides for weeks

**`as` deletes its output file on error.** Combine that with a build step that
re-assembles *every* unit unconditionally (no `[ -f x.o ] ||` guard) and you get a silent,
delayed catastrophe: any file that fails to assemble loses its expected object on the next
full build, and the match gate then skips with a vague "expected stale" message.

Ours was masked for weeks because the incremental commands (`objdiff`, `verify_all`) never
re-assemble — stale-but-valid objects survived from an old build, so everything looked
green. It only surfaced when a full build ran.

**Rules:**
- Run a **clean** build periodically, not just incremental ones. Green incremental runs
  prove very little.
- Treat "gate skipped" as a **failure**, never as a pass. A skip is an untested claim.
- Check expected-object coverage explicitly (`count(units) == count(expected/*.o)`).

### PS2 assembler syntax gotchas
splat emits VU0 macro-mode operands that binutils rejects:
- accumulator: emitted `ACC`, binutils wants **`$ACC`**
- divide register: emitted `Q`, binutils wants **`$Q`**

Broadcast operands (`$vf8x`) *are* accepted. Normalize the special registers at assemble
time, anchored to the `v<op>` mnemonic — plenty of unrelated labels contain the letters
`ACC` (`.L0015ACC0`, `0xC26ACCCD`, `func_001CACC0`).

---

## 4. Compiler identification comes first

Read `.comment` in the boot ELF before choosing a toolchain. Two families dominate PS2:

**Metrowerks CodeWarrior (`mwccps2`)** — run under `wibo` on x86_64 Linux.
**ee-gcc** (Sony SDK builds) — commonly 2.9-991111-01.

A single game usually contains **both**: game code from the licensee's compiler, and SDK /
libc / libgcc objects from Sony's. Tell-tales for the ee-gcc regions:
`sd`/`ld $ra` (64-bit saves), `daddu rd,rs,zero` register moves, unfilled `jal;nop` slots,
`move s8,sp` frame pointers.

**Route per-file, not per-project.** We carry a `// COMPILER:` directive in each source
file. Several functions sat parked for a long time only because they were being attacked
with the wrong compiler — a batch of libgcc routines (`__divdi3`, `__udivdi3`, `__mulsf3`)
matched immediately once routed to ee-gcc.

**Recognize stock library code.** Large parts of the SDK region are unmodified newlib /
libgcc2.c / fp-bit.c. Reconstructing the canonical upstream implementation is usually an
exact match, not an approximation — far faster than decompiling instruction by instruction.

**Compiler *point releases* matter.** A later mwcc build (2.3.3 vs 2.3-991202) byte-matched
an entire family of functions the pinned build could not, differing only in one delay-slot
fill decision. Keep multiple point releases available and record which one each file needs.
But never switch the *project-wide* default to chase a few functions — it regresses
everything already matched.

---

## 5. The `// NEARMISS` mechanism (recommended, portable)

The biggest process win of the project.

**Problem:** a strict matching decomp only commits C that compiles byte-identical. Everything
else stays an opaque `INCLUDE_ASM` stub — byte-correct via the linker, but **useless as
ground truth** for a future native port, for documentation, or for the next agent.

**Mechanism:** commit body-correct near-miss C anyway, marked so byte-identity is untouched.
A file whose **first line** is `// NEARMISS <name> (...)` is treated exactly like a stub by
the build: not compiled, not an objdiff unit, excluded from `matched_code`. The linker fills
the function from the already-byte-matched disassembly, so **the ELF stays byte-identical**.

**Discipline that makes it trustworthy:**
- The header must state the **re-measured** percentage (never the agent's claim — inflated
  numbers are common), the compiler, and the precise divergence mechanism.
- Enforce a floor (we use 50%); below it, revert to the stub. Faithful-but-imperfect is
  valuable; a decode you can't vouch for is worse than nothing.
- Keep a single registry file, one row per function.
- Promotion path: reproduce byte-exact output, drop the marker, it becomes a normal unit.

**Why it matters:** it converts "unmatchable" from a dead end into documentation, and it
captures the *reason* each function resists — which is exactly what lets a later session
notice that a whole class of "walls" was misdiagnosed.

**Known-unmatchable classes worth harvesting this way:**
- **Jump-table dispatchers.** If the original consolidated all jump tables into one external
  rodata unit, a C `switch` makes the compiler emit its *own local* table, and the
  local-vs-external relocation counts as a mismatch no matter how the schedule lines up.
  No `-O`/pragma/inline-asm lever exists (we tested five hypotheses). They decode to perfectly
  readable `switch` statements at 89-98% — harvest, don't fight.
- **VU0/COP2 and MMI 128-bit SIMD** — often has no faithful C form at all.
- **Tail-call trampolines** (`j func` instead of `jal`) — gcc 2.9 has no
  `-fno-optimize-sibling-calls`; the C is trivially correct but can never match.

---

## 6. Codegen idioms worth knowing on any PS2 project

Full catalogue in `fanout/MATCHING_GUIDE.md`. The transferable ones:

**Delay-slot / branch-likely (both compilers).** A likely branch appears when the delay slot
can be filled with a *speculatable* op from the taken path. Determine which op the target put
in the slot, then reshape the C so that op is the first statement of that path.
- *mwcc:* a pure-ALU op (`li`/`addiu`/`sll`). For switch state machines: switch on a **local**
  copy of the state byte, ascending case labels, and write the advance using that local
  (`st + 1`), not a re-read.
- *ee-gcc:* a cheap **load** of a global through an **already-live** base register. A `lui`
  rematerialization is *not* speculatable — this exact distinction is what produced a bogus
  "wall" for us.

**Float compound assignment picks operand order (mwcc).** `x = x + y` loads the LHS first
(`add.s f0,f2,f0`); `x += y` loads the RHS first (`add.s f0,f0,f1`) — the CodeWarrior shape.
Nine sites converted took one function from 99.87 to 100.0.

**Scratchpad memory behaves as `volatile`.** PS2 scratchpad (`0x70000000`) accesses in the
original are never reordered or CSE'd across each other. When a residual is scratchpad
access *order*, declare them `volatile`.

**`volatile` is double-edged.** It can be *required* (to force an annul, or to reproduce a
dead-then-live double load) and simultaneously *fatal* (a volatile MEM can never be moved
into a delay slot, because the conflict check rejects it unconditionally). We hit a genuine
deadlock where the qualifier needed for one behaviour forbade the other. When you see this,
you have found a real wall — describe it at that level of precision.

**Switch lowering direction.** mwcc lowers `switch` to a **descending** compare chain. If the
target tests ascending, write `if`/`else if`, not a `switch`.

**Inverse-CSE.** If the target recomputes a subexpression while your build hoists it into a
callee-saved register (tell: the frame grows), inline the expression at every use site
*including inside the guarding condition* so it gets recomputed per path.

---

## 7. Orchestration notes (parallel agents)

- **One wave at a time.** The container daemon can wedge under concurrent `container run`
  storms. Batch all compiles for a wave into a single container invocation per image; never
  call it in a tight loop.
- **Agents inflate their scores.** Always re-measure server-side before committing. Our
  integrator recomputes every percentage and ignores the agent's number entirely.
- **Agents drop out.** When a slot dies (API error, rate limit), its functions are silently
  missing from the results. Diff the input list against the returned list and **requeue the
  gap** — do not record them as attempted, or they become permanently parked without ever
  having been tried.
- **Resumable runs pay for themselves.** Rate limits and 5xx errors are routine on long runs;
  make completed agents replay from cache so a retry costs nothing.
- **Give agents the escape hatch.** Ours are explicitly told to report a *structural* problem
  (e.g. "objdiff caps below 100 because the symbol has two entry points") rather than grinding
  against it. One agent diagnosing a splat symbol-table issue was worth more than the match
  it failed to get.

---

## 8. Legal hygiene (non-negotiable)

- The repo contains **only original code, tooling, and build scripts**.
- Everything disc-derived — ISO, boot ELF, disassembly, extracted assets — is generated
  **locally by each user from their own legal copy**, and is git-ignored. Never commit it,
  never paste it into an external service.
- Keep `.gitignore` ahead of the artifacts: `iso/`, `*.iso`, boot ELF patterns
  (`SLUS_*`/`SLES_*`/`SCUS_*`/`*.elf`), the asm output dir, extracted assets, scratch dumps.
- Sanity-check every commit for disc-derived content before it lands. Generated `src/*.c`
  written by you is fine; raw disassembly is not.
- The supported workflow is always "the user supplies their own disc." Do not help
  circumvent copy protection.

---

## 9. Suggested order of attack for a new project

1. **Characterize the boot ELF** — serial, SHA1, size; pin them and never silently change.
2. **Check for DWARF** (`readelf -S`, `--debug-dump=info`). Symbols present is a completely
   different (far easier) project than fully blind matching.
3. **Read `.comment`** to identify the compiler; expect a second compiler for the SDK region.
4. **Stand up the pipeline before the grind:** splat → assemble expected → compile → objdiff,
   and get *one trivial leaf function* to 100%. Prove the oracle end-to-end first.
5. **Audit the pipeline** with the checks in §1 and §3 — size guard, orphan reconciliation,
   clean-build check, expected-object coverage — *before* trusting any progress number.
6. **Sweep for invented symbols** (§2) early; every one is a permanently unmatchable function
   that will otherwise be misfiled as a compiler wall.
7. Then grind, harvesting near-misses (§5) as you go, and re-test parked classes periodically.
