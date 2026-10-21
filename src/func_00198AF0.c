// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 94.68% NEARMISS).
// Camera action 14 (player code 0x28), state byte cam + 1:
//   0: advance, clear +2, fall into 1.
//   1: unless the player code (+0x230) is 0x28 go to state 3; else
//      func_00198930(cam, player) and advance when player + 0x1F1 == 1.
//   2: go to state 3 when the code is not 0x28 or player + 0x1F1 == 2.
//   3: rebuild the eye / target (D_70003B50, the player position, the matrix
//      D_70003400 and the offset {0, 0, -15, 1} at D_70003600), copy them to
//      D_008105E0 / D_008105D0, clear +6, set +1 to 3 (cam + 5 == 0) or 0, and
//      clear +2 / +3 / +8.
// Body correction from the original instructions: state 2's test is
// `code != 0x28 || flag == 2` (the old C had `code == 0x28 && flag == 2`); the
// port's em_camera_aim.c already follows the instructions there. Also: every
// case ends in `break` (idiom-27), the advance stores through an unsigned char,
// and the accumulates are `+=` (idiom-21).

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
            break;
        }
        func_00198930(arg0, arg1);
        if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
            *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x230) != 0x28 || *(unsigned char *)(arg1 + 0x1F1) == 2) {
            *(char *)(arg0 + 1) = 3;
        }
        break;
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
        *(float *)(arg0 + 0x10) += *(float *)(arg0 + 0x20);
        *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + (*(float *)(arg0 + 0x24) + *(float *)(arg0 + 0x5C));
        *(float *)(arg0 + 0x18) += *(float *)(arg0 + 0x28);
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
