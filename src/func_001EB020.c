// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 95.53% NEARMISS).
// Fixes read from the original instructions:
//  - the third spawn sets up its 1.0 and 9.0 arguments ahead of the 1e-6 one
//    (the first two spawns do not): in the original both are integer values
//    converted to float in locals (idiom-24), 9 first, then 1.
//
// Spawns three particle effects for object arg0 into queue arg1. For each one
// it reads the global LCG seed at *(D_00275C34+4), turns its high 16 bits into
// a fraction (/65535 + 0.0001 bias), advances the seed (seed*37 + 11), calls
// func_001CFB50(D_0081F8F0, 0, arg0, fade at +0x54, fraction, 1.0, 1e-6, 9.0)
// and queues it with func_001CFBE0(arg1, 1, table, D_0081F8F0, 0) against the
// tables D_00255980, D_00255A10 and D_00255AA0.
extern int *D_00275C34;
extern int D_00255980[2];
extern int D_00255A10[2];
extern int D_00255AA0[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB020(int arg0, int arg1) {
    int r;
    float f;
    int scale_i;
    int unit_i;
    float scale;
    float unit;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 9.0f);
    func_001CFBE0(arg1, 1, D_00255980, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 9.0f);
    func_001CFBE0(arg1, 1, D_00255A10, D_0081F8F0, 0);

    scale_i = 9;
    scale = (float)scale_i;
    unit_i = 1;
    unit = (float)unit_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, unit, 9.999999974752427e-07f, scale);
    func_001CFBE0(arg1, 1, D_00255AA0, D_0081F8F0, 0);
}
