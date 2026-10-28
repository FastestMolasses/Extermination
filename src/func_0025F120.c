// DATA, NOT CODE (found 2026-10-28 while decompiling the asm-only functions):
// this 4-byte entry sits in a regular table of 32-bit words around
// 0x0025F0C0..0x0025F15C (the upper bytes step by 1, the low half is
// 0x0001); splat split it as a function. It has no C form; reclassifying the
// region as data in config is a separate split change.
// INCLUDE_ASM func_0025F120  (vram 0x0025F120, 4 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_0025F120.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
