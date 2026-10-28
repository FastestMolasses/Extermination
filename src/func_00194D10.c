// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.73% NEARMISS).
// Fixes read from the original instructions:
//  - The "FP-compare branch-lowering wall" was the return shape: the original
//    keeps a result local initialised to 0 and returns it at the end, with
//    only the success case returning 1 early (the twin func_0012E2C0 has the
//    same tail). Separate `return 0` statements let mwcc fill the slot.
//  - The "arg-eval ordering" residual was the table spelling: the table is an
//    array of 0x40-byte records indexed by a2 (fields +0x00 and +0x04), and the
//    probe callee takes pointer arguments.

// Tests entity a1 against table entry a2: runs func_001B1EA0(0, a1+0xA0,
// &D_0024A5F0[a2], 4); if that succeeds, computes
// func_0011DF78(*(float*)(a1+0xA4) - D_0024A5F0[a2].y) and returns 1 when the
// result is below 4.0f; returns 0 otherwise (also when the probe fails).
// D_0024A5F0 is a stride-0x40 table; .y is its +0x04 field (splat's D_0024A5F4).
extern int func_001B1EA0(int, void *, void *, int);
extern float func_0011DF78(float);

typedef struct {
    float x;
    float y;
    char pad[0x38];
} Ent;

extern Ent D_0024A5F0[];

int func_00194D10(int a0, char *a1, int a2) {
    int ret;

    ret = 0;
    if (func_001B1EA0(0, a1 + 0xA0, &D_0024A5F0[a2], 4) != 0) {
        if (func_0011DF78(*(float *)(a1 + 0xA4) - D_0024A5F0[a2].y) < 4.0f) {
            return 1;
        }
    }
    return ret;
}
