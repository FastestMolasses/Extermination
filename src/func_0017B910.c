// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 80.55% NEARMISS).
// Fixes read from the original instructions:
//  - -sdatathreshold 4: the original reaches D_00275B40 gp-relative; the clip-length
//    table D_0024875C and the scratchpad symbols stay absolute (unsized arrays).
//  - each scratchpad word at 0x70003A20/24/28/2C is its own relocated extern; this
//    lets the original hoist loads above stores to the other words (and gives the
//    branch-likely on the cur < lim test with cur - lim in its slot).
//  - the clip limit is read once into a local for the compare and the cur - lim
//    store; the 0x70003A20 subtraction re-reads the table.
//  - the distance squares are read back through the scratchpad words, not locals.
//  - func_0017B490's result is sign-extended from 64 bits by the caller (declared
//    long long here), and func_001C61D0's second argument is a short.
//
// Per-frame animation advance for an entity. Runs anim_eval_skeleton first.
// When the entity's 0x236 'busy' byte is 0 it advances the active clip: samples
// the clip frame via func_001C61D0/func_0017B490 into scratchpad 0x70003A20,
// compares the current time (+0x3C) against the clip-length table
// D_0024875C[state][sub] and stages the remaining time at 0x70003A24 and the
// end-pose X/Z (+0xC0/+0xC8) from the track struct *D_00275B40 (+0x48 if not
// yet at the limit, else +0x44, subtracting the limit instead of 1.0). For
// sub-clip 1 it halves the remaining time (min 1.0) into +0x268; otherwise sets
// +0x268 = 10 and kicks func_001749A0. It then computes the planar distance to
// target (dx = A28 - posX, dy = A2C - posZ, dist = func_0011E748(dx*dx + dy*dy)),
// builds an interpolation transform (func_001029C0/func_00102C58/func_001026A0),
// and stores per-axis step rates into +0x260/+0x264 (= transform / +0x268),
// marking state +0x1F0 = 5. When 0x236 is set it instead re-checks the current
// clip index: if changed it kicks func_001749A0 with 14.0; if unchanged and the
// 0x8000 flag is clear it clears the busy/sub state (+0x1F0 = 0, +0x25C = 0).
extern void anim_eval_skeleton(char *);
extern void func_001026A0(float *, float *, float *);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, char *);
extern float func_0011E748(float);
extern int func_001749A0(char *, short, int, float);
extern long long func_0017B490(char *, int, int, int);
extern int func_001C61D0(int, short);
extern float D_0024875C[][2];
extern char *D_00275B40;
extern float D_70003A20[];
extern float D_70003A24[];
extern float D_70003A28[];
extern float D_70003A2C[];
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];

void func_0017B910(char *arg0) {
    float cur;
    float lim;
    char *gp;
    short r;

    anim_eval_skeleton(arg0);
    if (*(unsigned char *)(arg0 + 0x236) == 0) {
        D_70003A20[0] = (float)func_001C61D0(*(int *)(arg0 + 0x40),
            func_0017B490(arg0, 1, *(unsigned char *)(arg0 + 0x235), *(unsigned char *)(arg0 + 0x25C)));
        cur = *(float *)(arg0 + 0x3C);
        lim = D_0024875C[*(unsigned char *)(arg0 + 0x235)][*(unsigned char *)(arg0 + 0x25C)];
        if (cur < lim) {
            gp = D_00275B40;
            D_70003A24[0] = cur - 1.0f;
            D_70003A20[0] = D_70003A20[0] - 1.0f;
            D_70003A28[0] = *(float *)(*(char **)(gp + 0x48) + 0xC0);
            D_70003A2C[0] = *(float *)(*(char **)(gp + 0x48) + 0xC8);
        } else {
            gp = D_00275B40;
            D_70003A24[0] = cur - lim;
            D_70003A20[0] = D_70003A20[0] - D_0024875C[*(unsigned char *)(arg0 + 0x235)][*(unsigned char *)(arg0 + 0x25C)];
            D_70003A28[0] = *(float *)(*(char **)(gp + 0x44) + 0xC0);
            D_70003A2C[0] = *(float *)(*(char **)(gp + 0x44) + 0xC8);
        }
        if (*(unsigned char *)(arg0 + 0x25C) == 1) {
            D_70003A24[0] = D_70003A24[0] / 2.0f;
            if (D_70003A24[0] < 1.0f) {
                D_70003A24[0] = 1.0f;
            }
            *(float *)(arg0 + 0x268) = D_70003A24[0];
        } else {
            *(float *)(arg0 + 0x268) = 10.0f;
            func_001749A0(arg0, func_0017B490(arg0, 6, *(unsigned char *)(arg0 + 0x235), *(unsigned char *)(arg0 + 0x25C)), 0,
                *(float *)(arg0 + 0x268));
        }
        D_70003A20[0] = D_70003A28[0] - *(float *)(arg0 + 0xB0);
        D_70003A24[0] = D_70003A2C[0] - *(float *)(arg0 + 0xB8);
        D_70003A28[0] = func_0011E748(D_70003A20[0] * D_70003A20[0] + D_70003A24[0] * D_70003A24[0]);
        func_001029C0(D_700036A0);
        func_00102C58(D_700036A0, D_700036A0, arg0 + 0xC0);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = D_70003A28[0];
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, D_700036A0, D_700038A0);
        *(float *)(arg0 + 0x260) = *(float *)0x700038B0 / *(float *)(arg0 + 0x268);
        *(float *)(arg0 + 0x264) = *(float *)0x700038B8 / *(float *)(arg0 + 0x268);
        *(char *)(arg0 + 0x1F0) = 5;
    } else {
        r = func_0017B490(arg0, 0, *(unsigned char *)(arg0 + 0x235), 0);
        if (r != *(short *)(arg0 + 0x20C)) {
            func_001749A0(arg0, r, 0, 14.0f);
        } else if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(char *)(arg0 + 0x1F0) = 0;
            *(unsigned char *)(arg0 + 0x25C) = 0;
        }
    }
}
