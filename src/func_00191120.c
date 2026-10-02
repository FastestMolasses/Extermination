// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Steps an angle toward a target: d = wrap(target - cur) (func_001B1470).
// When |d| < snap the result is wrap(cur). Otherwise, when |d| <= step the result
// is target, else wrap(cur + step) for d > 0 and wrap(cur - step) for d <= 0.
extern float func_001B1470(float a);
extern float func_0011DF78(float x);

float func_00191120(float target, float cur, float step, float snap) {
    float d = func_001B1470(target - cur);
    if (func_0011DF78(d) < snap) {
        return func_001B1470(cur);
    }
    if (d > 0.0f) {
        if (!(d <= step)) {
            cur += step;
        } else {
            return target;
        }
    } else {
        if (!(-d <= step)) {
            cur -= step;
        } else {
            return target;
        }
    }
    return func_001B1470(cur);
}
