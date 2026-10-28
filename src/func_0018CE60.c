// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003A30 0x700031B4 0x700031D0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.60% NEARMISS).
// Fixes read from the original instructions:
// - Every scratchpad word is a relocated extern (idiom-32): the clamp
//   results 0x70003A34/38/3C as D_70003A30[1..3], 0x700031B4, the
//   collision-record pointer 0x700031D0 and the w lane of the query vector
//   as D_700039E0[3]. With bare address constants mwcc held the pointer load
//   behind the literal store and speculated the shared upper half into
//   branch delay slots that the original leaves empty.
//
// Behaviour: probes func_0019A910 along a vector 200 below and then 200 above
// the position copied from arg1 (mode 7 when arg2 == 2, else 6). On a hit the
// lower/upper limit starts from 0x700031B4 and gets an offset when the hit
// record's flags at +0x1A are set (0x5000 / 0x8800) or when func_00102738 of
// the up/down axis and the record's vector at +0x24 exceeds 0.17; on a miss
// it is arg0+0x50 - 200 / arg1+4 + 200. The pair is stored at arg0+0x50/0x54
// (lower capped at upper - 3) and, unless arg2 == 5, arg0+0x14 is clamped
// into it.

extern float func_00102738(void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern int func_0019A910(void *a0, void *a1, int mode);

extern float D_700039C0[4];
extern float D_700039D0[4];
extern float D_700039E0[4];
extern float D_70003A30[4];
extern float D_700031B4;
extern char *D_700031D0;

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
        D_70003A30[2] = D_700031B4;
        e = D_700031D0;
        if (*(short *)(e + 0x1A) & 0x5000) {
            if (arg2 != 2) {
                if (*(float *)(arg0 + 0x5C) == 1.0f) {
                    D_70003A30[2] += 6.0f;
                } else {
                    D_70003A30[2] += 17.0f;
                }
            } else {
                D_70003A30[2] += 2.0f;
            }
        } else {
            D_700039D0[0] = 0.0f;
            D_700039D0[1] = 1.0f;
            D_700039D0[2] = 0.0f;
            D_700039D0[3] = 1.0f;
            D_700039E0[0] = *(float *)(e + 0x24);
            D_700039E0[1] = *(float *)(e + 0x28);
            D_700039E0[2] = *(float *)(e + 0x2C);
            D_700039E0[3] = 1.0f;
            D_70003A30[1] = func_00102738(&D_700039D0, &D_700039E0);
            if (D_70003A30[1] <= 0.17f) {
                goto skip1;
            }
            if (arg2 != 2) {
                if (*(float *)(arg0 + 0x5C) == 1.0f) {
                    D_70003A30[2] += 6.0f;
                } else {
                    D_70003A30[2] += 17.0f;
                }
            } else {
                D_70003A30[2] += 2.0f;
            }
        }
    } else {
        D_70003A30[2] = *(float *)(arg0 + 0x50) - 200.0f;
    }
skip1:

    D_700039D0[0] = D_700039C0[0];
    D_700039D0[1] = D_700039C0[1] + 200.0f;
    D_700039D0[2] = D_700039C0[2];
    D_700039D0[3] = 1.0f;

    if (func_0019A910(&D_700039C0, &D_700039D0, mode) != 0) {
        D_70003A30[3] = D_700031B4;
        e = D_700031D0;
        if (*(short *)(e + 0x1A) & 0x8800) {
            D_70003A30[3] -= 1.0f;
        } else {
            D_700039D0[0] = 0.0f;
            D_700039D0[1] = -1.0f;
            D_700039D0[2] = 0.0f;
            D_700039D0[3] = 1.0f;
            D_700039E0[0] = *(float *)(e + 0x24);
            D_700039E0[1] = *(float *)(e + 0x28);
            D_700039E0[2] = *(float *)(e + 0x2C);
            D_700039E0[3] = 1.0f;
            D_70003A30[1] = func_00102738(&D_700039D0, &D_700039E0);
            if (D_70003A30[1] <= 0.17f) {
                goto skip2;
            }
            D_70003A30[3] -= 1.0f;
        }
    } else {
        D_70003A30[3] = *(float *)(arg1 + 4) + 200.0f;
    }
skip2:

    if (D_70003A30[2] > D_70003A30[3]) {
        D_70003A30[2] = D_70003A30[3] - 3.0f;
    }

    *(float *)(arg0 + 0x50) = D_70003A30[2];
    *(float *)(arg0 + 0x54) = D_70003A30[3];

    if (arg2 != 5) {
        if (*(float *)(arg0 + 0x14) < *(float *)(arg0 + 0x50)) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x50);
        }
        if (*(float *)(arg0 + 0x14) > *(float *)(arg0 + 0x54)) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x54);
        }
    }
}
