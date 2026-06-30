// NEARMISS func_00198AF0  (vram 0x00198AF0, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + delay-slot scheduling near-miss (identical 94.68% on both builds). Body, the 4-way state dispatch and all field writes are exact. Residual: the dispatch constant 3 colors to a2 instead of v1; mwcc speculatively duplicates `state+1` into the case-0 beqz delay slot (extra addiu)...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 94.7% (identical on pinned 991202 and mwcc 2.3.3). Body, the 4-way
// state dispatch, and all field writes are fully recovered; residual is pure
// register coloring + delay-slot/branch scheduling, not a logic gap.
//
// State machine for entity arg0 driven by source object arg1. Dispatches on the
// state byte arg0+1 (0..3), default = no-op return.
//   state 0: advance state (arg0+1 = state+1), clear arg0+2, fall into state 1.
//   state 1: if arg1+0x230 != 0x28 force state 3 and return; else run
//            func_00198930(arg0, arg1) and, if arg1+0x1F1 == 1, bump arg0+1.
//   state 2: if arg1+0x230 == 0x28 and arg1+0x1F1 == 2, force state 3.
//   state 3: reset transform scratch (arg0+0x30 from D_70003B50 identity; arg0+0x20
//            from arg1+0xA0), set arg0+0x24 = arg1[0xB4] + arg0[0x8C], build a
//            matrix (func_001029C0/func_00102C58), seed global vec 0x70003600 =
//            (0,0,-15.0f,1.0f), advance arg0+0x10.. by func_001026A0, then add the
//            +0x20.. deltas back into +0x10.. (with the +0x5C bias on Y), publish
//            both transforms (func_00102948 into D_008105E0/D_008105D0), clear
//            arg0+6, then arg0+1 = (arg0[5]==0 ? 3 : 0), clear arg0+2/+3 and
//            the short at arg0+8.
//
// Wall (not the clean-store nop): the dispatch constant 3 colors to a2 not v1,
// mwcc duplicates `state+1` into the case-0 branch delay slot, and the two simple
// `dst += src` float adds schedule their loads in the opposite order from the
// target. All register-coloring / scheduling permutation; logic is exact.
extern void func_001026A0(char *, void *, void *);
extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, char *);
extern void func_00198930(char *, char *);
extern int D_008105D0;
extern int D_008105E0;
extern int D_70003400;
extern int D_70003600;
extern int D_70003B50;

void func_00198AF0(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x230) != 0x28) {
            *(char *)(arg0 + 1) = 3;
            return;
        }
        func_00198930(arg0, arg1);
        if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
            *(char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        }
        return;
    case 2:
        if (*(int *)(arg1 + 0x230) == 0x28 && *(unsigned char *)(arg1 + 0x1F1) == 2) {
            *(char *)(arg0 + 1) = 3;
        }
        return;
    case 3:
        func_00102948(arg0 + 0x30, &D_70003B50);
        func_00102948(arg0 + 0x20, arg1 + 0xA0);
        *(float *)(arg0 + 0x24) = *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C);
        func_001029C0(&D_70003400);
        func_00102C58(&D_70003400, &D_70003400, arg0 + 0x30);
        *(int *)0x70003600 = 0;
        *(int *)0x70003604 = 0;
        *(int *)0x70003608 = 0xC1700000;
        *(int *)0x7000360C = 0x3F800000;
        func_001026A0(arg0 + 0x10, &D_70003400, &D_70003600);
        *(float *)(arg0 + 0x10) = *(float *)(arg0 + 0x20) + *(float *)(arg0 + 0x10);
        *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + (*(float *)(arg0 + 0x24) + *(float *)(arg0 + 0x5C));
        *(float *)(arg0 + 0x18) = *(float *)(arg0 + 0x28) + *(float *)(arg0 + 0x18);
        func_00102948(&D_008105E0, arg0 + 0x20);
        func_00102948(&D_008105D0, arg0 + 0x10);
        *(char *)(arg0 + 6) = 0;
        if (*(unsigned char *)(arg0 + 5) == 0) {
            *(char *)(arg0 + 1) = 3;
        } else {
            *(char *)(arg0 + 1) = 0;
        }
        *(char *)(arg0 + 2) = 0;
        *(char *)(arg0 + 3) = 0;
        *(short *)(arg0 + 8) = 0;
        break;
    }
}
