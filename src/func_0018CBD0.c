// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.04% NEARMISS).
// Fixes read from the original instructions:
// - The falloff local has no zero initialiser: the original never writes it
//   on the uncompressed path, which is why it occupies a callee-saved float
//   register live from entry. One local serves both falloff terms.
// - The three vector sums are compound assignments (operand order).
// - The eye falloff is an if/else: 0.5*(ang-f3) for the -20 preset, else
//   ang-f3 clamped to -10 (the shared pre-assignment was the wrong shape).
// - The func_00102C58 vector argument is a float pointer expression with no
//   extra cast (the cast swapped argument setup order).

// Seeds desired camera vectors from the scratchpad rotation and player+A0.
// The scalar tail adds preset-dependent heights and compression falloff.
// CBD0 is used by the panel script camera retarget (001B7B30 sub3).
// Original instructions and a captured panel camera establish
// targetY=playerY+17 and eyeY=playerY+19 for the -46.8 preset with zero
// rotation. Full helper and solver equivalence is separate.
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, float *a2);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern float func_0011E748(float a0);
extern float func_0011DF78(float a0);
extern char D_70003400[];
extern char D_70003600[];
extern char D_70003B50[];

void func_0018CBD0(char *arg0, char *arg1, float fparg0) {
    float f20, t;
    float x, z, ang, f3, f4, f5;
    float speed;

    speed = fparg0;
    func_00102948(arg0 + 0x30, D_70003B50);
    func_00102948(arg0 + 0x20, arg1 + 0xA0);
    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, (float *)arg0 + 0xC);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = speed;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
    *(float *)(arg0 + 0x10) += *(float *)(arg0 + 0x20);
    *(float *)(arg0 + 0x14) += *(float *)(arg0 + 0x24);
    *(float *)(arg0 + 0x18) += *(float *)(arg0 + 0x28);
    func_001028D0(D_70003600, arg0 + 0x20, arg0 + 0x10);
    z = *(float *)0x70003600;
    x = *(float *)0x70003608;
    ang = func_0011E748(z * z + x * x);
    ang = ang - func_0011DF78(speed);

    if (*(float *)(arg0 + 0x64) == -46.79999923706055f) {
        f3 = -20.0f;
        f4 = 6.0f;
        f5 = 2.0f;
    } else {
        f3 = -10.0f;
        f4 = 2.0f;
        f5 = 6.0f;
    }

    /* The target falloff is only assigned when the horizontal distance is
       compressed; otherwise the original reads it uninitialised (whatever the
       caller left in the callee-saved float register it lives in). */
    if (ang < f3) {
        t = f3 - ang;
        f20 = f3 + t;
        if (f20 > -7.0f) {
            f20 = -7.0f;
        }
    }

    *(float *)(arg0 + 0x24) = 11.0f + (*(float *)(arg1 + 0xA4) + f4 + 0.30000001192092896f * f20);

    if (ang < f3) {
        if (f3 == -20.0f) {
            f20 = 0.5f * (ang - f3);
        } else {
            f20 = ang - f3;
            if (f20 < -10.0f) {
                f20 = -10.0f;
            }
        }
        *(float *)(arg0 + 0x14) = 11.0f + (f4 + (*(float *)(arg1 + 0xA4) + (f5 - f20)));
    } else {
        *(float *)(arg0 + 0x14) = 11.0f + (f4 + (f5 + *(float *)(arg1 + 0xA4)));
    }
}
