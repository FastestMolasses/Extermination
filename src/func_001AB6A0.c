// INCLUDE_ASM func_001AB6A0  (vram 0x001AB6A0, 160 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// frame-task dispatch. Walks the 3-slot task table at D_0028A750 (0x20
// bytes/slot; byte +0 = state, word +4 = task fn). States 1 ("just
// registered") and 4 ("resumed") are promoted to 2 ("running"); state 2
// runs (jalr *(slot+4)); anything else skips. The current slot pointer is
// parked in scratchpad 0x70003B6C so the running task (and
// func_001AB790/func_001AB7D0) can find its own slot.
//
// DECOMP ATTEMPT 2026-06-09 — wall-blocked at 78.5% (best, goto-shaped
// chain below). All residual diffs are documented walls (docs/PROGRESS.md):
//   1. Prologue reloc-pair interleave: CW schedules `sq $ra` AFTER the
//      lui/addiu pair of &D_0028A750; mwcc sinks the addiu below the sq.
//   2. Branch inversion / dead-b coalescing: CW emits
//      `beq state,4,promote; nop; b next; nop`; mwcc always inverts to
//      `bne state,4,next` when the target is the fall-through after the b
//      (every shape tried: if-chain, switch (72.6%), if/else (60.2%)).
//   3. Delay-slot address hoist: mwcc fills branch delay slots with the
//      safe `lui $at, 0x7000` materializations; CW leaves nops.
//   4. sltu-into-branch regalloc on the loop guard: CW `sltu $v1; bnez $v1`,
//      mwcc sinks into $at (cond-temp local falsified again).
// Opt sweep -O2..4 / ,p / ,s: identical or worse.
//
// Best attempt (78.5%, semantics verified):
//   extern char D_0028A750;
//   extern char D_0028A7B0;
//   void func_001AB6A0(void) {
//       char *slot;
//       unsigned char state;
//       *(char * volatile *)0x70003B6C = &D_0028A750;
//       do {
//           slot = *(char * volatile *)0x70003B6C;
//           state = *(unsigned char *)slot;
//           if (state == 2) goto run;
//           if (state == 1) goto promote;
//           if (state == 4) goto promote;
//           goto next;
//       promote:
//           *(unsigned char *)slot = 2;
//       run:
//           slot = *(char * volatile *)0x70003B6C;
//           (*(void (**)(void))(slot + 4))();
//       next:
//           slot = *(char * volatile *)0x70003B6C;
//           slot += 0x20;
//           *(char * volatile *)0x70003B6C = slot;
//       } while (slot < &D_0028A7B0);
//   }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AB6A0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
