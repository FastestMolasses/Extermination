// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003058 0x70003068
//
// sp = (*0x70003050 - 1.5 * *0x70003060, fparg0, *0x70003058 - 1.5 *
// *0x70003068); func_001029C0(D_700036A0), func_00102BB0(D_700036A0, ...,
// the float at 0x700031E4), func_00102918(D_700036A0, ..., sp). The scratchpad
// points (0, 1, 0, 1) at 0x700038A0 and (0, -1, 0, 1) at 0x700038B0 go through
// func_001026A0 into 0x700038C0 / 0x700038D0; returns 1 when
// func_0019A570(C0, D0, 6, 0) is non-zero, else 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// an 89.75 NEARMISS whose header called the residual an FP register-coloring
// permutation. It was MATCHING_GUIDE idiom-32: 0x70003058 and 0x70003068 are
// relocated externs (opted in with // SPAD:); 0x70003050 / 0x70003060 stay
// literal (the sweep's best set).

extern float D_70003058;
extern float D_70003068;
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, float *);
extern void func_001026A0(void *, void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

int func_00177F40(float fparg0) {
    float sp[3];

    sp[0] = *(float *)0x70003050 - 1.5f * *(float *)0x70003060;
    sp[1] = fparg0;
    sp[2] = D_70003058 - 1.5f * D_70003068;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x700031E4);
    func_00102918(D_700036A0, D_700036A0, sp);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038B0 = 0;
    *(int *)0x700038A4 = 0x3F800000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B4 = 0xBF800000;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    return func_0019A570(D_700038C0, D_700038D0, 6, 0) != 0 ? 1 : 0;
}
