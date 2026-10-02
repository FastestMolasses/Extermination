// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Side test: is the heading from b to a (atan2f(a.x - b.x, a.z - b.z),
// func_0011E620) at or to the left of heading, after wrapping the difference
// into (-pi, pi] (func_001B1470)? Returns 1 when the wrapped difference is
// >= 0, else 0.
extern float func_0011E620(float y, float x);
extern float func_001B1470(float angle);

int func_001B1380(float *a, float *b, float heading) {
    /* compiled as !(diff < 0.0f): a NaN difference returns 1 */
    if (func_001B1470(func_0011E620(a[0] - b[0], a[2] - b[2]) - heading) >= 0.0f) {
        return 1;
    }
    return 0;
}
