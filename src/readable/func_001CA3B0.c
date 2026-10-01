// NEARMISS func_001CA3B0  (vram 0x001CA3B0, 0x118 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// FPU register colouring of the half angles: the original divides x into a fresh register, copies
// y before halving it into another and halves z in place, so its five saved FPRs are assigned
// differently; the call sequence and the eight products match (eight in-place / fresh-local
// combinations measured).
//
// The function links from the asm body in src/func_001CA3B0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011E2A8(float);
extern float func_0011DE90(float);

void func_001CA3B0(float *q, float x, float y, float z) {
    float hx, hy, hz, sx, sy, sz, cx, cy, cz;
    x /= 2.0f;
    sx = func_0011E2A8(x);
    y /= 2.0f;
    sy = func_0011E2A8(y);
    z /= 2.0f;
    sz = func_0011E2A8(z);
    cx = func_0011DE90(x);
    cy = func_0011DE90(y);
    cz = func_0011DE90(z);
    q[0] = cx * (sz * cy) - sx * (cz * sy);
    q[1] = cx * (cz * sy) + sx * (sz * cy);
    q[2] = sx * (cz * cy) - cx * (sz * sy);
    q[3] = cx * (cz * cy) + sx * (sz * sy);
}
