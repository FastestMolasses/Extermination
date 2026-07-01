// NEARMISS func_001DB9D0  (vram 0x001DB9D0, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU-MAC wall (genuine, confirmed against sibling func_001D66A0 which documents the identical class): target uses mula.s/madd.s/msub.s accumulator fusion for the pre-loop rotation seed; no plain-C expression makes mwcc select the MAC pipeline for this pattern instead of separate mul.s+add.s/sub.s....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int float_to_int(float);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001D1F80(int, int, int);
extern float D_00253250;
extern float D_00253254;
extern float D_0025325C;
extern int D_00253260;
extern int D_00253264;
extern int D_00253268;
extern int D_0025326C;
extern int D_00253270;
extern int D_00253274;
extern int D_00253278;
extern int D_0025327C;
extern int D_00275670[];

void func_001DB9D0(int arg0, float dt) {
    float denom = 768.0f + (-568.0f * dt);
    float c0 = func_0011E2A8(0.0f);
    float s0 = func_0011DE90(0.0f);
    float cStep = func_0011E2A8(0.09817477f);
    float sStep;
    float scale;
    float k;
    float vx0;
    float vy0;
    float f22;
    float f23;
    float f24;
    float f25;
    int i;
    char *dl;
    char *pkt;
    char *p;

    sStep = func_0011DE90(0.09817477f);
    scale = D_0025325C / denom;
    vx0 = denom * c0;
    vy0 = denom * s0;
    k = 2.0f * cStep;
    f24 = vx0 * cStep + sStep * vy0;   /* mula.s f22,f25 / madd.s f24,f0,f23 */
    f25 = vy0 * cStep - sStep * vx0;   /* mula.s f23,f25 / msub.s f25,f0,f22 */
    f22 = vx0;
    f23 = vy0;
    func_001D1F80(arg0, 0, 1);

    dl = (char *)(D_00275670 + arg0 * 4);
    p = *(char **)(dl + 0x10);
    p[3] = 0x10;
    *(int *)(p + 4) = 0;
    *(short *)(p + 0) = 0x86;
    pkt = p;
    *(char **)(dl + 0x10) = pkt + 0x870;
    *(int *)(pkt + 0x10) = 0;
    *(int *)(pkt + 0x14) = 0;
    *(int *)(pkt + 0x18) = 0;
    *(int *)(pkt + 0x1C) = 0x50000085;
    *(int *)(pkt + 0x20) = 0x8021;
    *(int *)(pkt + 0x24) = 0x40264000;
    *(int *)(pkt + 0x28) = 0x4141;
    *(int *)(pkt + 0x2C) = 0;

    p = pkt + 0x30;
    i = 0;
    do {
        *(int *)(p + 0x00) = D_00253260;
        *(int *)(p + 0x04) = D_00253264;
        *(int *)(p + 0x08) = D_00253268;
        *(int *)(p + 0x0C) = D_0025326C;
        *(int *)(p + 0x20) = D_00253270;
        *(int *)(p + 0x24) = D_00253274;
        *(int *)(p + 0x28) = D_00253278;
        *(int *)(p + 0x2C) = D_0025327C;
        *(int *)(p + 0x10) = float_to_int(16.0f * (D_00253250 + (0.8f * (f22 * scale))));
        *(int *)(p + 0x14) = float_to_int(16.0f * (D_00253254 + (0.5f * (f23 * scale))));
        *(int *)(p + 0x18) = 0xFFFFFF;
        *(int *)(p + 0x1C) = 0;
        *(int *)(p + 0x30) = float_to_int(16.0f * ((0.8f * f22) + D_00253250));
        *(int *)(p + 0x34) = float_to_int(16.0f * ((0.5f * f23) + D_00253254));
        *(int *)(p + 0x38) = 0xFFFFFF;
        f22 -= k * f24;
        i += 1;
        *(int *)(p + 0x3C) = 0;
        p += 0x40;
        f23 -= k * f25;
        f24 += k * f22;
        f25 += k * f23;
    } while (i < 0x21);
}
