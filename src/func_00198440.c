// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.67% NEARMISS).
// Fixes read from the original instructions:
//  - The four scratchpad header words (0x70003600..0x7000360C) are written as four
//    separate scratchpad symbols (one extern per address, idiom-32), not through a
//    local pointer to a literal address. That fixes the float register colouring of
//    the three loads and keeps the D_70003610 argument address after the stores.
//  - The three header values are stored straight from their globals (no locals).

// Builds a transform/offset workspace in the EE scratchpad (0x70003600 region):
// writes (-D_002754E8, D_002754F0, D_002754EC, 0), calls func_001026A0 to populate
// D_70003610, then composes object position (arg0[4..6], [8..10]) from the entity
// at *(arg1+0x20)+0xA0.. and the scratchpad results. arg2 selects 3 path variants.
// Note 0.4f (0x3ECCCCCD) is the scale constant; the
// array extern types keep the scratchpad/BSS symbols out of small data (absolute
// addressing) while the D_002754E8/EC/F0 scalars stay gp-relative.
extern int func_001026A0(float *, int, float *);
extern int func_0018C850(void *, float, float);
extern int func_0018C920(float *, void *, float);
extern int func_00102948(void *, float *);
extern int func_00103230(float *, float *, float);
extern int func_001028B8(float *, float *, float *);
extern int func_00198240(int, int);
extern float D_002754E8;
extern float D_002754EC;
extern float D_002754F0;
extern char D_008105D0[16];
extern char D_008105E0[16];
extern unsigned char D_008106C6[16];
extern float D_700038A0[4];
extern float D_70003600[4];
extern float D_70003610[4];
extern float D_70003604[];
extern float D_70003608[];
extern int D_7000360C[];

void func_00198440(float *arg0, int arg1, int arg2) {
    int s0 = *(int *)(arg1 + 0x20);
    int s1;

    D_70003600[0] = -D_002754E8;
    D_70003604[0] = D_002754F0;
    D_70003608[0] = D_002754EC;
    D_7000360C[0] = 0;
    s1 = s0 + 0x1F0;
    func_001026A0(D_70003610, *(int *)(s0 + 0x110) + 0x90, D_70003600);
    arg0[4] = *(float *)(s0 + 0xA0) + *(float *)0x70003610;
    arg0[5] = *(float *)(s0 + 0xA4) + *(float *)0x70003614;
    arg0[6] = *(float *)(s0 + 0xA8) + *(float *)0x70003618;
    if (arg2 == 0) {
        func_0018C850(D_008105D0, arg0[5], 0.4f);
        func_0018C920(arg0 + 4, D_008105D0, 0.4f);
    } else {
        func_00102948(D_008105D0, arg0 + 4);
    }
    if (*(int *)(s1 + 0x20) != 0 && D_008106C6[0] != 0) {
        func_00102948(arg0 + 8, (float *)(s1 + 0x10));
    } else {
        func_00103230(D_70003610, (float *)(s0 + 0xC0), 200.0f);
        arg0[8] = *(float *)(s0 + 0xA0) + *(float *)0x70003610;
        arg0[9] = *(float *)(s0 + 0xA4) + *(float *)0x70003614;
        arg0[10] = *(float *)(s0 + 0xA8) + *(float *)0x70003618;
    }
    func_00102948(D_008105E0, arg0 + 8);
    {
        int r = func_00198240(arg1, s0);
        if (arg2 == 2 && r != 0) {
            func_001028B8(arg0 + 4, arg0 + 4, D_700038A0);
            func_00102948(D_008105D0, arg0 + 4);
        }
    }
}
