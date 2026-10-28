// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 66.71% NEARMISS).
// Fixes read from the original instructions:
//  - each pass uses the sibling template of func_001EBF10 (fraction /= 65535,
//    seed advance, then += 0.0001f).
//  - the second pass sets up its 15.0 argument (in a GPR) ahead of the 1.0 and
//    1e-6 ones: in the original source that pass computes all three from int
//    locals (idiom-24): scale = (float)15, unit = (float)1, eps = 1.0 / (float)1000000,
//    in that order. The other pass(es) use plain constants.
//
// Spawns two particle effects for object arg0 into queue arg1. Each pass advances the
// global LCG seed at D_00275C34[1] (seed*37 + 11), turns its old high 16 bits into a
// fraction (/65535 + 0.0001), calls func_001CFB50(&D_0081F8F0, 0, arg0, fade at +0x54,
// fraction, 1.0, 1e-6, 15.0) and queues it with func_001CFBE0(arg1, 1, table,
// &D_0081F8F0, 1) against the tables D_00256940 and D_002569D0.
extern int *D_00275C34;
extern int D_00256940[2];
extern int D_002569D0[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EC470(int arg0, int arg1) {
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
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_00256940, D_0081F8F0, 1);

    scale_i = 15;
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
    func_001CFBE0(arg1, 1, D_002569D0, D_0081F8F0, 1);
}
