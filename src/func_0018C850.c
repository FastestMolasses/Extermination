// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern float func_0011DF78(float); /* fabsf */

int func_0018C850(float *o, float target, float step) {
    float d, a, v;
    d = target - o[1];
    a = func_0011DF78(d);
    if (!(a <= step)) {
        v = a * (0.5f * step);
        v = (step < v) ? v : step;
        if (d < 0.0f) v = -v;
        o[1] += v;
    } else {
        o[1] = target;
        return 4;
    }
    return 0;
}
