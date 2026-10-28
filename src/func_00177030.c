// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003A20 0x70003A24 0x70003A28 0x70003A2C

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.32% NEARMISS).
// Fixes read from the original instructions:
//  - Wrong body: case 4 stores the dot product to scratchpad 0x70003A24 and
//    tests that stored value against 0.5.
//  - Wrong body: case 2 stores both deltas (0x70003A20 = x, 0x70003A28 = z)
//    to the scratchpad before the atan2/length calls and reads them back
//    from there; there were no separate locals.
//  - The failing guards of cases 0, 1 and 4 `break` to one shared
//    `return 0` after the switch (no default arm); case 0 then reuses the
//    dispatch's constant 1 as its return value.
//  - The scratchpad results 0x70003A20..0x70003A2C are relocated externs
//    (idiom-32, opted in with // SPAD:); with literal addresses mwcc
//    speculated the scratchpad upper half into the dispatch delay slots.
//  - Case 2 reads the AABB centre through D_700038B0[0]/[2], so the z load
//    is scheduled above the 0x70003A20 store as in the original.
//  - The heading sum `1.5707964f + D_70003A24` is its own local before the
//    facing at +0x218 is subtracted (FP register order).
//
// actor_anim: aim/turn state machine dispatched by arg1 (0-4). Computes the
// camera-relative yaw wrap via atan2f(func_0011E620)/func_001B1470, then per
// mode either latches +0xC4 (facing), blends +0xB0/+0xB8 toward an object AABB
// center (func_00199DB0), or (case 2) steers a turret blend by side/step, or
// (case 4) gates the latch on a func_00102738 dot product >= 0.5.
//
extern float func_0011E620(float y, float x);
extern float func_001B1470(float rad);
extern float func_0011DF78(float rad);
extern float func_0011DE90(float rad);
extern float func_0011E748(float x);
extern void func_00199DB0(void *out);
extern void func_001026A0(void *out, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *dst, void *src);

extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;
extern float D_70003A2C;
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];
extern float D_700038D0[];

int func_00177030(char *arg0, int arg1) {
    void *p = *(void **)0x700031D0;
    D_70003A20 = func_0011E620(-*(float *)((char *)p + 0x3C), *(float *)((char *)p + 0x34));
    D_70003A20 = func_001B1470(1.5707964f + D_70003A20);
    D_70003A24 = func_001B1470(D_70003A20 - *(float *)(arg0 + 0xC4));

    switch (arg1) {
    case 0:
        if (!(D_70003A24 < -0.7853982f) && D_70003A24 <= 0.7853982f) {
            *(float *)(arg0 + 0xC4) = D_70003A20;
            return 1;
        }
        break;
    case 1:
        if (!(D_70003A24 < -0.7853982f) && D_70003A24 <= 0.7853982f) {
            *(float *)(arg0 + 0xC4) = D_70003A20;
            func_00199DB0(D_700038B0);
            *(float *)(arg0 + 0xB0) = *(float *)0x700038B0;
            *(float *)(arg0 + 0xB8) = *(float *)0x700038B8;
            return 1;
        }
        break;
    case 2:
    {
        float side;
        if (func_0011DF78(D_70003A24) <= 1.5707964f) {
            *(float *)(arg0 + 0x218) = D_70003A20;
            side = 1.0f;
        } else {
            *(float *)(arg0 + 0x218) = func_001B1470(3.1415927f + D_70003A20);
            side = -1.0f;
        }
        func_00199DB0(D_700038B0);
        D_70003A20 = D_700038B0[0] - *(float *)(arg0 + 0xB0);
        D_70003A28 = D_700038B0[2] - *(float *)(arg0 + 0xB8);
        D_70003A24 = func_0011E620(-D_70003A28, D_70003A20);
        D_70003A2C = func_0011E748((D_70003A20 * D_70003A20) + (D_70003A28 * D_70003A28));
        {
            float a = 1.5707964f + D_70003A24;
            float step = D_70003A2C * func_0011DE90(func_001B1470(a - *(float *)(arg0 + 0x218)));
            *(float *)(arg0 + 0xB0) = *(float *)0x700038B0 - (side * ((*(float **)0x700031D0)[0x34/4] * step));
            *(float *)(arg0 + 0xB8) = *(float *)0x700038B8 - (side * ((*(float **)0x700031D0)[0x3C/4] * step));
        }
        return 1;
    }
    case 3:
        func_00199DB0(D_700038B0);
        *(float *)(arg0 + 0xB0) = *(float *)0x700038B0;
        *(float *)(arg0 + 0xB8) = *(float *)0x700038B8;
        return 1;
    case 4:
    {
        void *p2 = *(void **)0x700031D0;
        *(float *)0x700038B0 = *(float *)((char *)p2 + 0x34);
        *(int *)0x700038B4 = 0;
        *(float *)0x700038B8 = *(float *)((char *)p2 + 0x3C);
        *(int *)0x700038BC = 0x3F800000;
        func_00102760(D_700038B0, D_700038B0);
        *(int *)0x700038C0 = 0;
        *(int *)0x700038C4 = 0;
        *(int *)0x700038C8 = 0x3F800000;
        *(int *)0x700038CC = 0;
        func_001026A0(D_700038D0, arg0 + 0xD0, D_700038C0);
        D_70003A24 = func_00102738(D_700038B0, D_700038D0);
        if (!(D_70003A24 < 0.5f)) {
            *(float *)(arg0 + 0xC4) = D_70003A20;
            func_00199DB0(D_700038A0);
            *(float *)(arg0 + 0xB0) = *(float *)0x700038A0;
            *(float *)(arg0 + 0xB8) = *(float *)0x700038A8;
            return 1;
        }
        break;
    }
    }
    return 0;
}
