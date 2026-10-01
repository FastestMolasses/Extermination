// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Steps an angle toward another by at most step. d = wrap(cur - target)
// (func_001B1470 wraps into (-pi, pi]); d == 0 returns wrap(target). When
// |d| <= step the angle is already close enough and cur is returned;
// otherwise the result is wrap(target + step) for d > 0 and
// wrap(target - step) for d < 0, i.e. step away from target toward cur.
extern float func_001B1470(float angle);

float func_001B12B0(float cur, float target, float step) {
    float d = func_001B1470(cur - target);

    if (d == 0.0f) {
        return func_001B1470(target);
    }
    if (d > 0.0f) {
        if (!(d <= step)) {
            target = target + step;
            goto wrap;
        }
        return cur;
    }
    if (!(-d <= step)) {
        target = target - step;
        goto wrap;
    }
    return cur;
wrap:
    return func_001B1470(target);
}
