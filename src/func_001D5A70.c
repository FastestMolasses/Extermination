// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 43.80% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - replaces the asm function body; the VU0 macro-mode work is written as one-instruction inline asm statements;
//  - the clip matrix stays loaded in VU0 rows 24..27 across all three vertices;
//    each vertex copy gets w = 1.0 and its six-bit clip flags are read from the
//    VU0 clip-flag register.
//
// Triangle clip classification. The three vertices at +0x40, +0x80 and +0xC0
// (w taken as 1.0) go through the clip matrix func_001CD370(0); each gets
// the VU0 clip judgement against |w| (six flags: +x, -x, +y, -y, +z, -z).
// Returns 0xFF when one flag is set for all three vertices (the triangle is
// wholly outside one plane), 1 when any vertex has a flag set (it needs
// clipping), else 0.

extern float *func_001CD370(int n);
extern void func_00102948(void *dst, void *src);

int func_001D5A70(int unused, char *tri) {
    float *m;
    float v[4];
    int f0;
    int f1;
    int f2;
    int fl;

    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    func_00102948(v, tri + 0x40);
    v[3] = 1.0f;
    {
        asm("lqc2 vf1, 0x0(%0)" : : "r"(v));
        asm("vmulax.xyzw ACC, vf24, vf1x");
        asm("vmadday.xyzw ACC, vf25, vf1y");
        asm("vmaddaz.xyzw ACC, vf26, vf1z");
        asm("vmaddw.xyzw vf2, vf27, vf0w");
        asm("vclipw.xyz vf2, vf2w");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("cfc2.ni %0, vi18" : "=r"(fl));
    }
    f0 = fl & 0x3F;
    func_00102948(v, tri + 0x80);
    v[3] = 1.0f;
    {
        asm("lqc2 vf1, 0x0(%0)" : : "r"(v));
        asm("vmulax.xyzw ACC, vf24, vf1x");
        asm("vmadday.xyzw ACC, vf25, vf1y");
        asm("vmaddaz.xyzw ACC, vf26, vf1z");
        asm("vmaddw.xyzw vf2, vf27, vf0w");
        asm("vclipw.xyz vf2, vf2w");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("cfc2.ni %0, vi18" : "=r"(fl));
    }
    f1 = fl & 0x3F;
    func_00102948(v, tri + 0xC0);
    v[3] = 1.0f;
    {
        asm("lqc2 vf1, 0x0(%0)" : : "r"(v));
        asm("vmulax.xyzw ACC, vf24, vf1x");
        asm("vmadday.xyzw ACC, vf25, vf1y");
        asm("vmaddaz.xyzw ACC, vf26, vf1z");
        asm("vmaddw.xyzw vf2, vf27, vf0w");
        asm("vclipw.xyz vf2, vf2w");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("vnop");
        asm("cfc2.ni %0, vi18" : "=r"(fl));
    }
    f2 = fl & 0x3F;
    if (f2 & (f0 & f1)) {
        return 0xFF;
    }
    if (f2 | (f0 | f1)) {
        return 1;
    }
    return 0;
}
