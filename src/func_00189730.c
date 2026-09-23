// func_00189730 -- byte-matched from C (objdiff 100%). Jump-table dispatcher: the
// compiled local .rodata table is pinned at its original address
// (tools/decomp/rodata_pin.py). Promoted from NEARMISS in the jr-table lane
// (2026-09-23): D_0024A410 is `int [][6]` and copy_qw4 is the real two-argument
// call; the old four-argument call was a register-colouring device.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// SEMANTICS: 7-state scripted-camera/anim state machine. p is the script state
// block (p[5] = state, p+0x28 = s16 timer/step counter, p+0x2A = s16 variant
// selector, p+0x38 = float per-frame delta), e is the actor/entity block
// (e[4] = active flag, e[0x1F0] = action id, e[0x275] = sub-action,
// e+0x20C = s16 anim id, e+0x3C = float distance). The state machine ramps the
// float at offset 0x74 of *(D_00275B40+4) (a camera/view field) down and back
// up, and in state 3 streams matrices out of D_0024A410/D_0024A3B0 into the
// scratchpad buffers D_700036A0/D_700036D0. Dropping e[4] out of state 1 resets
// the machine to state 0.

extern volatile int D_70003B68[16];                 /* PS2 scratchpad @ 0x70003B68 */

extern short D_00248B9C[8];
extern short D_00248C7C[8];
extern int D_0024A3B0[];
extern int D_0024A410[][6];
extern unsigned char *D_00275B40;
extern int D_700036A0[];
extern int D_700036D0[];

extern void copy_qw4(void *, void *);
extern int func_001026A0(int *, int, int *);
extern int func_00122BB8(void);
extern int func_001F4010(int, int *);

void func_00189730(unsigned char *p, unsigned char *e) {
    int st;
    int n;

    if (e[4] != 1) {
        p[5] = 0;
        *(int *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = 0;
    }
    st = p[5];
    switch (st) {
    case 0:
        if (e[0x275] == 2 && e[0x1F0] == 0x33) {
            p[5]++;
        }
        break;
    case 1:
        if (*(short *)(e + 0x20C) == D_00248B9C[0] || *(short *)(e + 0x20C) == D_00248C7C[0]) {
            p[5]++;
            *(short *)(p + 0x28) = 8;
            *(float *)(p + 0x38) = -0.07875798f;
        }
        break;
    case 2:
        n = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = n - 1;
        if (n == 0) {
            p[5] = p[5] + 1;
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = -0.63006383f;
            *(short *)(p + 0x28) = 0;
            *(short *)(p + 0x2A) = func_00122BB8() & 1;
        } else {
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) += *(float *)(p + 0x38);
        }
        break;
    case 3:
        if (*(short *)(p + 0x28) >= 6) {
            p[5] = st + 1;
        } else if ((D_70003B68[0] & 1) == 0) {
            *(short *)(p + 0x28) = *(short *)(p + 0x28) + 1;
            n = D_0024A410[*(short *)(p + 0x2A)][*(short *)(p + 0x28)];
            copy_qw4(D_700036A0, *(char **)(D_00275B40 + 8) + 0x90);
            func_001026A0(D_700036D0, *(int *)(D_00275B40 + 8) + 0x90, &D_0024A3B0[n * 4]);
            func_001F4010(8, D_700036A0);
        }
        break;
    case 4:
        if (*(float *)(e + 0x3C) <= 11.0f) {
            p[5] = st + 1;
            *(short *)(p + 0x28) = 8;
        }
        break;
    case 5:
        n = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = n - 1;
        if (n == 0) {
            p[5] = p[5] + 1;
            *(int *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) = 0;
        } else {
            *(float *)(*(unsigned char **)(D_00275B40 + 4) + 0x74) -= *(float *)(p + 0x38);
        }
        break;
    case 6:
        if (e[0x1F0] != 0x33) {
            p[5] = 0;
        }
        break;
    }
}
