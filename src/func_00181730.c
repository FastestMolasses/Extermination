// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.91% NEARMISS).
// Fixes read from the original instructions:
// - The "FP register coloring" residual was not a compiler wall: the original
//   reads the transform result straight from the D_700038B0 vector each time
//   (no b0/b8 temporaries) and writes the two probe points through the
//   D_700038C0 / D_700038D0 extern arrays. Only the 20.5 + y sum is a local.
//   With that spelling mwcc 2.3.3 colors the constants as the original does.
// - sdatathreshold 8 keeps the 8-byte gp-relative double D_002754B8 in sdata.
//
// Two passes (one per float of the D_002754B8 pair): builds a point at the
// object's +0x38 x offset by -3.0 (arg1 == 0) or +3.0, transforms it by the
// matrix at arg0 + 0xD0 (func_001026A0), then calls func_0019A570 on the
// segment from (x, y + 20.5 - 5.0, z) to (x, y + 20.5 + 5.0, z). Returns 1 when
// that call is nonzero and the byte at +0x1A of the record pointed to by
// scratchpad 0x700031D0 is 0x34; otherwise 0 after both passes.

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

        D_700038C0[0] = D_700038B0[0];
        D_700038D0[0] = D_700038B0[0];
        t = 20.5f + D_700038B0[1];
        D_700038C0[1] = t - 5.0f;
        D_700038C0[2] = D_700038B0[2];
        D_700038D0[1] = 5.0f + t;
        D_700038D0[2] = D_700038B0[2];

        if (func_0019A570(&D_700038C0, &D_700038D0, 4, 0) != 0 &&
            *(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x34) {
            return 1;
        }

        i += 1;
        p += 1;
    } while (i < 2);

    return 0;
}
