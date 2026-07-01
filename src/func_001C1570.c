// NEARMISS func_001C1570  (vram 0x001C1570, 0x504 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Broad register-allocation/scheduling churn in the LCG loop and trig/matrix chain (large register-pressure function); one design detail (second cos() argument reusing the first cos()'s result * half-angle via delay-slot register reuse) is a medium-confidence read of unusual target register chainin...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS ~72% (mwcc 991202 = 71.9%, mwcc233 = 71.3%). Particle-burst emitter,
// dispatched on arg0+0x22C:
//   case 0: decays a lifetime float at p+0x24 once p+0x38's tick counter
//     passes 0x78 (returns 1 / "done" when it goes negative); otherwise seeds
//     an identity/parent-relative matrix at D_700036A0, gets a particle-system
//     handle via func_001CCF70(arg0+0xB0), then LOOPs 4 times (i=1..4)
//     advancing a 32-bit LCG seed (seed = seed*0x25+0xB) TWICE per iteration to
//     draw two independent [0,1) fractions, uses them to build a clamped
//     [0,2] "radius" and per-axis scale/offset globals (D_00250E80/88/9C/AC),
//     and fires func_001CFB50(...)+func_001CFBE0(handle,1,&D_00250E70,buf,1)
//     per sub-particle. Then nudges p+0x20 by 0.01 with the same [0,2] wrap.
//   case 1: advances p+0x38's counter, returns 1 once it hits 0x3D; every 5th
//     tick samples two RNG angles and chains cos/sin calls to build a scaled
//     offset vector, transforms it through a local 4x4-ish stack matrix
//     (identity -> rotate -pi/2 -> combine with arg0+0xD0 -> three 1.2x scales)
//     and spawns/repositions an entity via func_001EFEB0.
//
// Fully recovered: LCG idiom (matches sibling func_001EB600's seed*0x25+0xB
// pattern), func_001CFB50/func_001CFBE0 signatures (matches func_00187690's
// committed decl), and the local-stack-matrix layout at sp+0xD0 (NOT the
// global D_700036A0 -- confirmed by tracing sp-relative addiu's in the target
// asm; frame size now matches target exactly, -0x110).
//
// MEDIUM CONFIDENCE CAVEAT: in case 1 the target's register chaining shows
// the SECOND cos() call's argument is literally the product
// cos(ang0)*half_ang1 (an f22 register reused/overwritten in the delay slot
// of the first cos() call), not a second independently-sampled angle as a
// naive read would suggest. This is reproduced faithfully below but is a
// judgment call on unusual delay-slot register reuse -- flagged for review.
//
// WALL: after the above fixes, remaining diff is broad register-allocation /
// instruction-scheduling churn through the loop body and the second block's
// trig/matrix chain (typical of a large, register-pressure-heavy function);
// no further structural lever found within budget.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001026D0(void *a, void *b, void *c);
extern void func_00102900(void *a, void *b, float s);
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern void func_001029C0(void *a);
extern void func_00102B08(void *a, void *b, float s);
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */
extern int func_00122BB8(void);
extern int func_001CCF70(void *a0);
extern void func_001CFB50(void *dst, int b, void *src, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void *func_001EFEB0(int a, void *b);

extern float D_00250E70;
extern float D_00250E80;
extern float D_00250E88;
extern float D_00250E9C;
extern int D_00250EAC;
extern float D_700036A0;

int func_001C1570(char *arg0) {
    char *p;
    float f;
    int seed;
    int handle;
    int i;
    float frac;
    float radius;
    void *ent;
    float sp60[0x18];

    p = arg0 + 0x1F0;

    switch (*(int *)(arg0 + 0x22C)) {
    case 0:
        *(int *)(p + 0x38) = *(int *)(p + 0x38) + 1;
        if (*(int *)(p + 0x38) >= 0x79) {
            f = *(float *)(p + 0x24) - 0.011111111f;
            *(float *)(p + 0x24) = f;
            if (f < 0.0f) {
                return 1;
            }
        }

        func_001029C0(&D_700036A0);
        func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
        seed = *(int *)(p + 0x34);
        handle = func_001CCF70(arg0 + 0xB0);
        frac = (float) ((seed >> 0x10) & 0xFFFF) / 65535.0f;

        for (i = 1; i < 5; i++) {
            int seed2 = seed * 0x25 + 0xB;

            radius = *(float *)(p + 0x20) + 0.5f * (frac + 0.0001f);
            if (!(radius <= 2.0f)) {
                radius -= 1.0f;
            }

            D_00250E80 = *(float *)(p + 0x28) * ((float) i / 5.0f) - 5.0f;
            D_00250E88 = *(float *)(p + 0x2C) * ((float) i / 5.0f) - 5.0f;
            D_00250E9C = 32.0f * *(float *)(p + 0x24);
            D_00250EAC = 0;

            func_001CFB50(sp60, 0, &D_700036A0, radius,
                          (float) ((seed2 >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f,
                          1.0f, 0.1f, *(float *)(p + 0x30));
            func_001CFBE0(handle, 1, &D_00250E70, sp60, 1);

            seed = seed2 * 0x25 + 0xB;
            frac = (float) ((seed >> 0x10) & 0xFFFF) / 65535.0f;
        }

        f = *(float *)(p + 0x20) + 0.01f;
        if (!(f <= 2.0f)) {
            f -= 1.0f;
        }
        *(float *)(p + 0x20) = f;
        return 0;

    case 1:
        *(int *)(p + 0x38) = *(int *)(p + 0x38) + 1;
        if (*(int *)(p + 0x38) >= 0x3D) {
            return 1;
        }
        if ((*(int *)(p + 0x38) % 5) == 0) {
            /*
             * Random spherical-ish offset: sample two angles ang0/half1 from
             * the RNG, then chain three trig calls to build a scaled offset
             * vector, each axis additionally scaled by its own RNG fraction:
             *   sp[0] = (p28 * cos(ang0))                 * rndFrac
             *   sp[1] = (p2C * cos(cos(ang0) * half1))    * rndFrac
             *   sp[2] = (p28 * sin(ang0))                 * rndFrac
             * (the second axis's trig argument really is the *product*
             * cos(ang0)*half1, per the target's exact register chaining --
             * not a typo).
             */
            float ang0;
            float half1;
            float composed;
            float c0;
            float rad0;
            float rad1;
            float sp[3];
            float mtx[0x10];

            ang0 = (6.2831855f * ((float) func_00122BB8() / 2147483600.0f)) - 3.1415927f;
            half1 = 0.5f * (3.1415927f * ((float) func_00122BB8() / 2147483600.0f));
            c0 = func_0011DE90(ang0);
            composed = c0 * half1;

            rad0 = *(float *)(p + 0x28) * c0;
            sp[0] = rad0 * ((float) func_00122BB8() / 2147483600.0f);

            rad1 = *(float *)(p + 0x2C) * func_0011DE90(composed);
            sp[1] = rad1 * ((float) func_00122BB8() / 2147483600.0f);

            radius = *(float *)(p + 0x28) * func_0011E2A8(ang0);
            sp[2] = radius * ((float) func_00122BB8() / 2147483600.0f);

            func_001026A0(sp, arg0 + 0xD0, sp);
            func_001029C0(mtx);
            func_00102B08(mtx, mtx, -1.5707964f);
            func_001026D0(mtx, arg0 + 0xD0, mtx);
            func_00102900(mtx, mtx, 1.2f);
            func_00102900((char *)mtx + 0x10, (char *)mtx + 0x10, 1.2f);
            func_00102900((char *)mtx + 0x20, (char *)mtx + 0x20, 1.2f);
            func_00102948((char *)mtx + 0x30, sp);

            ent = func_001EFEB0(-0x7FFFFFDB, mtx);
            if (ent != 0) {
                *(int *)((char *)ent + 0x38) = 0;
            }
        }
        return 0;
    }
    return 0;
}
