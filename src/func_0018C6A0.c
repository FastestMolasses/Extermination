// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Eases cur's x (+0) and z (+8) toward target's. For each axis with delta
// d = target - cur: when |d| <= 1 the axis takes a quarter of d and sets its
// done bit (x 1, z 2); otherwise it moves by min(|d| / 6, maxstep) in d's
// direction. func_0011DF78 is fabsf. Returns the done bits.
extern float func_0011DF78(float x);

int func_0018C6A0(float *target, float *cur, float maxstep) {
    int done = 0;
    float d;
    float a;
    float step;

    d = target[0] - cur[0];
    a = func_0011DF78(d);
    if (!(a <= 1.0f)) {
        step = a / 6.0f;
        step = (maxstep > step) ? step : maxstep;
        if (d < 0.0f) {
            step = -step;
        }
        cur[0] = cur[0] + step;
    } else {
        done = 1;
        cur[0] = cur[0] + d / 4.0f;
    }
    d = target[2] - cur[2];
    a = func_0011DF78(d);
    if (!(a <= 1.0f)) {
        step = a / 6.0f;
        step = (maxstep > step) ? step : maxstep;
        if (d < 0.0f) {
            step = -step;
        }
        cur[2] = cur[2] + step;
    } else {
        done |= 2;
        cur[2] = cur[2] + d / 4.0f;
    }
    return done;
}
