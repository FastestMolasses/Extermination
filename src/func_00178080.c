// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.59% NEARMISS).
// Fixes read from the original instructions:
//  - func_0019BC40 takes one argument (its definition in src/ is
//    void func_0019BC40(float *pos)); the old two-argument int extern was wrong.
//  - The camera-delta stores go through the D_700038B0 / D_700031B0 externs, so
//    the second source load can be scheduled above the first store.
//  - The window select is an if/else that sets the 6.0 constant in both arms;
//    the angle reload after the call follows the flag-pointer setup.
//  - Local declaration order: index first, then the camera pointer, then the
//    6.0 / half-width floats before the stored angle (fixes the loop-pointer
//    and float register assignment).
//
// Scans the active-slot table for the first slot whose sightline check passes,
// then aims at it. Prologue: fills the D_700038B0 target vector from camera
// state at *0x700031D0 (delta of 0x700031B0/B8 minus cam+0x24/+0x2C), sets
// self+0xB4+15 into 0x700038B4 and marks D_700038BC = 1.0f, then calls
// func_0019BC40(target). Loop over 0..*0x700031E0: skip unless the u16 flag
// (D_70003170[i], &1) is set and the angle D_00282250[i] < 0.6283 (~36 deg). The
// window half-width is 6.0 if self+0xD == 2 else 3.0; if the stored angle 0x38B4
// lies inside (D_700030F0[i] - half, D_700030F0[i] + 6.0), lock on: write the
// world aim point into self+0x2E0/2E4/2E8 (cam pos + 1.5*cam basis, minus 20.5),
// compute the yaw via func_0011E620(-cam+0x2C, cam+0x24) into 0x70003A24, store
// self+0x218 = func_001B1470(4.712389 + yaw), and return 1. Return 0 if none.
extern float func_0011E620(float y, float x);
extern void func_0019BC40(float *pos);
extern float func_001B1470(float a);

extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;
extern float D_700038B0;
extern float D_700031B0[4];

int func_00178080(char *arg0) {
    int i;
    char *p;
    float six;
    float sel;
    float f3;
    float lo;
    unsigned short *flags;
    float *tsin;
    float *tpos;
    int cnt;

    p = *(char **)0x700031D0;
    (&D_700038B0)[0] = D_700031B0[0] - *(float *)(p + 0x24);
    (&D_700038B0)[2] = D_700031B0[2] - *(float *)(p + 0x2C);
    *(float *)0x700038B4 = 15.0f + *(float *)(arg0 + 0xB4);
    *(int *)0x700038BC = 0x3F800000;
    func_0019BC40(&D_700038B0);
    flags = &D_70003170;
    f3 = *(float *)0x700038B4;
    tsin = &D_00282250;
    tpos = &D_700030F0;
    cnt = *(int *)0x700031E0;
    i = 0;
    goto guard;
    for (;;) {
        if ((*flags & 1) && *tsin < 0.62831855f) {
            if (*(unsigned char *)(arg0 + 0xD) == 2) {
                six = 6.0f;
                sel = six;
            } else {
                six = 6.0f;
                sel = 3.0f;
            }
            lo = *tpos - sel;
            if (!(f3 <= lo) && f3 < six + *tpos) {
                p = *(char **)0x700031D0;
                *(float *)(arg0 + 0x2E0) = *(float *)0x700031B0 + 1.5f * *(float *)(p + 0x24);
                *(float *)(arg0 + 0x2E8) = *(float *)0x700031B8 + 1.5f * *(float *)(*(char **)0x700031D0 + 0x2C);
                *(float *)(arg0 + 0x2E4) = (&D_700030F0)[i] - 20.5f;
                p = *(char **)0x700031D0;
                *(float *)0x70003A24 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                *(float *)(arg0 + 0x218) = func_001B1470(4.712389f + *(float *)0x70003A24);
                return 1;
            }
        }
        flags += 1;
        tsin += 1;
        tpos += 1;
        i++;
    guard:
        if (i >= cnt) {
            return 0;
        }
    }
}
