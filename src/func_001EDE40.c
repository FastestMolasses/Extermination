// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 82.52% NEARMISS).
// Fixes read from the original instructions:
//  - the original sets up the spawn constants in a different order in each of
//    the three func_001CFB50 calls. First call: the 1e-6 and the 10.0 come
//    first and the 1.0 last; in the original source the 1e-6 is computed as
//    1.0 / (float)1000000 from an int local and the 10.0 is an int local
//    converted to float (idiom-24; both fold to constants but keep their early
//    setup). Second call: plain constants. Third call: only the 10.0 is an int
//    converted to float, which sets it up ahead of the 1.0 and the 1e-6.
//  - func_001CFBE0's last argument is 1, not 0 (the original copies it from the
//    register already holding the 1 of the second argument).
//  - the three default parameter blocks are 8-float volatile arrays at +0x20 of
//    the 0x90-byte emitter records; the ease toward 0.02 goes through a field
//    pointer.
//
// Periodic particle/effect re-seed. Rewrites three 8-float parameter blocks
// (D_00257530, D_002575C0, D_00257650, at +0x20 of the 0x90-byte emitter records
// D_00257510/D_002575A0/D_00257630) to their defaults, then for each emitter advances
// the PRNG word D_00275C34[1] (r*0x25+0xB), derives
// f = ((r>>16)&0xFFFF)/65535 + 0.0001, calls func_001CFB50(D_0081F8F0, 0, arg0,
// <field 0x54>, f, 1.0f, 1e-6f, 10.0f) and func_001CFBE0(arg1, 1, record,
// D_0081F8F0, 1). Finally eases the float at D_00275C34+8 toward 0.02 by 1/10
// and floors it at 0.02.
extern int *D_00275C34;
extern int D_0081F8F0[2];
extern char D_00257510[0x90];
extern char D_002575A0[0x90];
extern char D_00257630[0x90];
extern volatile float D_00257530[8];
extern volatile float D_002575C0[8];
extern volatile float D_00257650[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EDE40(int arg0, int arg1) {
    int r;
    float f;
    float v;
    int scale_i;
    float scale;
    int eps_i;
    float eps;
    int scale3_i;
    float scale3;

    D_00257530[0] = 16.0f; D_00257530[1] = 12.0f; D_00257530[2] = 16.0f; D_00257530[3] = 0.0f; D_00257530[4] = 0.0f; D_00257530[5] = 0.0f; D_00257530[6] = 0.0f; D_00257530[7] = 0.0f;
    D_002575C0[0] = 128.0f; D_002575C0[1] = 0.0f; D_002575C0[2] = 128.0f; D_002575C0[3] = 96.0f; D_002575C0[4] = 96.0f; D_002575C0[5] = 0.0f; D_002575C0[6] = 96.0f; D_002575C0[7] = 0.0f;
    D_00257650[0] = 128.0f; D_00257650[1] = 0.0f; D_00257650[2] = 128.0f; D_00257650[3] = 96.0f; D_00257650[4] = 96.0f; D_00257650[5] = 0.0f; D_00257650[6] = 96.0f; D_00257650[7] = 0.0f;

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
    func_001CFBE0(arg1, 1, D_00257510, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002575A0, D_0081F8F0, 1);

    scale3_i = 10;
    scale3 = (float)scale3_i;
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, scale3);
    func_001CFBE0(arg1, 1, D_00257630, D_0081F8F0, 1);
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
