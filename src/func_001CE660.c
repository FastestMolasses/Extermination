// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003680 0x70003684

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 39.79% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): each VU0 macro
// instruction is its own asm statement; everything else is C.
// Fixes read from the original instructions:
//  - the sine and cosine of each sample are kept in two scratchpad words
//    (0x70003680 / 0x70003684, relocated externs via the SPAD line) and read
//    back from there for every product, not held in registers.
//  - the matrix-times-point kernel is inline VU0 code that reloads the four
//    matrix rows for every sample (VU0 rows 20..23) and transforms the two
//    points of the sample in place with an implicit w of 1.
//  - odd samples fill the quad's first pair (0x70003400), even samples the
//    second pair (0x70003420); the quad base 0x70003400 is passed on.
//
// One ring band of a textured strip: `band->steps` samples from start_deg to
// end_deg (degrees, converted with pi/180). For each sample the inner point is
// (in_rx * sin, in_y, in_rz * cos, 1) and the outer point (out_rx * sin, out_y,
// out_rz * cos, 1); both go through `mtx`. From the second sample on the quad
// of the last two samples is drawn by func_001CDDC0(layer, tag, quad, tex0,
// rgba).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

typedef struct {
    int   steps;
    float start_deg;
    float end_deg;
    float in_rx,  in_rz,  in_y;
    float out_rx, out_rz, out_y;
} RingBand;

extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_001CDDC0(int layer, int tag, Vec4 *quad,
                          unsigned long long tex0, unsigned int rgba);

extern Vec4 D_70003400[2];
extern Vec4 D_70003420[2];
extern float D_70003680;
extern float D_70003684;

void func_001CE660(int layer, int tag, const Mtx4 *mtx, const RingBand *band,
                   unsigned long long tex0, unsigned int rgba)
{
    float angle, step;
    int i;
    Vec4 *v;
    Vec4 *w;

    angle = band->start_deg * 3.14159274f / 180.0f;
    step  = (band->end_deg * 3.14159274f / 180.0f - angle)
            / (float)(band->steps - 1);

    for (i = 0; i < band->steps; i++) {
        D_70003680 = func_0011E2A8(angle);
        D_70003684 = func_0011DE90(angle);
        v = (i & 1) ? D_70003400 : D_70003420;

        v[0].x = band->in_rx * D_70003680;
        v[0].y = band->in_y;
        v[0].z = band->in_rz * D_70003684;
        v[0].w = 1.0f;
        v[1].x = band->out_rx * D_70003680;
        v[1].y = band->out_y;
        v[1].z = band->out_rz * D_70003684;
        v[1].w = 1.0f;

        asm("lqc2 vf20, 0x0(%0)" : : "r"(mtx));
        asm("lqc2 vf21, 0x10(%0)" : : "r"(mtx));
        asm("lqc2 vf22, 0x20(%0)" : : "r"(mtx));
        asm("lqc2 vf23, 0x30(%0)" : : "r"(mtx));
        asm("lqc2 vf1, 0x0(%0)" : : "r"(v));
        asm("vmulax.xyzw ACC, vf20, vf1x");
        asm("vmadday.xyzw ACC, vf21, vf1y");
        asm("vmaddaz.xyzw ACC, vf22, vf1z");
        asm("vmaddw.xyzw vf2, vf23, vf0w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(v));
        w = v + 1;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(w));
        asm("vmulax.xyzw ACC, vf20, vf1x");
        asm("vmadday.xyzw ACC, vf21, vf1y");
        asm("vmaddaz.xyzw ACC, vf22, vf1z");
        asm("vmaddw.xyzw vf2, vf23, vf0w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(w));

        if (i != 0) {
            func_001CDDC0(layer, tag, D_70003400, tex0, rgba);
        }
        angle += step;
    }
}
