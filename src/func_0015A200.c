// INCLUDE_ASM func_0015A200  (vram 0x0015A200, 184 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (FINDINGS "GENERATOR" / "KIND-0xE COMPANION RESOLVED"):
// generator child spawn helper — allocates a class-2 actor and wires a
// kind-0xD worm (brain func_00153F10) or kind-0xE tendril field (brain
// func_001546C0) off a generator pad: +0x03 = kind byte, +0x0D =
// parent pad kind (D_00248120 footprint rec), +0x2E = pair index, pos
// +0xB0 copied verbatim from the pad origin (vec4 copy func_00102948),
// +0xC0/C4/C8 zeroed, +0x20 = parent's +0x14 canonical pointer.
// Returns 1 on spawn, 0 if the actor pool is full.
//
// WALL-BLOCKED at 91.30% (2026-06-10): best attempt below reproduces
// all 43 live rows byte-identically — bare params + alloc-first give
// CW's exact saved-register mapping (parent->s1, kind->s3,
// pairIdx->s2, e->s0) with `sq s0` in the jal slot, and the
// `goto nospawn` shape gets `beqz s0,<epilogue>` with the
// `paddub v0,zero,zero` slot fill. Residuals are catalogued walls:
// (a) dead-`b`-dup — CW keeps the dead return-0 block (`b .L29C;
// sw v1,0x20` + dead `paddub v0,zero,zero`); mwcc coalesces it at
// O3/O4 (return-1 falls into the epilogue, 2 rows short); -O2,p keeps
// the block but unfills EVERY delay slot (much worse) — the exact
// trade documented in "CONFIRMED COMPILER WALLS"; (b) prologue
// interleave — CW emits `li a0,2` between the paddub saves of a0 and
// a1 (frees a0 eagerly); mwcc emits all three paddub saves first
// (one swapped row; a `cls = 2;` first statement does not move it).
//
// int func_0015A200(unsigned char *parent, int kind, int pairIdx) {
//     int cls;
//     unsigned char *e;
//     cls = 2;
//     e = func_001AFA90(cls);
//     if (e == 0) goto nospawn;
//     e[0x3] = kind;
//     e[0xD] = parent[0x54];
//     e[0x9A] = 0;
//     *(short *)(e + 0x2E) = pairIdx;
//     func_00102948(e + 0xB0, parent + 0xB0);
//     *(int *)(e + 0xC0) = 0;
//     *(int *)(e + 0xC4) = 0;
//     *(int *)(e + 0xC8) = 0;
//     if (kind == 0xD) *(int (**)())(e + 0x10) = func_00153F10;
//     else             *(int (**)())(e + 0x10) = func_001546C0;
//     *(int *)(e + 0x20) = *(int *)(parent + 0x14);
//     return 1;
// nospawn:
//     return 0;
// }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_0015A200.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
