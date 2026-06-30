# Near-miss decompilations (readable C, NOT byte-identical)

This registry lists functions committed as **readable C that does not compile byte-identical**
to the original. Each is a genuine compiler artifact (register coloring, instruction scheduling,
branch-likely, frame-stride, etc.) that **no source change can fix** under the matching compiler —
the function's **logic and structure are faithful**, only the noted artifact differs.

## Why these exist

The decomp's primary goal is a maximally-faithful, byte-identical rebuild. For functions that are
genuinely wall-bound, an opaque `// INCLUDE_ASM` stub gives byte-identity but **zero readable source**
— useless as ground truth for the native-port effort. A `// NEARMISS` file instead commits the
readable, logically-faithful C **and** keeps the boot ELF byte-identical:

- The file's first line is `// NEARMISS <func> (...)`; `build.py` / `fill_unmatched.py` treat it like
  a stub — it is **not compiled, not an objdiff unit**, and **does not affect `matched_code`** (which
  continues to measure only true byte-match attempts).
- The linker fills the function's bytes from the **splat `.s`** (byte-identical), never from this C.
- Each file's header documents its **objdiff %** (best plain-C, reproducible via its `// COMPILER` /
  `// CFLAGS`) and the **precise divergence reason**.

To convert a near-miss to a true match later, reproduce the byte-exact bytes from C and drop the
`// NEARMISS` marker (it becomes a normal compiled unit at 100%).

## Registry

| Function | vram | bytes | objdiff % | compiler | divergence (compiler artifact) |
|---|---|---|---|---|---|
| func_00114360 | 0x00114360 | 228 | 89.82% | ee-gcc 2.9 | GPR coloring: `st` pointer in $v1 (orig) vs $v0 (ours); allocator deterministic |
