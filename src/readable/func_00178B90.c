// NEARMISS func_00178B90  (vram 0x00178B90, 0x330 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// FPU register colouring of the sub-step loop (the original keeps the step in f21 and the
// remainder in f23) and the sign test's branch polarity (if / else, ternary and negated spellings
// measured).
//
// The function links from the asm body in src/func_00178B90.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Player horizontal move: advances the position (+0xB0 x, +0xB8 z) by the
// speed +0x38 along the heading +0xC4 (x by sinf, z by cosf: func_0011E2A8 /
// func_0011DE90). On the uphill surface 0x35 (+0x23B) while not airborne
// (+0x25F clear) the step is scaled by cos(slope +0x9C) blended toward 1 by
// how far the heading deviates from the slope direction +0x310 (within
// pi/2). While airborne the scale is not applied. Speeds of 4.5 or more are
// split into 4-unit sub-steps, each followed by the collision push
// func_001764E0 when collide is set, then the remainder; slower moves take
// one step (and one push).
extern float func_0011DF78(float x);
extern float func_0011DE90(float x);
extern float func_0011E2A8(float x);
extern int float_to_int(float x);
extern void func_001764E0(unsigned char *e);

void func_00178B90(unsigned char *e, int collide) {
    float scale = 1.0f;
    float d;
    float rest;
    float step;
    float sstep;
    int i;
    int n;

    if (e[0x25F] == 0 && e[0x23B] == 0x35) {
        d = func_0011DF78(*(float *)(e + 0x310) - *(float *)(e + 0xC4));
        if (d < 1.5707964f) {
            scale = func_0011DE90(*(float *)(e + 0x9C))
                  + d * ((1.0f - func_0011DE90(*(float *)(e + 0x9C))) / 1.5707964f);
        }
    }
    if (func_0011DF78(*(float *)(e + 0x38)) < 4.5f) {
        if (e[0x25F] == 0) {
            *(float *)(e + 0xB0) += *(float *)(e + 0x38) * scale * func_0011E2A8(*(float *)(e + 0xC4));
            *(float *)(e + 0xB8) += *(float *)(e + 0x38) * scale * func_0011DE90(*(float *)(e + 0xC4));
        } else {
            *(float *)(e + 0xB0) += *(float *)(e + 0x38) * func_0011E2A8(*(float *)(e + 0xC4));
            *(float *)(e + 0xB8) += *(float *)(e + 0x38) * func_0011DE90(*(float *)(e + 0xC4));
        }
        if (collide != 0) {
            func_001764E0(e);
        }
    } else {
        rest = *(float *)(e + 0x38);
        if (rest < 0.0f) {
            step = -4.0f;
        } else {
            step = 4.0f;
        }
        n = float_to_int(rest / step);
        for (i = 0; i < n; i++) {
            sstep = step * scale;
            if (e[0x25F] == 0) {
                *(float *)(e + 0xB0) += sstep * func_0011E2A8(*(float *)(e + 0xC4));
                *(float *)(e + 0xB8) += sstep * func_0011DE90(*(float *)(e + 0xC4));
            } else {
                *(float *)(e + 0xB0) += step * func_0011E2A8(*(float *)(e + 0xC4));
                *(float *)(e + 0xB8) += step * func_0011DE90(*(float *)(e + 0xC4));
            }
            rest -= step;
            if (collide != 0) {
                func_001764E0(e);
            }
        }
        if (e[0x25F] == 0) {
            *(float *)(e + 0xB0) += rest * scale * func_0011E2A8(*(float *)(e + 0xC4));
            *(float *)(e + 0xB8) += rest * scale * func_0011DE90(*(float *)(e + 0xC4));
        } else {
            *(float *)(e + 0xB0) += rest * func_0011E2A8(*(float *)(e + 0xC4));
            *(float *)(e + 0xB8) += rest * func_0011DE90(*(float *)(e + 0xC4));
        }
        if (collide != 0) {
            func_001764E0(e);
        }
    }
}
