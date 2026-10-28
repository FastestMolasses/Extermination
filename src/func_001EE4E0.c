// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 95.99% NEARMISS).
// Fixes read from the original instructions (order of the constant arguments
// of func_001CFB50):
//  - the first spawn sets up its 1e-6 and 10 arguments ahead of the 1.0: in the
//    original 1e-6 is computed from an int local (eps_i = 1000000; eps =
//    1.0f / (float)eps_i) and the scale is an int converted to float (idiom-24).
//  - the third spawn sets up its scale first: an int converted to float.
//  - the second spawn uses plain constants.
//
// Effect/animation setup, sibling of func_001EE190 / func_001EF1C0 (same template). Rewrites the
// 8-float parameter blocks at offset 0x20 of three 0x90-byte records (D_002576C0 / D_00257750 /
// D_002577E0) to 12/12/16/0/0/0/0/0 and twice 32/32/64/96/16/16/64/0, then for each record
// advances the LCG at D_00275C34[1] (r * 37 + 11), derives
// phase = ((r >> 16) & 0xFFFF) / 65535 + 1e-4, calls
// func_001CFB50(D_0081F8F0, 0, arg0, state->f54, phase, 1.0, 1e-6, 15.0) and
// func_001CFBE0(arg1, mode, record, D_0081F8F0, 1) with mode 1, 6, 1.
// Finally eases state->f8 toward 0.02 by a tenth of the gap and clamps it to a 0.02 floor.
// The parameter blocks are volatile float[8] so they stay absolute (per-word relocations) and are
// stored in address order; D_00275C34 stays gp-relative (-sdatathreshold 4).
extern int *D_00275C34;
extern int D_002576C0[2];
extern int D_00257750[2];
extern int D_002577E0[2];
extern int D_0081F8F0[2];
extern volatile float D_002576E0[8];
extern volatile float D_00257770[8];
extern volatile float D_00257800[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EE4E0(int arg0, int arg1) {
    int scale_i;
    float scale;
    int eps_i;
    float eps;
    int scale3_i;
    float scale3;
    int r;
    float f;
    float v;

    D_002576E0[0] = 12.0f; D_002576E0[1] = 12.0f; D_002576E0[2] = 16.0f; D_002576E0[3] = 0.0f; D_002576E0[4] = 0.0f; D_002576E0[5] = 0.0f; D_002576E0[6] = 0.0f; D_002576E0[7] = 0.0f;
    D_00257770[0] = 32.0f; D_00257770[1] = 32.0f; D_00257770[2] = 64.0f; D_00257770[3] = 96.0f; D_00257770[4] = 16.0f; D_00257770[5] = 16.0f; D_00257770[6] = 64.0f; D_00257770[7] = 0.0f;
    D_00257800[0] = 32.0f; D_00257800[1] = 32.0f; D_00257800[2] = 64.0f; D_00257800[3] = 96.0f; D_00257800[4] = 16.0f; D_00257800[5] = 16.0f; D_00257800[6] = 64.0f; D_00257800[7] = 0.0f;
    scale_i = 15;
    scale = (float)scale_i;
    eps_i = 1000000;
    eps = 1.0f / (float)eps_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, eps, scale);
    func_001CFBE0(arg1, 1, D_002576C0, D_0081F8F0, 1);
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 6, D_00257750, D_0081F8F0, 1);
    scale3_i = 15;
    scale3 = (float)scale3_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, scale3);
    func_001CFBE0(arg1, 1, D_002577E0, D_0081F8F0, 1);
    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
