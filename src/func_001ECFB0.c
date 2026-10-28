// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// TU_STATIC: func_001ECEF0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 67.36% NEARMISS).
// Fixes read from the original instructions:
//  - compiled after its real predecessor func_001ECEF0 (same-TU static copy, see
//    TU_STATIC; tu_static.py removes it after checking it equals the original),
//    so the float-constant setup order follows the original file.
//  - the slot pointer is a char pointer stepped by 0x90 and the loop counter is
//    declared first (saved-register order); the PRNG word is D_00275C34[1] and the
//    random value is built in split steps (divide, store the next seed, add bias).
//  - the ease toward 0.02 is an in-place update through a field pointer.
//
// Runs three particle/emitter slots from the table at D_002571B0 (stride 0x90).
// Each iteration advances the PRNG word D_00275C34[1] (n*37+11), derives the
// normalized float ((n>>16)&0xFFFF)/65535 + 0.0001, and calls
// func_001CFB50(&D_0081F8F0, 0, arg0, <field 0x54>, frac, 1.0f, 1e-6f, 3.0f)
// then func_001CFBE0(arg1, 1, slot, &D_0081F8F0, 0). Finally eases the float at
// D_00275C34+8 toward 0.02 (delta/10) and clamps it to >= 0.02.
static void func_001ECEF0(int arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001ECEF0.c"
extern int D_002571B0[0x100];

void func_001ECFB0(int arg0, int arg1) {
    int i;
    char *slot;
    int r;
    float f;
    float v;

    slot = (char *)D_002571B0;
    for (i = 0; i < 3; i++) {
        r = D_00275C34[1];
        f = (float)((r >> 0x10) & 0xFFFF);
        f /= 65535.0f;
        D_00275C34[1] = r * 0x25 + 0xB;
        f += 0.0001f;
        func_001CFB50(D_0081F8F0, 0, arg0,
                      *(float *)((char *)D_00275C34 + 0x54),
                      f, 1.0f, 9.999999974752427e-07f, 3.0f);
        func_001CFBE0(arg1, 1, slot, D_0081F8F0, 0);
        slot += 0x90;
    }

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
