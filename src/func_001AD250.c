// INCLUDE_ASM func_001AD250  (vram 0x001AD250, 264 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// game-task sub-state machine (func_001ACEC0 -> HERE). Dispatches on byte +9
// of the current task slot (slot ptr in scratchpad 0x70003B6C) via the
// 6-entry jr-table at 0x0026DCB0:
//   0: bring-up poll func_001AD360; when done -> state 5, clear +0xA/+0xB,
//      kick func_001AEDB0(0)
//   1: func_001AD4D0 (trampoline to the in-game frame machine func_001AE040)
//   2: func_001AD4E0   3: func_001AD740   4: func_001ADF00
//   5: end poll func_001ADF50; when done -> back to state 1, clear +0xA/+0xB
//
// DECOMP ATTEMPT 2026-06-09 — wall-blocked at 92.3% (switch form below).
// The jump-table lowering, jr dispatch, all six bodies, the early-epilogue
// b-with-lq-delay of case 0, and the paddub arg-zero all match. Residual
// diffs are documented walls (docs/PROGRESS.md):
//   1. Delay-slot fill: three `beqz; nop` (table guard, case 0's and case
//      5's call-result tests) — mwcc fills each with the following safe
//      `lui` (jtbl pair / $at 0x7000); CW 2.3.1 leaves nops. mwcc provably
//      fills only when a safe candidate exists (cf. matched func_001F0060,
//      where the candidate would change semantics) — here candidates exist.
//   2. Reloc-pair interleave: mwcc schedules `sll v1,v1,2` between the
//      jtbl lui/addiu pair; CW keeps the pair adjacent.
//
// Best attempt (92.3%, semantics verified):
//   extern int func_001AD360(void);
//   extern void func_001AD4D0(void), func_001AD4E0(void), func_001AD740(void);
//   extern void func_001ADF00(void);
//   extern int func_001ADF50(void);
//   extern void func_001AEDB0(int);
//   void func_001AD250(void) {
//       switch (*(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9)) {
//       case 0:
//           if (func_001AD360()) {
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 5;
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
//               func_001AEDB0(0);
//           }
//           break;
//       case 1: func_001AD4D0(); break;
//       case 2: func_001AD4E0(); break;
//       case 3: func_001AD740(); break;
//       case 4: func_001ADF00(); break;
//       case 5:
//           if (func_001ADF50()) {
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 1;
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
//           }
//           break;
//       }
//   }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AD250.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
