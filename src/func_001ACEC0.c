// INCLUDE_ASM func_001ACEC0  (vram 0x001ACEC0, 328 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// the GAME TASK installed in frame-task slot 0 (replaces the boot task
// 0x001AB7E0). On entry stores 2 to scratchpad byte 0x70003B90, then
// dispatches on the task slot's state byte +8 (slot ptr in spad 0x70003B6C),
// compare order 3,2,1,0 (if-chain, no jr-table):
//   0: if func_001AD1A0() done: +8 = (gp-rel byte D_00275BE0 == 0) ? 1 : 2,
//      clear +9/+0xA; then func_001D1EF0() unconditionally.
//   1: if func_001AD230() (always returns 4): +8 = 3, clear +9/+0xA/+0xB.
//   2: +8 = 3 (stored via the slot+8 pointer computed at entry), +9 = 5,
//      clear +0xA/+0xB.
//   3: func_001AD250() (the sub-state machine).
//
// DECOMP ATTEMPT 2026-06-09 — wall-blocked at 88.8% (C below; gp-rel
// D_00275BE0 needs the default sdata threshold, hence no -sdatathreshold 0;
// the raw 0x7000xxxx scratchpad casts emit the same $at form regardless).
// Residual diffs are documented walls (docs/PROGRESS.md):
//   1. Branch inversion / dead-b: CW `beqz state, st0; nop; b ret-epilogue;
//      lq $ra (delay)`; mwcc inverts to `bnez state, ret` and drops the b.
//   2. Delay-slot address hoist: mwcc fills `bnez D_00275BE0`'s and two b's
//      delay slots with safe `lui $at, 0x7000` / `lq $ra`; CW leaves nops.
//   3. Head interleave: CW schedules `addiu a2, a0, 8` (the kept slot+8
//      pointer) BETWEEN the `lui $at`/`sb` of the 0x70003B90 store; mwcc
//      keeps lui/sb adjacent (statement reordering does not move it).
//
// Best attempt (88.8%, semantics verified):
//   extern int func_001AD1A0(void);
//   extern int func_001AD230(void);
//   extern void func_001AD250(void);
//   extern void func_001D1EF0(void);
//   extern unsigned char D_00275BE0;
//   void func_001ACEC0(void) {
//       char *slot;
//       unsigned char *statep;
//       unsigned char state;
//       slot = *(char * volatile *)0x70003B6C;
//       statep = (unsigned char *)(slot + 8);
//       *(volatile unsigned char *)0x70003B90 = 2;
//       state = *(unsigned char *)(slot + 8);
//       if (state == 3) goto st3;
//       if (state == 2) goto st2;
//       if (state == 1) goto st1;
//       if (state == 0) goto st0;
//       return;
//   st0:
//       if (func_001AD1A0()) {
//           if (D_00275BE0 == 0)
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 1;
//           else
//               *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 2;
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 0;
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
//       }
//       func_001D1EF0();
//       return;
//   st1:
//       if (func_001AD230()) {
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x8) = 3;
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 0;
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
//           *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
//       }
//       return;
//   st2:
//       *statep = 3;
//       *(unsigned char *)(*(char * volatile *)0x70003B6C + 0x9) = 5;
//       *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xA) = 0;
//       *(unsigned char *)(*(char * volatile *)0x70003B6C + 0xB) = 0;
//       return;
//   st3:
//       func_001AD250();
//       return;
//   }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001ACEC0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
