// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 98.88% NEARMISS).
// Fixes read from the original instructions (order of the constant arguments
// of func_001CFB50):
//  - the second spawn sets up its 1e-6 and 10 arguments ahead of the 1.0: in
//    the original 1e-6 is computed from an int local (eps_i = 1000000; eps =
//    1.0f / (float)eps_i) and the 10 is an int converted to float (idiom-24).
//    The first and third spawns use plain constants.
//
// Initializes two 8-float global parameter blocks (D_00257380[], D_002574A0[]),
// then runs three "emit particle/clip" iterations: each advances the
// PRNG word at D_00275C34[1] (r = r*0x25 + 0xB), derives a normalized float
// f = ((r>>16)&0xFFFF)/65535 + 0.0001, and calls func_001CFB50(D_0081F8F0, 0,
// arg0, *(float*)(D_00275C34+0x54), f, 1.0f, 1e-6f, 10.0f) followed by
// func_001CFBE0(arg1, 1, <table>, D_0081F8F0, 1) with table D_00257360 /
// D_00257870 / D_00257480 for iterations 1/2/3. Finally it relaxes the float at
// D_00275C34+8 toward 0.02 by 1/10 each call, then floors it up to 0.02.
// (Twin of func_001EAD70, but with 3 iterations + the header block + trailing clamp.)
extern int *D_00275C34;
extern int D_0081F8F0[2];
extern float D_00257380[8];
extern float D_002574A0[8];
extern int D_00257360[2];
extern int D_00257870[2];
extern int D_00257480[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EEBA0(int arg0, int arg1) {
    int scale_i;
    float scale;
    int eps_i;
    float eps;
    int r;
    float f;
    float v;

    D_00257380[0] = 8.0f;
    D_00257380[1] = 24.0f;
    D_00257380[2] = 8.0f;
    D_00257380[3] = 0.0f;
    D_00257380[4] = 0.0f;
    D_00257380[5] = 0.0f;
    D_00257380[6] = 0.0f;
    D_00257380[7] = 0.0f;
    D_002574A0[0] = 24.0f;
    D_002574A0[1] = 112.0f;
    D_002574A0[2] = 24.0f;
    D_002574A0[3] = 128.0f;
    D_002574A0[4] = 24.0f;
    D_002574A0[5] = 112.0f;
    D_002574A0[6] = 24.0f;
    D_002574A0[7] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257360, D_0081F8F0, 1);

    scale_i = 10;
    scale = (float)scale_i;
    eps_i = 1000000;
    eps = 1.0f / (float)eps_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, eps, scale);
    func_001CFBE0(arg1, 1, D_00257870, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257480, D_0081F8F0, 1);

    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)((char *)D_00275C34 + 8) = v;
}
