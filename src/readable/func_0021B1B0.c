// NEARMISS func_0021B1B0  (vram 0x0021B1B0, 0x344 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 95.22% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register allocation of the LCG step (the original keeps next / k / brightness in s1 / s3 / s2)
// and FPU temporaries; the arithmetic order matches.
//
// The function links from the asm body in src/func_0021B1B0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Draws the horizon line strip of the area-load / area-state effect: 512
// segments across the screen, each from the previous point to (x, y + noise),
// with a brightness pulse travelling along the strip. e+4 is the pulse
// phase (0..1), e+8 its strength, e+0x18 the base y, e+0x14 a 32-bit LCG
// state (reseeded to 0x07234567 here, stepped s = s * 5 + 1 every fifth
// segment). The noise for segment i is the linear blend of the high halves
// of the current and next LCG states, scaled by 1280 * w^16 with
// w = 1 - |1 - 2 * i / 512| (+0.2 on every 64th segment). Brightness b =
// 255 * strength * d^4 for the phase distance d (0 below 0.1), colour
// (b/4, 3b/8, b, b). After the strip two fades are drawn with
// func_001DFA40 under render states 7 and 2.
typedef struct V4i {
    int x, y, z, w;
} V4i;

extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D63B0(int ctx, V4i *p0, V4i *c0, V4i *p1, V4i *c1);
extern void func_001DFA40(int a0, int a1, unsigned long long colour, float a, float b);
extern void func_00102948(void *dst, void *src);
extern int float_to_int(float x);
extern float func_0011DF78(float x);

void func_0021B1B0(char *e) {
    V4i c0;
    V4i c1;
    V4i p0;
    V4i p1;
    int i;
    unsigned int next;
    int b;
    int k;
    unsigned int s;
    float fv;
    float t;
    float d;
    float w;

    *(unsigned int *)(e + 0x14) = 0x07234567;
    c0.z = 0;
    c0.y = 0;
    c0.x = 0;
    p0.x = 0;
    c0.w = 0x80;
    p0.y = *(int *)(e + 0x18);
    p0.z = 0;
    p0.w = 0;
    func_001D1F80(0, 0, 7);
    for (i = 0; i < 0x200; i++) {
        s = *(unsigned int *)(e + 0x14);
        k = i % 5;
        next = s * 5 + 1;
        fv = (float)(((s >> 16) * (5 - k) + (next >> 16) * k) / 5) / 65536.0f - 0.5f;
        t = (float)i / 512.0f;
        d = t - *(float *)(e + 4);
        if (d < 0.0f) {
            d += 1.0f;
        }
        d = d * d;
        d = d * d;
        if (d < 0.1f) {
            d = 0.0f;
        }
        b = float_to_int(255.0f * (d * *(float *)(e + 8)));
        w = (0.5f - func_0011DF78(0.5f - t)) / 0.5f;
        w = w * w;
        w = w * w;
        w = w * w;
        w = w * w;
        if (!(i & 0x3F)) {
            w += 0.2f;
        }
        p1.x = (i + 0x700) * 16;
        fv *= 1280.0f;
        fv *= w;
        p1.y = float_to_int((float)*(int *)(e + 0x18) + fv);
        p1.z = 0xFFFFFF;
        c1.x = (b * 2) >> 3;
        c1.y = (b * 3) >> 3;
        c1.z = b;
        p1.w = 0;
        c1.w = b;
        func_001D63B0(0, &p0, &c0, &p1, &c1);
        if (k == 4) {
            *(unsigned int *)(e + 0x14) = next;
        }
        func_00102948(&c0, &c1);
        func_00102948(&p0, &p1);
    }
    func_001D1F80(0, 0, 7);
    func_001DFA40(0, 0, 0x80808080, -0.45f, 5.5f);
    func_001D1F80(0, 0, 2);
    func_001DFA40(0, 0x40, 0x40404040, -0.45f, 2.75f);
}
