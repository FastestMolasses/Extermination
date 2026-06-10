// INCLUDE_ASM func_00154460  (vram 0x00154460, 324 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): tendril-field trigger gate. Returns 1 iff gameplay
// frame (spad 0x70003B8D == 0) AND the player (D_008102B0 +0xA0/A4/
// A8) is within 3*recX / 3+recY / 3*recZ of the actor origin +0xB0,
// rec = D_00248120 + self[0xD]*20 (PARENT pad kind footprint).
//
// DECOMP ATTEMPT 2026-06-10 — wall-blocked at 67.5% structural
// (semantics verified). Best attempt:
//   extern float D_008102B0[];  extern GenRec D_00248120[];  // x,y,z,f3,f4
//   extern float func_0011DF78(float);  // fabs
//   int func_00154460(unsigned char *self) {
//       float *plr = D_008102B0; unsigned char *s = self; float d;
//       if (*(volatile unsigned char *)0x70003B8D) return 0;
//       d = func_0011DF78(plr[0x28] - *(float *)(s + 0xB0));
//       if (!(d <= 3.0f * D_00248120[s[0xD]].x)) return 0;
//       d = func_0011DF78(plr[0x2A] - *(float *)(s + 0xB8));
//       if (!(d <= 3.0f * D_00248120[s[0xD]].z)) return 0;
//       d = func_0011DF78(plr[0x29] - *(float *)(s + 0xB4));
//       return (d <= 3.0f + D_00248120[s[0xD]].y) ? 1 : 0;
//   }
// Three mwcc-policy walls (recorded in FINDINGS s33): (a) CW lowers
// each early `return 0` as `bc1t skip; nop` + `b common-epilogue;
// v0=0 in slot`; mwcc fills the bc1t slot with the v0=0 and inlines
// the epilogue — 1 instr shorter per site; (b) CW materializes the
// D_00248120 lui/addiu pair BEFORE the `lbu kind` reload, mwcc
// after; (c) final `return cond;`: CW bc1t + dead `addiu v0,1`
// trailer, mwcc bc1f (ternary and if/return spellings identical).
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00154460.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
