// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 71.27% NEARMISS).
// Fixes read from the original instructions:
//  - the second cos() argument is the second sampled half-angle itself, not
//    cos(ang0) * half-angle (the product seen in a call delay slot is the
//    half-angle being computed); the offset vector is four floats with w = 1.0;
//  - the loop's sub-particle step is (i + 1) / 5 and both LCG fractions are
//    drawn inside the loop, each fully formed before the seed advances;
//  - the four parameter words at 0x00250E80/88/9C/AC are fields of the record
//    D_00250E70 handed to func_001CFBE0 (the -5.0 offsets are applied by
//    re-reading the fields after both products are stored);
//  - the radius wrap is a ternary, the trailing p+0x20 nudge uses compound
//    assignments, the tick limits are written as `>` tests, the switch has no
//    default arm, and the RNG scale is 2^31.
//
// Particle-burst emitter, dispatched on arg0+0x22C (p = arg0+0x1F0):
//   case 0: bumps the tick counter p+0x38; past 0x78 ticks it decays the
//     lifetime float p+0x24 by 1/90 and returns 1 once it goes negative.
//     Otherwise seeds an identity matrix at D_700036A0 relative to arg0+0xB0,
//     gets a particle-system handle via func_001CCF70(arg0+0xB0), then for
//     i = 1..4 advances the 32-bit LCG seed p+0x34 (seed*0x25+0xB) twice to
//     draw two [0,1) fractions, builds a wrapped (<= 2.0) radius from p+0x20,
//     fills the emit-parameter record (x/y offsets from p+0x28/p+0x2C scaled by
//     (i+1)/5 minus 5, scale 32*p+0x24, flags 0) and fires
//     func_001CFB50(...) + func_001CFBE0(handle, 1, &D_00250E70, buf, 1).
//     Finally nudges p+0x20 by 0.01 with the same wrap. Returns 0.
//   case 1: bumps p+0x38, returns 1 once it passes 0x3C; every 5th tick samples
//     two RNG angles, builds a randomly scaled offset vector from cos/sin,
//     transforms it through arg0+0xD0, builds a local matrix (identity, rotate
//     -pi/2, combined with arg0+0xD0, three rows scaled 1.2, translation = the
//     offset) and spawns an entity via func_001EFEB0, clearing its +0x38 word.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001026D0(void *a, void *b, void *c);
extern void func_00102900(void *a, void *b, float s);
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern void func_001029C0(void *a);
extern void func_00102B08(void *a, void *b, float s);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern int func_00122BB8(void);
extern int func_001CCF70(void *a0);
extern void func_001CFB50(void *dst, int b, void *src, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern char *func_001EFEB0(int a, void *b);

typedef struct {
    char pad0[0x10];
    float x;
    float pad14;
    float y;
    char pad1C[0x2C - 0x1C];
    float scale;
    char pad30[0x3C - 0x30];
    int flags;
} EmitParams;
extern EmitParams D_00250E70;
extern float D_700036A0;

int func_001C1570(char *arg0) {
    int i;
    int handle;
    char *p;
    int seed;
    float frac;
    float frac2;
    float radius;
    float t;
    char *ent;
    float buf[0x18];

    p = arg0 + 0x1F0;

    switch (*(int *)(arg0 + 0x22C)) {
    case 0:
        *(int *)(p + 0x38) = *(int *)(p + 0x38) + 1;
        if (*(int *)(p + 0x38) > 0x78) {
            *(float *)(p + 0x24) = *(float *)(p + 0x24) - 0.011111111f;
            if (*(float *)(p + 0x24) < 0.0f) {
                return 1;
            }
        }

        func_001029C0(&D_700036A0);
        func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
        handle = func_001CCF70(arg0 + 0xB0);
        seed = *(int *)(p + 0x34);

        for (i = 1; i < 5; i++) {
            frac = (float)((seed >> 0x10) & 0xFFFF);
            frac /= 65535.0f;
            frac += 0.0001f;
            seed = seed * 0x25 + 0xB;
            radius = *(float *)(p + 0x20) + 0.5f * frac;
            radius = (radius > 2.0f) ? radius - 1.0f : radius;

            t = (float)(i + 1) / 5.0f;
            D_00250E70.x = *(float *)(p + 0x28) * t;
            D_00250E70.y = *(float *)(p + 0x2C) * t;
            D_00250E70.x -= 5.0f;
            D_00250E70.y -= 5.0f;
            D_00250E70.scale = 32.0f * *(float *)(p + 0x24);
            D_00250E70.flags = 0;

            frac2 = (float)((seed >> 0x10) & 0xFFFF);
            frac2 /= 65535.0f;
            frac2 += 0.0001f;
            seed = seed * 0x25 + 0xB;
            func_001CFB50(buf, 0, &D_700036A0, radius, frac2, 1.0f, 0.1f, *(float *)(p + 0x30));
            func_001CFBE0(handle, 1, &D_00250E70, buf, 1);
        }

        *(float *)(p + 0x20) += 0.01f;
        if (!(*(float *)(p + 0x20) <= 2.0f)) {
            *(float *)(p + 0x20) -= 1.0f;
        }
        break;

    case 1:
        *(int *)(p + 0x38) = *(int *)(p + 0x38) + 1;
        if (*(int *)(p + 0x38) > 0x3C) {
            return 1;
        }
        if ((*(int *)(p + 0x38) % 5) == 0) {
            float ang0;
            float r;
            float ang1;
            float vec[4];
            float mtx[0x10];

            ang0 = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
            ang1 = 0.5f * (3.1415927f * ((float)func_00122BB8() / 2147483648.0f));
            r = *(float *)(p + 0x28) * func_0011DE90(ang0);
            vec[0] = r * ((float)func_00122BB8() / 2147483648.0f);
            r = *(float *)(p + 0x2C) * func_0011DE90(ang1);
            vec[1] = r * ((float)func_00122BB8() / 2147483648.0f);
            r = *(float *)(p + 0x28) * func_0011E2A8(ang0);
            vec[2] = r * ((float)func_00122BB8() / 2147483648.0f);
            vec[3] = 1.0f;

            func_001026A0(vec, arg0 + 0xD0, vec);
            func_001029C0(mtx);
            func_00102B08(mtx, mtx, -1.5707964f);
            func_001026D0(mtx, arg0 + 0xD0, mtx);
            func_00102900(mtx, mtx, 1.2f);
            func_00102900(mtx + 4, mtx + 4, 1.2f);
            func_00102900(mtx + 8, mtx + 8, 1.2f);
            func_00102948(mtx + 12, vec);

            ent = func_001EFEB0(0x80000025, mtx);
            if (ent != 0) {
                *(int *)(ent + 0x38) = 0;
            }
        }
        break;
    }
    return 0;
}
