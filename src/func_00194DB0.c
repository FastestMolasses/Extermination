// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.15% NEARMISS).
// Fixes read from the original instructions:
//  - The goal-angle clamps were logically wrong: each is "if above the upper
//    bound use it, else if below the lower bound use it" (the old C never
//    clamped the upper side and wrongly snapped in-range values).
//  - The pitch clamp is "hi = pitch; if above -1497 use -1497, else if below
//    -1657 use -1657", and hi += 1497 is its own statement before the scale.
//  - The level-3 test is nested: level 3 steps only when the wrapped delta
//    exceeds 45 deg; other levels always step (two separate step calls).
//  - +0x14 is written as a float in both arms of an if/else (-3.0 / -13.0).

// Steers an actor's yaw (arg0+0xAC) toward a target angle derived from arg1's
// pitch field (+0xA8), clamped/wrapped into a piecewise angle range. If
// arg1's level id (+0x230) is 3 AND the wrapped delta to the goal is small
// (<=45 deg via func_001B1470/func_0011DF78 abs), the yaw is left unchanged
// this frame; otherwise it is stepped toward goal by func_001B12B0 at a
// ~0.2 deg rate. Recomputes arg0's screen
// offsets (+0x10/+0x18) from sin/cos of the new yaw scaled by var_f20 (94 or
// 100 depending on whether arg1's pitch is below -1610). Sets +0x14 to -13.0
// or -3.0 based on a flag bit at +0x5A, clamps +0x20 to <= -90 and +0x28 into
// [-1617,-1537], then calls func_0018C0C0/func_0018D7B0/func_0018C6A0/
// func_0018C4B0 to push the updated transform.
extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E2A8(float);
extern void func_0018C0C0(char *);
extern void func_0018C4B0(float *, float, float);
extern void func_0018C6A0(char *, float *, float);
extern void func_0018D7B0(char *, int);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern float D_008105D0;

void func_00194DB0(char *arg0, char *arg1) {
    float pitch;
    float hi;
    float goal;
    float scale;
    float delta;

    pitch = *(float *)(arg1 + 0xA8);
    scale = (pitch < -1610.0f) ? 94.0f : 100.0f;

    hi = pitch;
    if (!(hi <= -1497.0f)) {
        hi = -1497.0f;
    } else if (hi < -1657.0f) {
        hi = -1657.0f;
    }

    hi += 1497.0f;
    delta = (3.1415927f * hi) / -160.0f;
    if (pitch < -1577.0f) {
        goal = 1.5707964f + (3.1415927f - delta);
        if (!(goal <= 2.792527f)) {
            goal = 2.792527f;
        } else if (goal < 1.9198622f) {
            goal = 1.9198622f;
        }
    } else {
        goal = (3.1415927f - delta) - 1.5707964f;
        if (!(goal <= 1.2217306f)) {
            goal = 1.2217306f;
        } else if (goal < 0.34906587f) {
            goal = 0.34906587f;
        }
    }

    if (*(int *)(arg1 + 0x230) == 3) {
        if (!(func_0011DF78(func_001B1470(goal - *(float *)(arg0 + 0xAC))) <= 0.7853982f)) {
            *(float *)(arg0 + 0xAC) = func_001B12B0(goal, *(float *)(arg0 + 0xAC), 0.0034906587f);
        }
    } else {
        *(float *)(arg0 + 0xAC) = func_001B12B0(goal, *(float *)(arg0 + 0xAC), 0.0034906587f);
    }

    *(float *)(arg0 + 0x10) = -142.0f + (scale * func_0011E2A8(*(float *)(arg0 + 0xAC)));
    *(float *)(arg0 + 0x18) = -1577.0f + (scale * func_0011DE90(*(float *)(arg0 + 0xAC)));

    if (*(short *)(arg0 + 0x5A) & 1) {
        *(float *)(arg0 + 0x14) = -3.0f;
    } else {
        *(float *)(arg0 + 0x14) = -13.0f;
    }

    if (!(*(float *)(arg0 + 0x20) <= -90.0f)) {
        *(float *)(arg0 + 0x20) = -90.0f;
    }

    if (*(float *)(arg0 + 0x28) < -1617.0f) {
        *(float *)(arg0 + 0x28) = -1617.0f;
    } else if (!(*(float *)(arg0 + 0x28) <= -1537.0f)) {
        *(float *)(arg0 + 0x28) = -1537.0f;
    }

    func_0018C0C0(arg0);
    func_0018D7B0(arg0, 5);
    func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.0f);
    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 1.0f);
}
