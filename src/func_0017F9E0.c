// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A8 0x700038B0 0x700038B8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.38% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad vec slots 0x700038A0/A8/B0/B8 are relocated float externs
//    (idiom-32, opted in with // SPAD:): the address arguments and the averaging
//    reads go through D_700038A0..D_700038B8. As relocated externs the merge
//    block's upper-half address is no longer speculated into the skip-else
//    branch slot, which the original leaves empty.
//  - The scratch heading floats 0x70003A20/0x70003A24 are written as the
//    relocated externs D_70003A20/D_70003A24 (already in the global list).
//
// Aim/orient setup for actor arg0 against the camera/target record at *0x700031D0.
// Computes a base heading: func_0011E620(-record[0x3C], record[0x34]) (atan2-like)
// stored at the scratch float 0x70003A20, then func_001B1470(PI/2 + that) (angle
// normalize), latched back to 0x70003A20 and into arg0+0x218. The turn delta
// 0x70003A20 - arg0+0xC4 (normalized) is stored at 0x70003A24; its sign chooses a
// branch: if >= 0, arg0+0xC4 = normalize(arg0+0x218 - PI/2), arg0+0x2F1 = 0, and
// two func_0019F680(&D_700038A0/B0, *0x700031D0, 2/3) calls; if < 0, arg0+0xC4 =
// normalize(PI/2 + arg0+0x218), arg0+0x2F1 = 1, with func_0019F680 args 0/1.
// Both arms then set arg0+0xB0 = (D_700038A0 + D_700038B0)/2, arg0+0xB8 =
// (D_700038A8 + D_700038B8)/2 (the func_0019F680 calls fill those vec slots),
// set arg0[5]=0x1B, arg0[6]=0, arg0+0x1F0=0x2F, arg0+0x1F1=0, and call
// func_00174A50(self, 0.0f).
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern void func_0019F680(void *p, int a, int b);
extern void func_00174A50(char *p, float f);
extern float D_700038A0;
extern float D_700038A8;
extern float D_700038B0;
extern float D_700038B8;
extern float D_70003A20;
extern float D_70003A24;

void func_0017F9E0(char *arg0) {
    char *p;
    float t;

    p = *(char **)0x700031D0;
    D_70003A20 = func_0011E620(-*(float *)(p + 0x3C), *(float *)(p + 0x34));
    t = func_001B1470(1.5707964f + D_70003A20);
    D_70003A20 = t;
    *(float *)(arg0 + 0x218) = t;
    t = func_001B1470(D_70003A20 - *(float *)(arg0 + 0xC4));
    D_70003A24 = t;
    if (!(t < 0.0f)) {
        *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0x218) - 1.5707964f);
        *(char *)(arg0 + 0x2F1) = 0;
        func_0019F680(&D_700038A0, *(int *)0x700031D0, 2);
        func_0019F680(&D_700038B0, *(int *)0x700031D0, 3);
    } else {
        *(float *)(arg0 + 0xC4) = func_001B1470(1.5707964f + *(float *)(arg0 + 0x218));
        *(char *)(arg0 + 0x2F1) = 1;
        func_0019F680(&D_700038A0, *(int *)0x700031D0, 0);
        func_0019F680(&D_700038B0, *(int *)0x700031D0, 1);
    }
    *(float *)(arg0 + 0xB0) = (D_700038A0 + D_700038B0) / 2.0f;
    *(float *)(arg0 + 0xB8) = (D_700038A8 + D_700038B8) / 2.0f;
    *(char *)(arg0 + 5) = 0x1B;
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 0x1F0) = 0x2F;
    *(char *)(arg0 + 0x1F1) = 0;
    func_00174A50(arg0, 0.0f);
}
