// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.31% NEARMISS).
// Fixes read from the original instructions:
//  - the second spawn sets up its 1.0 and 15.0 arguments ahead of the 1e-6 one
//    (the first spawn does not): in the original both are integer values
//    converted to float in locals (idiom-24), 15 first, then 1. The old verdict
//    (an FP register-colouring wall) was wrong.
//
// Brackets two identical particle/effect spawn passes with func_0021B9A0 audio/
// channel calls (chan 2 off, chan 3 set to 2000.0, ... chan 1 off at the end).
// Each pass reads the gp-relative LCG seed at *(D_00275C34+0x04), turns its high
// 16 bits into a [0,1) fraction via /65535.0f + 0.0001f bias, advances the seed
// (seed = seed*0x25 + 0xB), then calls func_001CFB50 with the shared sprite/anim
// block D_0081F8F0, the fade scalar at *(D_00275C34+0x54), the random fraction,
// and the unit/epsilon/15.0 tuning constants; finally queues via func_001CFBE0
// against a per-pass descriptor (D_00255C50 then D_00255CE0). D_00275C34 is
// gp-relative (sdatathreshold 4); D_0081F8F0 / D_00255C50 / D_00255CE0 stay
// absolute.
extern int *D_00275C34;
extern int D_00255C50[2];
extern int D_00255CE0[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int chan, float a, float b);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB440(int arg0, int arg1) {
    int r;
    float f;
    int scale_i;
    int unit_i;
    float scale;
    float unit;

    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 5, D_00255C50, D_0081F8F0, 0);

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
    func_001CFBE0(arg1, 5, D_00255CE0, D_0081F8F0, 0);

    func_0021B9A0(1, 0.0f, 0.0f);
}
