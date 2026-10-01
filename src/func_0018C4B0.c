// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Eases cur's y (+4) toward target_y. With d = target_y - y: when |d| <= 1
// the axis takes a quarter of d and returns 4 (the y done bit); otherwise it
// moves by min(|d| / 8, maxstep) in d's direction and returns 0. Companion of
// func_0018C6A0 (x / z). func_0011DF78 is fabsf.
extern float func_0011DF78(float x);

int func_0018C4B0(float *cur, float target_y, float maxstep) {
    float d = target_y - cur[1];
    float a = func_0011DF78(d);
    float step;

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
