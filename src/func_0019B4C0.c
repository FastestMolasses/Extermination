// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000319C 0x700031AC 0x700031D0 0x700031D4 0x700031D8 0x7000324E 0x70003254
//
// SEMANTICS: sibling of func_0019B2C0 but driven by two vectors (a1 = target, a2 = origin) and an actor a0.
// Loop builds the global scratch transform: D_70003190[i] = a1[i]-a2[i]; D_700031A0[i] = a1[i]; tmp[i] = a1[i]
// (i=0..2). Zeroes scratch 0x700031AC/0x7000319C/0x700031D4; runs the matrix helpers
// func_001028D0(mat,D_700031A0,D_70003190) / func_00102760 / func_00103230(.,.,0.01f) /
// func_001028D0(D_70003190,D_70003190,mat). Stores (a0[2] & 0x1F) as a short at 0x7000324E. Mode select:
// if (flags & 2) stores *(int*)(a0+0x14) at 0x70003254 and probes func_001A06A0() (mode 2 on zero); if
// (flags & 4) probes func_0019CF50() (mode 4). Calls func_001028B8(D_70003190,D_70003190,mat). If a mode
// was chosen: D_70003190[4..6]=tmp, D_70003190[12..14]=[8..10]-[4..6], and if flags<0 (bit 0x80000000)
// accumulates the delta D_70003190[12+i] into the actor's float triple at a0+0xB0. Otherwise clears
// 0x700031D0. Stores mode at 0x700031D8 and returns it.
//
// NOTE: the first func_001028D0 takes the SEPARATE global symbol D_700031A0 (not D_70003190+4) as its
// middle arg — using the +4 addend would mis-match the reloc symbol; declare D_700031A0 as its own extern.
//
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 96.56 NEARMISS whose header recorded two walls. Both were source: the two
// filled delay slots are MATCHING_GUIDE idiom-32 (the scratchpad globals above
// as relocated externs, opted in with // SPAD:), and the last loop's add is
// `*(float *)(a0 + 0xB0) += p[12];`, which gives the target's field + delta
// operand order (`= p[12] + f` and `= f + p[12]` each give 99.84).
extern int D_7000319C;
extern int D_700031AC;
extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern short D_7000324E;
extern int D_70003254;
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001A06A0(void);
extern int func_0019CF50(void);

extern float D_70003190[];
extern float D_700031A0[];

int func_0019B4C0(unsigned char *a0, float *a1, float *a2, int flags) {
    float mat[4];
    float tmp[4];
    int i;
    int mode;

    mode = 0;
    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a1[i] - a2[i];
        t = a1[i];
        D_70003190[4 + i] = t;
        tmp[i] = t;
    }
    D_700031AC = 0;
    D_7000319C = 0;
    D_700031D4 = 0;
    func_001028D0(mat, D_700031A0, D_70003190);
    func_00102760(mat, mat);
    func_00103230(mat, mat, 0.01f);
    func_001028D0(D_70003190, D_70003190, mat);

    D_7000324E = a0[2] & 0x1F;

    if (flags & 2) {
        D_70003254 = *(int *)(a0 + 0x14);
        if (func_001A06A0() == 0) {
            mode = 2;
        }
    }
    if (flags & 4) {
        if (func_0019CF50() == 0) {
            mode = 4;
        }
    }
    func_001028B8(D_70003190, D_70003190, mat);
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[4 + i] = tmp[i];
            D_70003190[12 + i] = D_70003190[8 + i] - D_70003190[4 + i];
        }
        if (flags & 0x80000000) {
            float *p = D_70003190;
            for (i = 0; i < 3; i++) {
                *(float *)(a0 + 0xB0) += p[12];
                p++;
                a0 += 4;
            }
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = mode;
    return mode;
}
