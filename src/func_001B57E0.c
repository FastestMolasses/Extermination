// INCLUDE_ASM func_001B57E0  (vram 0x001B57E0, 116 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// per-frame INPUT read (step C of the main loop). Calls
// func_001B5F40(&D_00810E70, &D_00810E40) to unpack the raw libpad RPC
// landing buffer (0x810E40) into the frame input block (button halfwords
// 0x810E70/72/74/76/78/7A = current/pressed/released triples). On failure
// (pad not ready) neutralizes the block: all six button halfwords 0, both
// analog bytes D_00810E64/65 recentered to 0x80.
//
// DECOMP ATTEMPT 2026-06-09 — wall-blocked at 96.55%: every instruction
// matches EXCEPT one. CW leaves the `bnez $v0` delay slot as nop; mwcc 2.3
// fills it with the body's first `lui $at, %hi(D_00810E70)` (safe
// candidate). Documented delay-slot-fill wall: mwcc fills whenever a safe
// candidate exists (matched funcs like func_001F0060 only kept their nop
// because the candidate would have changed semantics). Falsified here:
// result-in-local, early-return shape, volatile stores, -O2/3/4 x p/s
// (level 2 loses the prologue interleave entirely), pragma
// scheduling/peephole off (ignored).
//
// Best attempt (96.55%, semantics verified):
//   extern unsigned short D_00810E70, D_00810E72, D_00810E74;
//   extern unsigned short D_00810E76, D_00810E78, D_00810E7A;
//   extern unsigned char D_00810E64, D_00810E65;
//   extern char D_00810E40;
//   extern int func_001B5F40(unsigned short *out, char *raw);
//   void func_001B57E0(void) {
//       if (func_001B5F40(&D_00810E70, &D_00810E40) == 0) {
//           D_00810E70 = 0;
//           D_00810E72 = 0;
//           D_00810E74 = 0;
//           D_00810E76 = 0;
//           D_00810E78 = 0;
//           D_00810E7A = 0;
//           D_00810E64 = 0x80;
//           D_00810E65 = 0x80;
//       }
//   }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001B57E0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
