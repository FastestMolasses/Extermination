// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001ECA20
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 76.91% NEARMISS).
// Fixes read from the original instructions:
//  - compiled after its real same-TU predecessor func_001ECA20 (TU_STATIC;
//    tu_static.py removes the copy after checking it equals the original): the
//    order in which mwcc 2.3.3 sets up the float constants depends on what was
//    compiled earlier in the file (97.12% without it).
//  - 1.0f is one local, declared first and shared by the scale test and all
//    three func_001CFB50 calls; that puts the first call's 6.0f setup ahead of
//    its 1.0f, as in the original.
//  - arg0 is a pointer (the func_001CD520 destination is arg0 + 0x30); the
//    192/255 scaling reuses one product for both shifted words; each emit pass
//    uses the sibling template (fraction /= 65535, seed advance, += 0.0001f).
//  - the damping update and its 0.05 floor each go through a pointer to the
//    +0x08 field (read-modify-write, then a clamp written as a ternary store).
//
// GS/render-state pass for a pair of scene handles (arg0 = base struct, arg1 =
// submit index). If the global "scale" factor at D_00275C34+0x54 is below 1.0,
// builds a fade/alpha packet: computes 255/192-scaled byte components into the
// scratchpad words 0x70003600/04/08, packs them with alpha=0x80 into a 32-bit
// RGBA word, and pushes a scaled-radius quad via func_001CD520(mode=0, n=2,
// dst=arg0+0x30, packedConst=0x20045BA5154222DC, radius, radius, 6.0f, rgba).
// Then, three times, advances the LCG seed at D_00275C34+4 (seed*37+11),
// derives a 0..1 fraction from its high 16 bits, and submits a particle
// packet via func_001CFB50(&D_0081F8F0, 0, arg0, scale, frac+0.0001, 1.0,
// 1e-6, 6.0) followed by func_001CFBE0(arg1, mode, table, &D_0081F8F0, 0) with
// mode/table = (0,D_00256EE0), (2,D_00256EE0), (1,D_00256F70). Finally moves a
// damping factor at D_00275C34+8 a tenth of the way toward 0.05 and clamps it
// to a 0.05 floor.
static void func_001ECA20(int arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001ECA20.c"
extern int float_to_int(float);
extern void func_001CD520(int mode, int n, void *dst, long long packedConst, float f12, float f13, float f14, int packed);
extern unsigned char D_00256EE0[16];
extern unsigned char D_00256F70[16];

void func_001ECB00(char *arg0, int arg1) {
    float one = 1.0f;
    float *scaleP;
    int idx;
    float frac;

    scaleP = (float *)(D_00275C34 + 0x54);
    if (*scaleP < one) {
        float diff = one - *scaleP;
        float f;
        int packed;
        *(int *)0x70003600 = float_to_int(255.0f * diff);
        diff = 192.0f * diff;
        *(int *)0x70003604 = float_to_int(diff) << 8;
        *(int *)0x70003608 = float_to_int(diff) << 16;
        packed = (*(int *)0x70003608 | 0x80000000) | *(int *)0x70003604 | *(int *)0x70003600;
        f = 6.0f + 6.0f * *scaleP;
        func_001CD520(0, 2, arg0 + 0x30, 0x20045BA5154222DCLL, f, f, 6.0f, packed);
    }

    idx = *(int *)(D_00275C34 + 4);
    frac = (float)((idx >> 16) & 0xFFFF);
    frac /= 65535.0f;
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    frac += 0.0001f;
    func_001CFB50(&D_0081F8F0, 0, (int)arg0, *(float *)(D_00275C34 + 0x54),
                  frac, one, 1.0000000116860974e-06f, 6.0f);
    func_001CFBE0(arg1, 0, &D_00256EE0, &D_0081F8F0, 0);

    idx = *(int *)(D_00275C34 + 4);
    frac = (float)((idx >> 16) & 0xFFFF);
    frac /= 65535.0f;
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    frac += 0.0001f;
    func_001CFB50(&D_0081F8F0, 0, (int)arg0, *(float *)(D_00275C34 + 0x54),
                  frac, one, 1.0000000116860974e-06f, 6.0f);
    func_001CFBE0(arg1, 2, &D_00256EE0, &D_0081F8F0, 0);

    idx = *(int *)(D_00275C34 + 4);
    frac = (float)((idx >> 16) & 0xFFFF);
    frac /= 65535.0f;
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    frac += 0.0001f;
    func_001CFB50(&D_0081F8F0, 0, (int)arg0, *(float *)(D_00275C34 + 0x54),
                  frac, one, 1.0000000116860974e-06f, 6.0f);
    func_001CFBE0(arg1, 1, &D_00256F70, &D_0081F8F0, 0);

    {
        float *p = (float *)(D_00275C34 + 8);
        *p += (0.05f - *p) / 10.0f;
    }
    {
        float *p = (float *)(D_00275C34 + 8);
        *p = (*p < 0.05f) ? 0.05f : *p;
    }
}
