// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.34% NEARMISS).
// Fixes read from the original instructions:
// - x and y are halved into fresh locals (hx, hy) declared AFTER the sine/cosine
//   locals, so they take the later saved FPRs; z is halved in place, spelled
//   `z = z / 2.0f` (the compound `/=` form colours differently).
//
// Builds a quaternion from three Euler angles: halves each angle, takes the sine
// (func_0011E2A8) and cosine (func_0011DE90) of each half angle, and writes the
// four components to q[0..3].

extern float func_0011E2A8(float);
extern float func_0011DE90(float);

void func_001CA3B0(float *q, float x, float y, float z) {
    float sx, sy, sz, cx, cy, cz, hx, hy;
    hx = x / 2.0f;
    sx = func_0011E2A8(hx);
    hy = y / 2.0f;
    sy = func_0011E2A8(hy);
    z = z / 2.0f;
    sz = func_0011E2A8(z);
    cx = func_0011DE90(hx);
    cy = func_0011DE90(hy);
    cz = func_0011DE90(z);
    q[0] = cx * (sz * cy) - sx * (cz * sy);
    q[1] = cx * (cz * sy) + sx * (sz * cy);
    q[2] = sx * (cz * cy) - cx * (sz * sy);
    q[3] = cx * (cz * cy) + sx * (sz * sy);
}
