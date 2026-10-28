// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.85% NEARMISS).
// Fixes read from the original instructions:
// - each clamp branch stores all 8 floats and returns 0 itself (no shared tail store);
// - D_00275BFC is gp-relative small data (-sdatathreshold 4); D_008106F3 is declared
//   as an unsized array so it stays an absolute (non-small) byte;
// - the last-keyframe pointer is indexed as an array (&keys[(n - 1) * 8]);
// - absolute values are ternaries, the second lerp half is compound `+=`, and the
//   last field is stored then re-added through the destination (arg0[7] = arg0[7] + a).

// Interpolates a "keyframe" record (8 floats, stride 0x20 bytes: 7 data
// floats + 1 time float at +0x1C) from a table pointed to by arg1+0x10.
// fparg0 is a fractional frame time. Clamps to first/last keyframe, else
// interpolates linearly between the bracketing pair, tracking whether the
// interpolated "loop" flag (field +0x1C, abs'd) went negative (wrap).
// The interpolated field 6 starts from 0, so it is a copy of the first key's field 6.
extern int float_to_int(float);
extern int D_00275BFC;
extern char D_008106F3[];

int func_001C7C00(float *arg0, float *arg1, float fparg0) {
    float *keys;
    float *next;
    float *cur;
    float a;
    float b;
    float t;

    keys = (float *) ((char *) arg1 + 0x10);
    if (fparg0 < 0.0f) {
        arg0[0] = keys[0];
        arg0[1] = keys[1];
        arg0[2] = keys[2];
        arg0[3] = keys[3];
        arg0[4] = keys[4];
        arg0[5] = keys[5];
        arg0[6] = keys[6];
        arg0[7] = keys[7];
        return 0;
    }
    if (!(fparg0 < *arg1)) {
        keys = &keys[(float_to_int(*arg1) - 1) * 8];
        arg0[0] = keys[0];
        arg0[1] = keys[1];
        arg0[2] = keys[2];
        arg0[3] = keys[3];
        arg0[4] = keys[4];
        arg0[5] = keys[5];
        arg0[6] = keys[6];
        arg0[7] = keys[7];
        return 0;
    }
    t = fparg0 - (float) float_to_int(fparg0);
    cur = (float *) ((char *) keys + (float_to_int(fparg0) << 5));
    next = (float *) ((char *) cur + 0x20);
    if (cur[7] < 0.0f) {
        t = 0.0f;
        D_008106F3[0] = 1;
        D_00275BFC = 0x20;
    } else {
        D_008106F3[0] = 0;
    }
    a = (cur[7] < 0.0f) ? -cur[7] : cur[7];
    b = (next[7] < 0.0f) ? -next[7] : next[7];
    b = t * (b - a);
    arg0[0] = t * (next[0] - cur[0]);
    arg0[1] = t * (next[1] - cur[1]);
    arg0[2] = t * (next[2] - cur[2]);
    arg0[3] = t * (next[3] - cur[3]);
    arg0[4] = t * (next[4] - cur[4]);
    arg0[5] = t * (next[5] - cur[5]);
    arg0[6] = 0.0f;
    arg0[0] += cur[0];
    arg0[1] += cur[1];
    arg0[2] += cur[2];
    arg0[3] += cur[3];
    arg0[4] += cur[4];
    arg0[5] += cur[5];
    arg0[6] += cur[6];
    arg0[7] = b;
    arg0[7] = arg0[7] + a;
    return 1;
}
