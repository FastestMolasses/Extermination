// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-axis position smoothing/clamp. For two axes (struct offsets 0 and 8),
// moves arg1's value toward arg0's by at most a capped step:
//   d = arg0[axis] - arg1[axis]; a = fabs(d) (func_0011DF78)
//   if (a <= maxstep) snap exactly: arg1[axis] = arg0[axis], set result bit
//   else step by min(maxstep, a*0.5*maxstep) toward arg0 (sign from d)
// Returns a bitmask: bit0 = axis0 snapped exactly, bit1 = axis8 snapped exactly.
extern float func_0011DF78(float);

int func_0018C920(char *arg0, char *arg1, float maxstep) {
    int flags;
    float d;
    float a;
    float v;

    flags = 0;

    d = *(float *)(arg0 + 0) - *(float *)(arg1 + 0);
    a = func_0011DF78(d);
    if (!(a <= maxstep)) {
        v = a * (0.5f * maxstep);
        v = maxstep < v ? v : maxstep;
        if (d < 0.0f) {
            v = -v;
        }
        *(float *)(arg1 + 0) = *(float *)(arg1 + 0) + v;
    } else {
        *(float *)(arg1 + 0) = *(float *)(arg0 + 0);
        flags = 1;
    }

    d = *(float *)(arg0 + 8) - *(float *)(arg1 + 8);
    a = func_0011DF78(d);
    if (!(a <= maxstep)) {
        v = a * (0.5f * maxstep);
        v = maxstep < v ? v : maxstep;
        if (d < 0.0f) {
            v = -v;
        }
        *(float *)(arg1 + 8) = *(float *)(arg1 + 8) + v;
    } else {
        *(float *)(arg1 + 8) = *(float *)(arg0 + 8);
        flags |= 2;
    }

    return flags;
}
