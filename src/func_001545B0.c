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
// (a^2 sin^2 + b^2 cos^2) (a = 0.92*recZ, b = 0.92*recX; sin/cos =
// func_0011E2A8/func_0011DE90 — s45 trig-label fix; consistent with
// the engine's forward = (sin yaw, cos yaw)) compared against
// distXZ^2. Returns 1 inside.
//
// WALL-BLOCKED at 95.45% — 63/66 rows byte-identical from the C below
// (2026-06-10 attempt; one probe + one tail variant). The whole FP
// body matches on the FIRST try: f20-f24 callee-save allocation,
// every mov.s, the 0.92f lui/ori/mtc1, both jal-slot mul.s fills, the
// mula/mul/madd ACC interleave, the reloc PAIR for D_00248120 (s1
// destination = the pool-order datum's split-pair case), and the
// rec-stride sll/addu/sll (*20). The `goto inside` tail reproduces
// CW's `bc1t <epilogue>; addiu v0,1` likely-style slot fill exactly.
// Residual 3 rows, both catalogued walls:
// (a) dead-`b`-dup: CW keeps `b .L690; paddub v0,zero,zero` + the
//     DEAD `addiu v0,zero,1` (the dup'd return-1 block head); mwcc
//     coalesces to `paddub v0,zero,zero` falling into the epilogue.
//     Same family as func_00154460 / func_001860A0.
// (b) FP-stall nop: CW pads `mula.s f3,f3; nop; madd.s f0,f2,f2`
//     (ACC hazard); mwcc emits the pair back-to-back (its hazard
//     model only pads compare->bc1x, which it DID reproduce).
//
// extern unsigned char D_00248120[];
// extern float func_001B1240(unsigned char *origin, float x, float z);
// extern float func_0011E2A8(float a); /* sin — s45 trig-label fix */
// extern float func_0011DE90(float a); /* cos — s45 trig-label fix */
// (locals `c`/`s` below keep their pre-fix names: c holds sin(ang),
// s holds cos(ang). Names are codegen-neutral — left as-is to match
// the verified 95.45% attempt verbatim.)
// int func_001545B0(unsigned char *self, float x, float z) {
//     unsigned char *rec;
//     float ang, a, b, c, s, dx, dz, r2, d2;
//     rec = D_00248120 + self[0xD] * 20;
//     ang = func_001B1240(self + 0xB0, x, z);
//     a = 0.9200000166893005f * *(float *)(rec + 8);
//     b = 0.9200000166893005f * *(float *)(rec + 0);
//     a = a * a;
//     b = b * b;
//     c = func_0011E2A8(ang);
//     c = c * c;
//     s = func_0011DE90(ang);
//     s = s * s;
//     dx = x - *(float *)(self + 0xB0);
//     dz = z - *(float *)(self + 0xB8);
//     r2 = (a * b) / (a * c + b * s);
//     d2 = dx * dx + dz * dz;
//     if (d2 <= r2) goto inside;
//     return 0;
// inside:
//     return 1;
// }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001545B0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
