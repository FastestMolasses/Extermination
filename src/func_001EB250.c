// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 99.90% NEARMISS).
// Fixes read from the original instructions:
//  - the 15.0 and 1.0 spawn arguments are integer values converted to float in
//    locals (idiom-24): both in the first spawn (15 first, then 1), only the 15.0
//    in the second; that sets them up ahead of the 1e-6 argument as the original
//    does. The two spawns are written out, not an inline helper.
//  - the final restore call takes both zero arguments from one chained
//    assignment (`hi = lo = 0.0f`), which materializes the zero in $f13 first.
//
// Spawns two muzzle/impact particle effects for a weapon (arg0) into render
// queue arg1. When the global timer *(D_00275C34+0x54) exceeds 0.8, decays the
// weapon's +0x38 field by 2.0. Calls func_0021B9A0 to set audio/voice params
// (channel 2 -> 0, channel 3 -> 2000.0). For each of the two effects it advances
// the global LCG-style counter at *(D_00275C34+4) (n = n*37 + 11) and feeds
// ((n>>16 & 0xFFFF)/65535 + 0.0001) as a randomized parameter to
// func_001CFB50(template D_0081F8F0, ...) then emits it via func_001CFBE0 with
// effect tables D_00255B30 / D_00255BC0. Finishes by resetting audio channel 1.
extern int *D_00275C34;
extern int D_00255B30[2];
extern int D_00255BC0[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int mode, float scale, float bias);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB250(int arg0, int arg1) {
    int r;
    float f;
    int scale_i;
    int unit_i;
    float scale;
    float unit;
    int scale2_i;
    float scale2;

    if (!(*(float *)((char *)D_00275C34 + 0x54) <= 0.8f)) {
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 2.0f;
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);
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
    func_001CFBE0(arg1, 5, D_00255B30, D_0081F8F0, 0);

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
    func_001CFBE0(arg1, 5, D_00255BC0, D_0081F8F0, 0);
    {
        float lo, hi;
        hi = lo = 0.0f;
        func_0021B9A0(1, lo, hi);
    }
}
