// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 95.31% NEARMISS).
// Fixes read from the original instructions:
//  - -sdatathreshold 4 (D_00275C34 is gp-relative); each pass uses the sibling
//    template of func_001EBF10 (fraction /= 65535, seed advance, then += 0.0001f).
//  - the second pass sets up its 5.0 argument (in a GPR) ahead of the 1.0 and 1e-6
//    ones: in the original source that pass computes all three from int locals
//    (idiom-24): scale = (float)5, unit = (float)1, eps = 1.0 / (float)1000000, in
//    that order. The first pass uses plain constants.
//
// Two particle/effect emissions. Each advances the LCG seed at D_00275C34[1]
// (seed*0x25 + 0xB), derives a [0,1) fraction ((seed>>16)&0xFFFF)/65535 + 0.0001,
// calls func_001CFB50(D_0081F8F0, 0, arg0, D_00275C34->+0x54, fraction, 1.0f,
// 1e-6f, 5.0f), then func_001CFBE0(arg1, 1 then 0, D_00256790 then D_00256820,
// D_0081F8F0, 0).
extern int *D_00275C34;
extern int D_0081F8F0[2];
void func_001CFB50(void *p, int b, void *c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);
extern int D_00256790[2];
extern int D_00256820[2];

void func_001EC270(void *arg0, int arg1) {
    int r;
    float f;
    int scale_i;
    float scale;
    int unit_i;
    float unit;
    int eps_i;
    float eps;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 5.0f);
    func_001CFBE0(arg1, 1, D_00256790, D_0081F8F0, 0);

    scale_i = 5;
    scale = (float)scale_i;
    unit_i = 1;
    unit = (float)unit_i;
    eps_i = 1000000;
    eps = 1.0f / (float)eps_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, unit, eps, scale);
    func_001CFBE0(arg1, 0, D_00256820, D_0081F8F0, 0);
}
