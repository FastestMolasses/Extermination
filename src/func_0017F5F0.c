// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.94% NEARMISS).
// Fixes read from the original instructions:
// - State 1's timer-expiry test is a float post-decrement compared against a
//   zero local (`if ((*timer)-- == zero)`), not a copy of the timer stored back
//   as `t - 1.0f`. The old spelling made mwcc reload/recolor the 1.0f constant
//   (the "FP odd/even coloring wall"); the post-decrement keeps the 1.0f from
//   the `1.0f - table` expression in the target's register.
// - State 0 keeps the explicit `t = timer; timer = t - 1.0f; if (t == zero)`
//   spelling, which already matched.

// CORRECTED (decomp lane round 5, 2026-10-20) against the original instructions:
// func_00174FD0 takes the entity; the original calls it with its own a0 (arg0)
// untouched, so the call passes arg0 (the old C called it with no argument, which
// was the "prologue saved-register move-order swap"). The timer-expiry updates of
// +0x38 are compound assignments (`+= d`, `-= d`), which give the target's
// operand order.

//
// Entity turn/aim-blend state machine. arg0 is the entity, arg1 is a
// 'skip-anim-clip' flag. First block: state = arg0[0x24C] selects a turn
// blend (0=turn-toward via D_00248790 table +cos speed, 1=turn-away via
// (1-table)*cos, default=snap using func_0011E2A8 heading trig), each firing
// anim_clip via func_001749A0(e, clipId, 0, blendSeconds) unless arg1 skips
// it, then ramps arg0[0x38] over a 30-frame timer (arg0[0x2F8]/arg0[0x2E4])
// and clamps it to 1.5f. Second block: arg0[0x24C]==2/3 rotates the aim
// angle arg0[0xC4] toward a computed target using func_001B1470 (angle
// normalize) + func_001B12B0 (angle lerp, rate 0.06981317f), tracking a
// left/right turn flag at arg0[0x25D]; default case resets the turn flag and
// lerps back to the base heading arg0[0x218].
extern float func_0011E2A8(float a);
extern int func_001749A0(int e, short clip, int flags, float blend);
extern float func_001B1470(float a);
extern float func_001B12B0(float a, float b, float rate);
extern float D_00248790[];
extern float D_002487A0[];
extern void func_00174FD0(int);

void func_0017F5F0(int arg0, int arg1) {
    int state;
    int flag;
    flag = arg1;
    func_00174FD0(arg0);
    state = *(int *)(arg0 + 0x24C);

    if (state == 0) {
        if (flag == 0) {
            func_001749A0(arg0, 0x61, 0, 8.0f);
        }
        *(float *)0x70003A20 = 1.0f + D_00248790[*(unsigned char *)(arg0 + 0x23F)];
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + *(float *)0x70003A20 * (0.01f * func_0011E2A8(*(float *)(arg0 + 0x9C)));
        if (*(unsigned char *)(arg0 + 0x1F1) != 1) {
            *(float *)(arg0 + 0x2F8) = 30.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x38) * *(float *)0x70003A20) / *(float *)(arg0 + 0x2F8);
            *(unsigned char *)(arg0 + 0x1F1) = 1;
        } else {
            float zero = 0.0f;
            float t = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0x2F8) = t - 1.0f;
            if (t == zero) {
                float d = *(float *)(arg0 + 0x2E4);
                *(float *)(arg0 + 0x38) += d;
            }
        }
    } else if (state == 1) {
        if (flag == 0) {
            func_001749A0(arg0, 0x62, 0, 8.0f);
        }
        *(float *)0x70003A20 = 1.0f - D_00248790[*(unsigned char *)(arg0 + 0x23F)];
        if (*(unsigned char *)(arg0 + 0x1F1) != 2) {
            *(float *)(arg0 + 0x2F8) = 30.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x38) * *(float *)0x70003A20) / *(float *)(arg0 + 0x2F8);
            *(unsigned char *)(arg0 + 0x1F1) = 2;
        } else {
            float zero = 0.0f;
            if ((*(float *)(arg0 + 0x2F8))-- == zero) {
                *(float *)(arg0 + 0x38) -= *(float *)(arg0 + 0x2E4);
            }
        }
    } else {
        *(unsigned char *)(arg0 + 0x1F1) = 0;
        if (flag == 0) {
            func_001749A0(arg0, 0x5F, 0, 8.0f);
        }
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + 0.01f * func_0011E2A8(*(float *)(arg0 + 0x9C));
    }

    if (!(*(float *)(arg0 + 0x38) <= 1.5f)) {
        *(float *)(arg0 + 0x38) = 1.5f;
    }

    state = *(int *)(arg0 + 0x24C);
    if (state == 2) {
        *(unsigned char *)(arg0 + 0x25D) = 1;
        if (flag == 0) {
            func_001749A0(arg0, 0x63, 0, 8.0f);
        }
        {
            int idx = *(unsigned char *)(arg0 + 0x23F);
            float ang = *(float *)(arg0 + 0x218) - (3.1415927f * D_002487A0[idx]) / 180.0f;
            *(float *)0x70003A24 = ang;
            *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1470(ang), *(float *)(arg0 + 0xC4), 0.06981317f);
        }
    } else if (state == 3) {
        *(unsigned char *)(arg0 + 0x25D) = 2;
        if (flag == 0) {
            func_001749A0(arg0, 0x64, 0, 8.0f);
        }
        {
            int idx = *(unsigned char *)(arg0 + 0x23F);
            float ang = *(float *)(arg0 + 0x218) + (3.1415927f * D_002487A0[idx]) / 180.0f;
            *(float *)0x70003A24 = ang;
            *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1470(ang), *(float *)(arg0 + 0xC4), 0.06981317f);
        }
    } else {
        unsigned char turn = *(unsigned char *)(arg0 + 0x25D);
        if (((turn == 1) || (turn == 2)) && (flag == 0)) {
            func_001749A0(arg0, 0x5F, 0, 12.0f);
        }
        *(unsigned char *)(arg0 + 0x25D) = 0;
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.06981317f);
    }
}
