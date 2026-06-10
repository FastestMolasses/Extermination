// INCLUDE_ASM func_001BA080  (vram 0x001BA080, 276 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001BA080.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// ---------------------------------------------------------------------------
// 2026-06-10 attempt: WALL-BLOCKED at 61/69 words (88.4%). Semantics fully
// understood — script opcode 0x06 (ftab_0024D880[0x06], interpreter
// func_001BA1F0): flag/counter ops on byte arrays D_00810758 (event flags)
// and D_008107D8 (script counters), index rec[+0x14], sub-op rec[+0x8]:
//   0/1 set flag 1/0xFF; 2 wait counter!=0; 3 counter=rec[+0x18] byte;
//   4 wait counter==rec[+0x18]; 5/6 counter++/--; >=7 return 1.
//
// Best shape (reproduce with the C below restored): dense `switch` on
// rec[2] + set-flag constants parked in the dead 1st param (idiom 16c) +
// case 6 falling OFF the switch end into `ret = 1; out: return ret;`
// (no explicit default — its return-1 folds into the guard's beql likely
// slot, and the fall-through kills mwcc's per-case `b`+li islands).
//   int ret; switch (rec[2]) {
//   case 0: self = 1;    D_00810758[rec[5]] = self; break;
//   case 1: self = 0xFF; D_00810758[rec[5]] = self; break;
//   case 2: if (D_008107D8[rec[5]] != 0) break; ret = 0; goto out;
//   case 3: D_008107D8[rec[5]] = *((unsigned char *)rec + 0x18); break;
//   case 4: if (D_008107D8[rec[5]] == rec[6]) break; ret = 0; goto out;
//   case 5: D_008107D8[rec[5]]++; break;
//   case 6: D_008107D8[rec[5]]--; }
//   ret = 1; out: return ret;
//
// Residual diffs = three known wall families (8 words):
//  (a) NEW DATUM — JUMP-TABLE DISPATCH ORDER: for a dense switch mwcc 2.3
//      emits [lui jtbl, sll idx, addiu %lo, addu]; CW 2.3.1 emitted
//      [lui, addiu %lo, sll, addu] (+0x14/+0x18 swap). Compiler-internal —
//      no source handle reaches inside switch lowering.
//  (b) wall #13 in cases 2/4: CW `bnez v0,.Lret1; nop; b .Lout; paddub
//      v0,zero,zero(slot)`; mwcc speculates the safe fall-through `paddub`
//      into the bnez slot and puts the nop on the `b` instead
//      (+0x7C/+0x84, +0xC0/+0xC8).
//  (c) shadow-fill interleave in case 3: CW splits the D_008107D8 address
//      pair around the value lbu ([lw idx][lui][lbu val][addiu][addu][sb]);
//      mwcc hoists the lbu above the whole pair (+0x8C/+0x90).
// ---------------------------------------------------------------------------
