// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 89.90% NEARMISS).
// Fixes read from the original instructions:
//  - the 15.0 and 1.0 spawn arguments are integer values converted to float in
//    locals (idiom-24): both in the first spawn (15 first, then 1), only the 15.0
//    in the second; that sets them up ahead of the 1e-6 argument as the original
//    does (same shape as func_001EB250). The old verdict (an FP register-colouring
//    wall) was wrong.
//
// Particle/effect spawn driver, same idiom family as the matched siblings
// func_001EB7F0 / func_001EBA20. Bracketed by audio/channel calls
// func_0021B9A0(chan, vol, pan): chan 2 then 3 (pan 2000.0f) before, chan 1
// after. Stores 20.0f to the object's +0x34 field, and decays its +0x38 field
// by 3.0 when the gp-relative fade scalar *(D_00275C34+0x54) is > 0. Then fires
// two identical passes: read the 32-bit LCG seed at *(D_00275C34+0x04), turn
// its high 16 bits into a [0,1) fraction (/65535.0f + 0.0001f bias), advance
// the seed (seed = seed*0x25 + 0xB), call func_001CFB50 with the shared block
// D_0081F8F0, the fade scalar, that fraction and the tuning constants, and
// queue via func_001CFBE0(arg1, 5, descriptor, block, 0) against D_00255D70
// then D_00255E00.
extern int *D_00275C34;
extern int D_00255D70[2];
extern int D_00255E00[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int chan, float a, float b);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB600(int arg0, int arg1) {
    int r;
    float f;
    int scale_i;
    int unit_i;
    float scale;
    float unit;
    int scale2_i;
    float scale2;

    if (!(*(float *)((char *)D_00275C34 + 0x54) <= 0.0f)) {
        *(float *)(arg0 + 0x38) -= 3.0f;
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);
    *(float *)(arg0 + 0x34) = 20.0f;

    scale_i = 15;
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
    func_001CFBE0(arg1, 5, D_00255D70, D_0081F8F0, 0);

    scale2_i = 15;
    scale2 = (float)scale2_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, scale2);
    func_001CFBE0(arg1, 5, D_00255E00, D_0081F8F0, 0);

    func_0021B9A0(1, 0.0f, 0.0f);
}
