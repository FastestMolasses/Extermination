// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0 0x700031D8
//
// SEMANTICS: builds the global scratch transform at 0x70003190 / D_700031A0 from the two input vectors
// (a0, a1). Seeds 0x70003190.x = a0.x, .y = a1.y, .z = a0.z; copies a1.xyz into 0x700031A0..0x700031A8
// (D_70003190[4..6]) and a stack temp; zeroes scratch fields 0x700031AC/0x7000319C/0x700031D4; chains the matrix helpers
// func_001028D0/func_00102760/func_00103230(.,.,0.01f)/func_001028B8 to produce a clamped/blended
// transform; sets the short flag at 0x7000324E = -1. Then a mode select: if (flags & 2) clears 0x70003254
// and probes func_0019FE50() (mode 2 on success); if (flags & 4) probes func_0019CB60() (mode 4). Recomputes
// the transform, and if a mode was chosen copies the saved a1 vector back into D_70003190[4..6] and stores
// the delta D_70003190[8..10]-[4..6] into [12..14]; if flags<0 (bit 0x80000000) nudges a0.x/a0.z by the
// global offsets 0x700031C0/0x700031C8. Otherwise clears 0x700031D0. Stores the mode at 0x700031D8 and
// returns it.
//
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 96.72 NEARMISS whose header called the two filled delay slots a genuine
// backend wall. They were MATCHING_GUIDE idiom-32: with 0x700031D0 and
// 0x700031D8 as relocated externs (opted in with // SPAD:) mwcc leaves both
// slots as nop. The CORRECTED note of lane NMFIX4 (2026-09-28) still holds: the
// first loop stores a1.xyz to 0x700031A0..0x700031A8 (D_70003190[4 + i]).
extern int D_700031D0;
extern int D_700031D8;
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019FE50(void);
extern int func_0019CB60(void);

extern float D_70003190[];
extern float D_700031A0[];

int func_0019B2C0(float *a0, float *a1, int flags) {
    float mat[4];
    float tmp[4];
    int i;
    int mode;

    *(float *)0x70003190 = a0[0];
    *(float *)0x70003194 = a1[1];
    *(float *)0x70003198 = a0[2];

    mode = 0;
    for (i = 0; i < 3; i++) {
        float t = a1[i];
        D_70003190[4 + i] = t;
        tmp[i] = t;
    }
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    func_001028D0(mat, D_700031A0, D_70003190);
    func_00102760(mat, mat);
    func_00103230(mat, mat, 0.01f);
    func_001028B8(D_700031A0, D_700031A0, mat);
    *(short *)0x7000324E = -1;

    if (flags & 2) {
        *(int *)0x70003254 = 0;
        if (func_0019FE50() == 0) {
            mode = 2;
        }
    }
    if (flags & 4) {
        if (func_0019CB60() == 0) {
            mode = 4;
        }
    }
    func_001028D0(D_700031A0, D_700031A0, mat);
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[4 + i] = tmp[i];
            D_70003190[12 + i] = D_70003190[8 + i] - D_70003190[4 + i];
        }
        if (flags & 0x80000000) {
            a0[0] = a0[0] + *(float *)0x700031C0;
            a0[2] = a0[2] + *(float *)0x700031C8;
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = mode;
    return mode;
}
