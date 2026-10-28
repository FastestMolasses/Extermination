// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.21% NEARMISS).
// Fixes read from the original instructions:
//  - Spelled like its byte-matched twin func_0018C4B0 (same ease, plus the
//    D_00810278 offset): the guard is `!(a <= 1.0f)` with the quarter-step
//    snap in the else arm returning 4 and a shared `return 0` after the if.
//  - The min clamp is `(maxstep > step) ? step : maxstep` (this sets the
//    branch-likely sense), not `(v <= a13) ? v : a13`.
//  - No copies of the parameters into locals; the target offset is added to
//    the parameter in place before the difference is taken.
//
// Eases cur's y (+4) toward target_y + D_00810278. With d = that target minus
// y: when |d| <= 1 the axis takes a quarter of d and returns 4 (the done bit);
// otherwise it moves by min(|d| / 8, maxstep) in d's direction and returns 0.
// func_0011DF78 is fabsf.
extern float D_00810278;
extern float func_0011DF78(float x);

int func_0018C5A0(float *cur, float target_y, float maxstep) {
    float d;
    float a;
    float step;

    target_y = target_y + D_00810278;
    d = target_y - cur[1];
    a = func_0011DF78(d);
    if (!(a <= 1.0f)) {
        step = a / 8.0f;
        step = (maxstep > step) ? step : maxstep;
        if (d < 0.0f) {
            step = -step;
        }
        cur[1] = cur[1] + step;
    } else {
        cur[1] = cur[1] + d / 4.0f;
        return 4;
    }
    return 0;
}
