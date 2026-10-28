// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 96.80% NEARMISS).
// Fixes read from the original instructions (order of the constant arguments
// of func_001CFB50):
//  - the first spawn sets up its 1e-6 and 10 arguments ahead of the 1.0: in the
//    original 1e-6 is computed from an int local (eps_i = 1000000; eps =
//    1.0f / (float)eps_i) and the scale is an int converted to float (idiom-24).
//  - the third spawn sets up all three constants early: its 10, its 1.0 and its
//    1e-6 all come from int locals (10 and 1 converted to float, 1e-6 as
//    1.0f / 1000000).
//  - the second spawn uses plain constants.
//
// Periodic particle/effect re-seed (sibling of func_001ED450). Rewrites three
// 8-word (0x20) global descriptor blocks at D_00257530, D_002575C0, D_00257650
// to their constant default vectors (24/24/48/0... and 96/96/192/96/64/64/160/0
// twice). Then for each of three sub-emitters: advances the global LCG at
// D_00275C34[1] (state = state*0x25 + 0xB), derives a normalized random scalar
// f = ((state>>16 & 0xFFFF)/65535) + 0.0001, primes the shared param buffer
// D_0081F8F0 via func_001CFB50(buf, 0, arg0, D_00275C34+0x54 timer, f, 1.0,
// 1e-6, 10.0), and submits the emitter for descriptor table D_00257510 /
// D_002575A0 / D_00257630 via func_001CFBE0(arg1, 1, table, buf, 1). Finally
// eases the global fade D_00275C34+8 toward 0.02 by a tenth each call and clamps
// it to a 0.02 floor.
//
// -sdatathreshold 4: D_00275C34 (gp-range pointer) stays gp-relative (matches
// CW); the init globals are declared volatile float[8] so they exceed the sdata
// threshold, stay absolute (per-word hi/lo relocs), and emit in address order.
extern int *D_00275C34;
extern int D_00257510[2];
extern int D_002575A0[2];
extern int D_00257630[2];
extern int D_0081F8F0[2];
extern volatile float D_00257530[8];
extern volatile float D_002575C0[8];
extern volatile float D_00257650[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EF1C0(int arg0, int arg1) {
    int scale_i;
    float scale;
    int eps_i;
    float eps;
    int scale3_i;
    float scale3;
    int unit3_i;
    float unit3;
    int eps3_i;
    float eps3;
    int r;
    float f;
    float v;

    D_00257530[0] = 24.0f; D_00257530[1] = 24.0f; D_00257530[2] = 48.0f; D_00257530[3] = 0.0f; D_00257530[4] = 0.0f; D_00257530[5] = 0.0f; D_00257530[6] = 0.0f; D_00257530[7] = 0.0f;
    D_002575C0[0] = 96.0f; D_002575C0[1] = 96.0f; D_002575C0[2] = 192.0f; D_002575C0[3] = 96.0f; D_002575C0[4] = 64.0f; D_002575C0[5] = 64.0f; D_002575C0[6] = 160.0f; D_002575C0[7] = 0.0f;
    D_00257650[0] = 96.0f; D_00257650[1] = 96.0f; D_00257650[2] = 192.0f; D_00257650[3] = 96.0f; D_00257650[4] = 64.0f; D_00257650[5] = 64.0f; D_00257650[6] = 160.0f; D_00257650[7] = 0.0f;

    scale_i = 10;
    scale = (float)scale_i;
    eps_i = 1000000;
    eps = 1.0f / (float)eps_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, eps, scale);
    func_001CFBE0(arg1, 1, D_00257510, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002575A0, D_0081F8F0, 1);

    scale3_i = 10;
    scale3 = (float)scale3_i;
    unit3_i = 1;
    unit3 = (float)unit3_i;
    eps3_i = 1000000;
    eps3 = 1.0f / (float)eps3_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, unit3, eps3, scale3);
    func_001CFBE0(arg1, 1, D_00257630, D_0081F8F0, 1);

    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
