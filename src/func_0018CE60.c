// NEARMISS func_0018CE60  (vram 0x0018CE60, 0x4C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/list-scheduling permutation. mwcc233's scheduler defers the 'lw a2, 0x700031D0' pointer load until right before its first use (the lh at +0x1A) instead of hoisting it next to the adjacent float load from 0x700031B4 (source order already matches target exactly: float store firs...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_00102738(void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern int func_0019A910(void *a0, void *a1, int mode);

extern float D_700039C0[4];
extern float D_700039D0[4];
extern float D_700039E0[3];

void func_0018CE60(char *arg0, char *arg1, int arg2) {
    int mode;
    char *e;

    if (arg2 == 2) {
        mode = 7;
    } else {
        mode = 6;
    }

    func_00102948(&D_700039C0, arg1);
    D_700039D0[0] = D_700039C0[0];
    D_700039D0[1] = D_700039C0[1] - 200.0f;
    D_700039D0[2] = D_700039C0[2];
    D_700039D0[3] = 1.0f;

    if (func_0019A910(&D_700039C0, &D_700039D0, mode) != 0) {
        *(float *)0x70003A38 = *(float *)0x700031B4;
        e = *(char **)0x700031D0;
        if (*(short *)(e + 0x1A) & 0x5000) {
            if (arg2 != 2) {
                if (*(float *)(arg0 + 0x5C) == 1.0f) {
                    *(float *)0x70003A38 += 6.0f;
                } else {
                    *(float *)0x70003A38 += 17.0f;
                }
            } else {
                *(float *)0x70003A38 += 2.0f;
            }
        } else {
            D_700039D0[0] = 0.0f;
            D_700039D0[1] = 1.0f;
            D_700039D0[2] = 0.0f;
            D_700039D0[3] = 1.0f;
            D_700039E0[0] = *(float *)(e + 0x24);
            D_700039E0[1] = *(float *)(e + 0x28);
            D_700039E0[2] = *(float *)(e + 0x2C);
            *(float *)0x700039EC = 1.0f;
            *(float *)0x70003A34 = func_00102738(&D_700039D0, &D_700039E0);
            if (*(float *)0x70003A34 <= 0.17f) {
                goto skip1;
            }
            if (arg2 != 2) {
                if (*(float *)(arg0 + 0x5C) == 1.0f) {
                    *(float *)0x70003A38 += 6.0f;
                } else {
                    *(float *)0x70003A38 += 17.0f;
                }
            } else {
                *(float *)0x70003A38 += 2.0f;
            }
        }
    } else {
        *(float *)0x70003A38 = *(float *)(arg0 + 0x50) - 200.0f;
    }
skip1:

    D_700039D0[0] = D_700039C0[0];
    D_700039D0[1] = D_700039C0[1] + 200.0f;
    D_700039D0[2] = D_700039C0[2];
    D_700039D0[3] = 1.0f;

    if (func_0019A910(&D_700039C0, &D_700039D0, mode) != 0) {
        *(float *)0x70003A3C = *(float *)0x700031B4;
        e = *(char **)0x700031D0;
        if (*(short *)(e + 0x1A) & 0x8800) {
            *(float *)0x70003A3C -= 1.0f;
        } else {
            D_700039D0[0] = 0.0f;
            D_700039D0[1] = -1.0f;
            D_700039D0[2] = 0.0f;
            D_700039D0[3] = 1.0f;
            D_700039E0[0] = *(float *)(e + 0x24);
            D_700039E0[1] = *(float *)(e + 0x28);
            D_700039E0[2] = *(float *)(e + 0x2C);
            *(float *)0x700039EC = 1.0f;
            *(float *)0x70003A34 = func_00102738(&D_700039D0, &D_700039E0);
            if (*(float *)0x70003A34 <= 0.17f) {
                goto skip2;
            }
            *(float *)0x70003A3C -= 1.0f;
        }
    } else {
        *(float *)0x70003A3C = *(float *)(arg1 + 4) + 200.0f;
    }
skip2:

    if (*(float *)0x70003A38 > *(float *)0x70003A3C) {
        *(float *)0x70003A38 = *(float *)0x70003A3C - 3.0f;
    }

    *(float *)(arg0 + 0x50) = *(float *)0x70003A38;
    *(float *)(arg0 + 0x54) = *(float *)0x70003A3C;

    if (arg2 != 5) {
        if (*(float *)(arg0 + 0x14) < *(float *)(arg0 + 0x50)) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x50);
        }
        if (*(float *)(arg0 + 0x14) > *(float *)(arg0 + 0x54)) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x54);
        }
    }
}
