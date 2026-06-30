// NEARMISS func_00181730  (vram 0x00181730, 0x1A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register coloring on the (20.5+B4) temp: target colors the 5.0f constant into f1 / B4 into f0, mwcc 2.3.3 swaps the physical FPR assignment. Body byte-identical; pure FP-coloring permutation, permuter territory. Not the clean-store nop. 991202 worse (90.13%).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 95.91% (mwcc 2.3.3; 991202=90.13%). Body/structure byte-identical.
// Sole residual: FP register coloring on the (20.5 + B4) temp arithmetic --
// the target colors the 5.0f constant into f1 and B4 into f0; mwcc swaps which
// physical FPR holds the loaded B4 value vs. the loop-invariant constant. Pure
// FP-coloring permutation (idioms exhausted; sdatathreshold 8 needed so the
// 8-byte gp-rel double D_002754B8 lands in sdata while the D_700038xx vectors
// stay lui/%lo). Permuter territory.

extern void func_001026A0(void *, void *, void *);
extern int func_0019A570(void *, void *, int, int);

extern double D_002754B8;
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];

int func_00181730(char *arg0, int arg1) {
    double sp58;
    int i;
    float *p;
    float t;

    i = 0;
    p = (float *)&sp58;
    *(double *)&sp58 = D_002754B8;

    do {
        if (arg1 == 0) {
            *(float *)0x700038A0 = *(float *)(arg0 + 0x38) - 3.0f;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = *p;
            *(int *)0x700038AC = 0x3F800000;
        } else {
            *(float *)0x700038A0 = 3.0f + *(float *)(arg0 + 0x38);
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = *p;
            *(int *)0x700038AC = 0x3F800000;
        }

        func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);

        {
            float b0 = *(float *)0x700038B0;
            float b8;
            t = 20.5f + *(float *)0x700038B4;
            *(float *)0x700038C0 = b0;
            *(float *)0x700038D0 = b0;
            b8 = *(float *)0x700038B8;
            *(float *)0x700038C4 = t - 5.0f;
            *(float *)0x700038C8 = b8;
            *(float *)0x700038D4 = 5.0f + t;
            *(float *)0x700038D8 = b8;
        }

        if (func_0019A570(&D_700038C0, &D_700038D0, 4, 0) != 0 &&
            *(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x34) {
            return 1;
        }

        i += 1;
        p += 1;
    } while (i < 2);

    return 0;
}
