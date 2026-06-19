// INCLUDE_ASM func_0015CF90  (vram 0x0015CF90, 108 bytes)
// PARKED — idiom-#13 delay-slot-fill WALL (analysis 2026-06-18, s81).
//
// The byte-identical machine code is supplied by fill_unmatched.py from the
// local splat .s; this stub keeps the rebuilt ELF byte-identical. Do NOT make
// this a compiled unit — the best-attempt C below reaches 96.3% (24/27 rows)
// but cannot reach 100% with mwccps2 2.3, so compiling it would break the
// byte-identical guarantee.
//
// Best-attempt C (compile -O4,p -sdatathreshold 0):
//   extern unsigned char D_00810706, D_00810707, D_008106B9;
//   extern float D_00810858, D_0081085C;
//   void func_0015CF90(unsigned char *a0) {
//       D_00810706 = a0[0x235];
//       D_00810707 = a0[0x234];
//       D_00810858 = *(float *)(a0 + 0x220);
//       D_0081085C = *(float *)(a0 + 0x228);
//       if (*(float *)(a0 + 0x220) <= 0.0f)
//           if (D_008106B9 == 0) D_008106B9 = 1;
//   }
//
// WHY IT WALLS (idiom #13 — see docs/PROGRESS.md):
//   Instruction SELECTION is byte-exact through 0x3C (the four snapshots, the
//   scheduler-hoisted `mtc1 zero,$f0` at 0x8, $f1 float temps). The only diff
//   is the two conditional-branch delay slots:
//     target:  bc1f .Lend; nop; lui at,%hi(D_008106B9); ...
//              bnez v1,.Lend; nop; li v1,1; ...
//     mwcc:    bc1f .Lend; lui at,%hi(...)   <- fills slot from fall-through
//              bnez v1,.Lend; li v1,1         <- fills slot from fall-through
//   CW 2.3.1 fills a branch delay slot ONLY from the taken path's first
//   instruction; mwcc 2.3 also fills from the fall-through whenever a SAFE
//   candidate exists. Here both fall-through candidates are inherently safe
//   to speculate (`lui` address-hi materialization; `li` of a constant), so
//   mwcc always fills them and CW's `nop`s are unreachable from C.
//
// FALSIFIED (all keep mwcc's fill or break selection):
//   -O4,p / -O3,p / -O4 / -O4,s / -O3,s  (fill both slots, 100 B)
//   -opt noschedule / -opt nointerleave  (ignored — still fill)
//   -O2 / -O2,p / -O1                    (no scheduler -> no mtc1 hoist; 0.0
//                                         materialized late into $f0, floats
//                                         in $f0 not $f1; wrong selection)
//   extern volatile D_008106B9           (ignored; identical to non-volatile)
//   *(volatile u8 *)&D_008106B9          (switches to lui/addiu GPR-address
//                                         form, 3 instr; still fills)
//   Idiom #18's volatile-load cure does NOT apply: the slot candidates here
//   are address-materialization / constant-load, not memory loads.
//
// Same family as func_001B57E0 (96.55%), func_001AB590 (97.87%),
// func_001AD250 (92.3%). Re-attempt only if a granular scheduler-off flag or
// a new idiom is found.
