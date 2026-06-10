// INCLUDE_ASM func_001545B0  (vram 0x001545B0, 264 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): tendril-point validity test — is world point (f12 X,
// f13 Z) inside the ELLIPSE of semi-axes 0.92*recX (X) / 0.92*recZ
// (Z) centered on the actor origin +0xB0? rec = D_00248120 +
// self[0xD]*20. Heading from origin to the point via atan2
// func_001B1240, then ellipse radius-at-angle r^2 = (a^2 b^2) /
// (a^2 cos^2 + b^2 sin^2) (a = 0.92*recZ, b = 0.92*recX; sin/cos =
// func_0011DE90/func_0011E2A8) compared against distXZ^2. Returns
// 1 inside (same bc1t + dead-addiu return-bool tail as
// func_00154460 — same matching walls; no attempt made).
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001545B0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
