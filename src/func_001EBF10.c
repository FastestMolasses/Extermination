// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 56.72% NEARMISS).
// Fixes read from the original instructions:
//  - the random-fraction bias is 0.0001f, not 1e-6 (only the fourth float
//    argument is 1e-6); the body is the byte-matched twin func_001EBA20's
//    template (fraction /= 65535, seed advance, += 0.0001f), with D_00275C34
//    re-read for the +0x54 argument.
//  - the scratchpad words 0x700036D0..DC are elements 12..15 (row 3,
//    translation x/y/z/w) of the 4x4 float matrix D_700036A0, written as
//    float stores; -sdatathreshold 4 (D_00275C34 is gp-relative).
//
// func_001EBF10 (vram 0x001EBF10, 0x2D4 bytes).
// Loads identity into the scratchpad matrix D_700036A0 (func_001029C0), then
// three emit passes. Each pass sets the matrix translation to (X, arg0->+0x34,
// Z, 1.0) with (X, Z) = (370, 380), (345, 390), (395, 390); draws a [0,1)
// fraction from the LCG seed at D_00275C34->+0x04 (high 16 bits / 65535 +
// 0.0001, seed = seed*0x25 + 0xB); calls func_001CFB50 with D_0081F8F0, the
// matrix, D_00275C34->+0x54, the fraction, 1.0f, 1e-6f, 0.0f; then
// func_001CFBE0 with descriptor D_002565E0 (pass 1) or D_00256670 (passes 2-3).
extern int *D_00275C34;
extern int D_002565E0[2];
extern int D_00256670[2];
extern int D_0081F8F0[2];
extern float D_700036A0[16];

extern void func_001029C0(float *m);
void func_001CFB50(void *p, int b, void *c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EBF10(char *arg0, int arg1) {
    int r;
    float f;

    func_001029C0(D_700036A0);
    D_700036A0[12] = 370.0f;
    D_700036A0[13] = *(float *)(arg0 + 0x34);
    D_700036A0[14] = 380.0f;
    D_700036A0[15] = 1.0f;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, D_700036A0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_002565E0, D_0081F8F0, 0);

    D_700036A0[12] = 345.0f;
    D_700036A0[13] = *(float *)(arg0 + 0x34);
    D_700036A0[14] = 390.0f;
    D_700036A0[15] = 1.0f;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, D_700036A0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_00256670, D_0081F8F0, 0);

    D_700036A0[12] = 395.0f;
    D_700036A0[13] = *(float *)(arg0 + 0x34);
    D_700036A0[14] = 390.0f;
    D_700036A0[15] = 1.0f;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, D_700036A0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_00256670, D_0081F8F0, 0);
}
