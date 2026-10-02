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
    float step;
    float rest;
    float sstep;
    int i;
    int n;

    if (e[0x25F] == 0 && e[0x23B] == 0x35) {
        d = func_0011DF78(*(float *)(e + 0x310) - *(float *)(e + 0xC4));
        if (d < 1.5707964f) {
            /* rest = ... below is a dead store (rest is reassigned before any
             * use); it only reproduces the original's FPU register allocation. */
            scale = func_0011DE90(*(float *)(e + 0x9C))
                  + d * ((1.0f - func_0011DE90(rest = *(float *)(e + 0x9C))) / 1.5707964f);
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
        /* compiled as !(rest < 0.0f): a NaN speed takes the +4 step */
        if (rest >= 0.0f) {
            step = 4.0f;
        } else {
            step = -4.0f;
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
