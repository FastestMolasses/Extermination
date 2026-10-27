// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A4 0x700038B4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.03% NEARMISS).
// Fixes read from the original instructions:
//  - State 0 does not leave the switch on its two failed tests: every path of
//    the one-time init falls through into the state-1 body (both early exits
//    branched to the state-1 code in the original), so the blocks nest as
//    if (...) { ...; if (ang < 0) { ... } } followed by the case-1 label.
//  - The scratch-vector y words 0x700038A4 and 0x700038B4 are relocated
//    externs (idiom-32, opted in with // SPAD:), D_700038A4 / D_700038B4,
//    read directly (no cached/ramp temporaries): "B4 += 10; if (A4 < B4)"
//    reproduces the original's early A4 load, in-place store and register copy.
//  - func_00102948 takes untyped vector pointers (its definition uses u128 *),
//    so arg0 + 0xB0 is passed uncast; that sets up the first argument before
//    the second as in the original, without the old write-only `p =` local.
//
// State-byte (arg0+6) driven proximity/idle timer, sibling of func_0014C9D0
// (same VU0-scratchpad recoil/blend idiom, same 0x700038A0/0x700038B0/
// 0x700036A0/0x70003A20 scratch layout). arg0 = entity, arg1 = linked block.
//
// state 0 (one-time init, falls through to state 1): advance state, set
// arg1+0x2C = 1.88f. Copy the entity position (arg0+0xB0) to 0x700038A0 and
// the player position (D_00810360) to 0x700038B0, raise the second vector's y
// by 10.0f, and if the entity's y is below it, derive an angle via
// func_0011DF78 (y delta) / func_001B15D0 (distance) / func_0011E620; if the
// negated angle is negative, rotate a unit vector through func_001029C0 /
// func_00102B08 / func_001026A0 and stash the resulting y in arg1+0x24.
//
// state 1 body: tick down arg1+0x14 (a countdown), else reroll it to a
// random 0-15 value via func_00122BB8() and refresh the aim pose
// (func_001B1240 -> arg1+0x34, func_001B12B0 -> arg0+0xC4). If arg1+0x28
// (the perturbation) is still non-negative: compare the y delta between
// arg0+0xB0 and D_00810360 against 6.5f; above it, a distance <= 100.0f
// rerolls directly, otherwise (or within 6.5f) a func_001B1560 view check at
// 80 degrees (1.3962635f) decides; on reroll, a fresh perturbation goes into
// arg1+0x28 from func_00122BB8().

extern void func_001026A0(float *a, float *b, float *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(float *a);
extern void func_00102B08(float *a, float *b, float v);
extern float func_0011DF78(float a);
extern float func_0011E620(float a, float b);
extern int func_00122BB8(void);
extern float func_001B1240(char *p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B1560(char *p, float *v, float r);
extern float func_001B15D0(float *a, float *b);

extern float D_00810360;
extern float D_00810368;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038A4;
extern float D_700038B4;

void func_0014C680(char *arg0, char *arg1) {
    unsigned char st;
    int cnt;
    float ang;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(float *)(arg1 + 0x2C) = 1.88f;
        func_00102948(&D_700038A0, arg0 + 0xB0);
        func_00102948(&D_700038B0, &D_00810360);
        D_700038B4 += 10.0f;
        if (D_700038A4 < D_700038B4) {
            *(volatile float *)0x70003A20 = func_0011DF78(D_700038A4 - D_700038B4);
            D_700038B4 = 0.0f;
            D_700038A4 = 0.0f;
            *(volatile float *)0x70003A24 = func_001B15D0(&D_700038A0, &D_700038B0);
            ang = -1.0f * func_0011E620(*(volatile float *)0x70003A20, *(volatile float *)0x70003A24);
            *(volatile float *)0x70003A20 = ang;
            if (ang < 0.0f) {
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, *(volatile float *)0x70003A20);
                *(volatile int *)0x700038A0 = 0;
                D_700038A4 = 0.0f;
                *(volatile float *)0x700038A8 = 1.88f;
                *(volatile float *)0x700038AC = 1.0f;
                func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
                *(float *)(arg1 + 0x24) = D_700038A4;
            }
        }
        /* fallthrough */
    case 1:
        cnt = *(int *)(arg1 + 0x14);
        if (cnt != 0) {
            *(int *)(arg1 + 0x14) = cnt - 1;
        } else {
            *(int *)(arg1 + 0x14) = (func_00122BB8() >> 15) & 0xF;
            *(float *)(arg1 + 0x34) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x34), *(float *)(arg0 + 0xC4), 0.07853981852531433f);
        }
        if (*(float *)(arg1 + 0x28) < 0.0f) {
            break;
        }
        func_00102948(&D_700038A0, arg0 + 0xB0);
        func_00102948(&D_700038B0, &D_00810360);
        *(volatile float *)0x70003A20 = func_0011DF78(D_700038A4 - D_700038B4);
        D_700038B4 = 0.0f;
        D_700038A4 = 0.0f;
        if (*(volatile float *)0x70003A20 <= 6.5f) {
            goto check_angle;
        }
        if (func_001B15D0(&D_700038A0, &D_700038B0) <= 100.0f) {
            goto reroll;
        }
    check_angle:
        if (func_001B1560(arg0, &D_700038B0, 1.3962634801864624f) != 0) {
            break;
        }
    reroll:
        *(float *)(arg1 + 0x28) = (-0.01f * ((float)((func_00122BB8() >> 11) & 0xFF) / 255.0f)) - 0.035f;
        break;
    }
}
