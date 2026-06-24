// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Aim/firing-angle classifier. When the global enable byte at 0x70003B8D is
// clear, derives a target angle from two unsigned-byte params (D_00810E64,
// D_00810E65) scaled to radians (byte/256 * PI), runs them through the
// sin/cos helper func_0011DE90 (results cached at +0x244/+0x248), then
// atan2-style func_0011E620 and fabs func_0011DF78; the fabs result is stashed
// at 0x70003A20 and the angle quadrant is written as a code (0..3) to +0x24C.
// When 0x70003B8D is set, only the alt fire-state path (+4==1 && +5==9) runs,
// zeroing +0x23F and setting +0x24C=-1.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906), not 991202 (98.596): the lone
// residual under 991202 is a branch-delay/clean-store nop the 2.3.3 scheduler
// leaves where CW does. Two structural moves were required to reach 100%:
// (1) the D_00810E65 angle is computed into a temp BEFORE the D_00810E64
// func_0011DE90 call so the two calls emit in target order; (2) the
// `<= 3PI/4` test is written as `> 3PI/4` early-return so the v1=2 store is the
// fallthrough block (matches CW bc1t lowering). Verified objdiff 100% vs
// build/expected/func_00174FD0.o.
extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E620(float, float);
extern unsigned char D_00810E57;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

void func_00174FD0(char *arg0) {
    float a;
    float r;
    float af;
    float ang65;

    if (*(unsigned char *)0x70003B8D != 0) {
        if (*(unsigned char *)(arg0 + 4) == 1) {
            if (*(unsigned char *)(arg0 + 5) == 9) {
                *(unsigned char *)(arg0 + 0x23F) = 0;
                *(int *)(arg0 + 0x24C) = -1;
            }
        }
        return;
    }

    *(unsigned char *)(arg0 + 0x23F) = D_00810E57;
    if (*(unsigned char *)(arg0 + 0x23F) == 0) {
        *(int *)(arg0 + 0x24C) = -1;
        return;
    }

    ang65 = 3.1415927f * ((float)(unsigned int)D_00810E65 / 256.0f);
    *(float *)(arg0 + 0x244) = func_0011DE90(3.1415927f * ((float)(unsigned int)D_00810E64 / 256.0f));
    a = func_0011DE90(ang65);
    *(float *)(arg0 + 0x248) = a;
    r = func_0011E620(-a, *(float *)(arg0 + 0x244));
    af = func_0011DF78(r);
    *(float *)0x70003A20 = af;
    if (af > 2.3561945f) {
        *(int *)(arg0 + 0x24C) = 2;
        return;
    }
    if (af < 0.7853982f) {
        *(int *)(arg0 + 0x24C) = 3;
        return;
    }
    if (r < 0.0f) {
        *(int *)(arg0 + 0x24C) = 0;
        return;
    }
    *(int *)(arg0 + 0x24C) = 1;
}
