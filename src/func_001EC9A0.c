// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 80.61% NEARMISS).
// Fixes read from the original instructions:
//  - the original sets up the 1e-6 argument first, ahead of the 1.0: the source
//    computes it as 1.0 / (float)1000000 from an int local (idiom-24 shape; it
//    folds to the same constant but keeps its early setup). The old verdict
//    (an FP-constant scheduling wall) was wrong.
//  - -sdatathreshold 4 (was 8), as in the matched twin func_001EAF00.
//
// Single-call effect wrapper: func_001CFB50(&D_0081F8F0, 0, arg0, D_00275C34->+0x54 fade,
// D_00275C34->+0x5C, 1.0f, 1e-6f, 30.0f), then func_001CFBE0(arg1, 2, D_00256D30, &D_0081F8F0, 0).
// The frame saves $ra and $s0 (s0 = arg1 held across the first call).
extern int *D_00275C34;
extern int D_00256D30[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EC9A0(int arg0, int arg1) {
    int eps_i;
    float eps;

    eps_i = 1000000;
    eps = 1.0f / (float)eps_i;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, eps, 30.0f);
    func_001CFBE0(arg1, 2, D_00256D30, D_0081F8F0, 0);
}
