// NEARMISS func_001D66A0 (0x001D66A0,0x28C bytes); assembly-backed.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Original GS Gouraud fan, encoded as a triangle strip with alternating
// center/outer vertices. PRIM4C selects untextured Gouraud additive output.
// The33 center/outer pairs produce32 visible triangles. This is used by
// the ITEM analog-stick glow, not a particle-emitter initialization.
//
// D275670 is a pointer to the draw context. The selected cursor is at
// context+0x10+4*slot; the previous source incorrectly treated the global
// as an array and scaled its slot twice. Corrected C measures68.56%
// with mwcc233/sdatathreshold0 (prior67.06%). The raw original
// packet geometry is independently checked by the native port's
// test_item_trail_reference.py at explicit SDK transcendental boundaries.
// The source retains separate scalar expressions for the target's MAC
// instructions; no claim that those expressions compile to identical words.
extern float func_0011E2A8(float x);
extern float func_0011DE90(float x);
extern int float_to_int(float x);
extern char *D_00275670;

char *func_001D66A0(int arg0, float *arg1, int *arg2, int *arg3, float fparg0) {
    float az, aw;
    float sinA, cosA, sinStep, cosStep;
    float f21, f22, f23, f24;
    float k;
    char *ctx;
    char *rec;
    char *cur;
    int i;

    az = arg1[2];
    aw = arg1[3];
    sinA = func_0011E2A8(fparg0);
    cosA = func_0011DE90(fparg0);
    sinStep = func_0011E2A8(0.09817477f);
    cosStep = func_0011DE90(0.09817477f);

    ctx = D_00275670 + arg0 * 4;
    k = 2.0f * sinStep;

    rec = *(char **)(ctx + 0x10);
    rec[3] = 0x10;
    *(int *)(rec + 4) = 0;
    *(short *)(rec + 0) = 0x86;
    *(char **)(ctx + 0x10) = rec + 0x870;

    f21 = az * cosA;
    f23 = cosStep * (aw * sinA) + (az * cosA) * sinStep;  /* ACC multiply, then multiply-add */
    f22 = az * sinA;
    f24 = cosStep * (aw * cosA) - (az * sinA) * sinStep;  /* ACC multiply, then multiply-sub */

    *(int *)(rec + 0x10) = 0;
    *(int *)(rec + 0x14) = 0;
    *(int *)(rec + 0x18) = 0;
    *(int *)(rec + 0x1C) = 0x50000085;
    *(int *)(rec + 0x20) = 0x8021;
    *(int *)(rec + 0x24) = 0x40264000;
    *(int *)(rec + 0x28) = 0x4141;
    *(int *)(rec + 0x2C) = 0;

    cur = rec + 0x30;
    i = 0;
    do {
        *(int *)(cur + 0x00) = arg2[0];
        *(int *)(cur + 0x04) = arg2[1];
        *(int *)(cur + 0x08) = arg2[2];
        *(int *)(cur + 0x0C) = arg2[3];
        *(int *)(cur + 0x20) = arg3[0];
        *(int *)(cur + 0x24) = arg3[1];
        *(int *)(cur + 0x28) = arg3[2];
        *(int *)(cur + 0x2C) = arg3[3];
        *(int *)(cur + 0x10) = float_to_int(16.0f * arg1[0]);
        *(int *)(cur + 0x14) = float_to_int(16.0f * arg1[1]);
        *(int *)(cur + 0x18) = 0xFFFFFF;
        *(int *)(cur + 0x1C) = 0;
        *(int *)(cur + 0x30) = float_to_int(16.0f * ((0.8f * f21) + arg1[0]));
        *(int *)(cur + 0x34) = float_to_int(16.0f * ((0.5f * f22) + arg1[1]));
        *(int *)(cur + 0x38) = 0xFFFFFF;
        *(int *)(cur + 0x3C) = 0;

        f21 -= k * f23;
        f22 -= k * f24;
        i += 1;
        cur += 0x40;
        f23 += k * f21;
        f24 += k * f22;
    } while (i < 0x21);

    return cur;
}
