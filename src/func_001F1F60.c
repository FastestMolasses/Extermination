// NEARMISS func_001F1F60  (vram 0x001F1F60, 0x968 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP web-targeting/coloring + list-scheduler permutation (permuter class, NOT the clean-store nop). Body, control flow, all call sites and constants fully recovered; instruction count equal (630==630 on mwcc233). Residuals: (1) the LCG-jitter float chain ((seed>>16&0xFFFF)/65535+1e-4, 3 sites) - ta...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// fx_render: 6x10 particle burst effect, state machine on the byte at arg0+4
// (ctx = owner record at *(arg0+0x24)).
//   state 0: acquire 6 fx slots via func_001F1550(arg0,6); mark all 60 particle
//     lifetimes (offset +0xA0..+0xC4 in each slot list of *D_00275B40) dead
//     (-1.0f); snapshot the owner transform (func_00102948 into p+0x20 from
//     ctx+0x100); zero cursors, seed the LCG at p+8 from func_00122BB8, ring
//     radius p+0x14 = 0.2f; advance to state 1.
//   states 1/2: owner state 3 aborts to state 3. State 1 only: on first tick
//     build the emit quat in D_700038A0 keyed on mode (arg0+0xD): mode 0 from
//     ctx+0xC0, mode 1 from ctx+0x70 (func_001029C0/func_00102760), commit via
//     func_001CD390, snapshot position arg0+0x100..108 from +0xB0..B8; then
//     spawn-step func_001F1DD0 (mode 0 runs a second pass with the transform
//     rotated by 2.0 via func_001028D0/00102870/001028B8) and re-snapshot the
//     owner transform. Both states then update all 6x10 particles: per-slot LCG
//     (seed = seed*37 + 11) writes the jitter float at 0x70003A24; live
//     particles (life >= 0) draw a func_001CD520 quad with RGBA packed through
//     func_00128250 clamps -- mode 0: grey 192, alpha 48*(1-life), scale
//     2+2*life, life += 0.08; mode 1: grey 80, alpha 80*(1-life^2), scale
//     2+3*life, life += 0.02. Lifetimes past 1.0 (or already dead) are counted
//     in `died` and clamped to -1.  While the ring radius p+0x14 < 1.5: mode 0
//     sprays one sprite (func_001CCF70/001CFA60/001CFBE0 with D_0025A0F0),
//     radius += 0.06; mode 1 sprays 4 sprites rotated +-pi/4 / +-3pi/4 about
//     the emit axis (D_700036A0/D_700036E0, table D_0025A180), radius += 0.04.
//     State 2 completes (state=3) once all 60 particles died and radius >= 1.5;
//     state 1 mode 1 instead draws the pulsing full-ring quad (scale 2..3 from
//     func_00122BB8) at ctx+0x100.
//   state 3: teardown via func_001AFC10(arg0).

extern int D_00275B40;
extern char D_0025A0F0[];
extern char D_0025A180[];
extern char D_700036A0[];
extern char D_700036E0[];
extern char D_70003710[];
extern char D_700038A0[];

extern int func_001F1550();
extern void func_00102948();
extern int func_00122BB8();
extern void func_001029C0();
extern void func_00102760();
extern void func_001CD390();
extern void func_001F1DD0();
extern void func_001028D0();
extern void func_00102870(void *, void *, float);
extern void func_001028B8();
extern unsigned int func_00128250(float);
extern int func_001CD520(int, int, void *, long long, int, float, float, float);
extern int func_001CCF70();
extern void func_001CFA60(void *, void *, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern void func_00102A60(void *, void *, float);
extern void func_00102B08(void *, void *, float);
extern void func_001026D0();
extern void func_001AFC10();

static inline long long shl64(long long x, int n) {
    return x << n;
}

struct Blk60 { unsigned char b[0x60]; };

void func_001F1F60(char *arg0) {
    struct Blk60 spB0;
    char *ctx;
    char *p;
    int died;
    int packed;
    int handle;
    int k;
    char *quad;
    char *life;
    int i, j;
    char *lp;
    int seed;
    float r;
    unsigned char st;
    unsigned char mode;
    float f;

    ctx = *(char **)(arg0 + 0x24);
    st = *(unsigned char *)(arg0 + 4);
    p = arg0 + 0x1F0;

    switch (st) {
    case 0:
        if (func_001F1550(arg0, 6) != 0) {
            for (i = 0; i < 6; i++) {
                lp = (char *)*(int *)(D_00275B40 + i * 4);
                for (j = 0; j < 0xA; j++, lp += 4) {
                    *(float *)(lp + 0xA0) = -1.0f;
                }
            }
            func_00102948(p + 0x20, ctx + 0x100);
            *(int *)(p + 0x10) = 0;
            *(int *)p = 0;
            *(int *)(p + 4) = 0;
            *(int *)(p + 8) = func_00122BB8();
            *(float *)(p + 0x14) = 0.2f;
            *(unsigned char *)(arg0 + 4) = 1;
        }
        break;
    case 1:
    case 2:
        if (*(unsigned char *)(ctx + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        if (st == 1) {
            if (*(int *)(p + 0x10) == 0) {
                *(int *)(p + 0x10) += 1;
                mode = *(unsigned char *)(arg0 + 0xD);
                switch (mode) {
                case 0:
                    func_001029C0(arg0 + 0xD0);
                    func_00102760(D_700038A0, ctx + 0xC0);
                    break;
                case 1:
                    func_001029C0(arg0 + 0xD0);
                    func_00102760(D_700038A0, ctx + 0x70);
                    break;
                }
                func_001CD390(arg0 + 0xD0, D_700038A0);
                *(float *)(arg0 + 0x100) = *(float *)(arg0 + 0xB0);
                *(float *)(arg0 + 0x104) = *(float *)(arg0 + 0xB4);
                *(float *)(arg0 + 0x108) = *(float *)(arg0 + 0xB8);
            }
            func_001F1DD0(arg0, p, ctx + 0x100);
            if (*(unsigned char *)(arg0 + 0xD) == 0) {
                func_001028D0(D_700038A0, ctx + 0x100, p + 0x20);
                func_00102870(D_700038A0, D_700038A0, 2.0f);
                func_001028B8(D_700038A0, D_700038A0, ctx + 0x100);
                func_001F1DD0(arg0, p, D_700038A0);
            }
            func_00102948(p + 0x20, ctx + 0x100);
        }
        died = 0;
        *(int *)(p + 0xC) = *(int *)(p + 8);
        for (i = 0; i < 6; i++) {
            seed = *(int *)(p + 0xC);
            life = (char *)*(int *)(D_00275B40 + i * 4);
            *(int *)(p + 0xC) = seed * 0x25 + 0xB;
            quad = life;
            r = (float)((seed >> 0x10) & 0xFFFF);
            r = r / 65535.0f;
            *(float *)0x70003A24 = r + 9.999999747378752e-05f;
            for (j = 0; j < 0xA; j++, life += 4, quad += 0x10) {
                if (!(*(float *)(life + 0xA0) < 0.0f)) {
                    mode = *(unsigned char *)(arg0 + 0xD);
                    switch (mode) {
                    case 0:
                        *(int *)0x700038A0 = 0x43400000;
                        *(int *)0x700038A4 = 0x43400000;
                        *(int *)0x700038A8 = 0x43400000;
                        *(float *)0x700038AC = 48.0f * (1.0f - *(float *)(life + 0xA0));
                        *(float *)0x70003A20 = 2.0f + (2.0f * *(float *)(life + 0xA0));
                        packed = func_00128250(*(float *)0x700038A0);
                        packed |= func_00128250(*(float *)0x700038A4) << 8;
                        packed |= func_00128250(*(float *)0x700038A8) << 0x10;
                        packed |= func_00128250(*(float *)0x700038AC) << 0x18;
                        f = *(float *)0x70003A20;
                        func_001CD520(0, 1, quad,
                                      shl64((long long)0x20041285, 0x20) | 0x55322090,
                                      packed, f, f, 0.0f);
                        *(float *)(life + 0xA0) += 0.07999999821186066f;
                        break;
                    case 1:
                        *(int *)0x700038A0 = 0x42A00000;
                        *(int *)0x700038A4 = 0x42A00000;
                        *(int *)0x700038A8 = 0x42A00000;
                        *(float *)0x700038AC = 80.0f * (1.0f - (*(float *)(life + 0xA0) * *(float *)(life + 0xA0)));
                        *(float *)0x70003A20 = 2.0f + (3.0f * *(float *)(life + 0xA0));
                        packed = func_00128250(*(float *)0x700038A0);
                        packed |= func_00128250(*(float *)0x700038A4) << 8;
                        packed |= func_00128250(*(float *)0x700038A8) << 0x10;
                        packed |= func_00128250(*(float *)0x700038AC) << 0x18;
                        f = *(float *)0x70003A20;
                        func_001CD520(0, 1, quad,
                                      shl64((long long)0x20041285, 0x20) | 0x55322090,
                                      packed, f, f, 0.0f);
                        *(float *)(life + 0xA0) += 0.019999999552965164f;
                        break;
                    }
                }
                if (*(float *)(life + 0xA0) > 1.0f ||
                    *(float *)(life + 0xA0) == -1.0f) {
                    died += 1;
                    *(float *)(life + 0xA0) = -1.0f;
                }
            }
        }
        if (*(float *)(p + 0x14) < 1.5f) {
            mode = *(unsigned char *)(arg0 + 0xD);
            switch (mode) {
            case 0:
                handle = func_001CCF70(arg0 + 0x100);
                seed = *(int *)(p + 0xC);
                *(int *)(p + 0xC) = seed * 0x25 + 0xB;
                r = (float)((seed >> 0x10) & 0xFFFF);
                r = r / 65535.0f;
                func_001CFA60(&spB0, arg0 + 0xD0, *(float *)(p + 0x14),
                              r + 9.999999747378752e-05f);
                func_001CFBE0(handle, 1, D_0025A0F0, &spB0, 0);
                *(float *)(p + 0x14) += 0.05999999865889549f;
                break;
            case 1:
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, 1.919862151145935f);
                for (k = 0; k < 4; k++) {
                    switch (k) {
                    case 0:
                        func_00102A60(D_700036E0, D_700036A0, 0.7853981852531433f);
                        break;
                    case 1:
                        func_00102A60(D_700036E0, D_700036A0, 2.356194496154785f);
                        break;
                    case 2:
                        func_00102A60(D_700036E0, D_700036A0, -0.7853981852531433f);
                        break;
                    case 3:
                        func_00102A60(D_700036E0, D_700036A0, -2.356194496154785f);
                        break;
                    }
                    func_001026D0(D_700036E0, arg0 + 0xD0, D_700036E0);
                    handle = func_001CCF70(D_70003710);
                    seed = *(int *)(p + 0xC);
                    *(int *)(p + 0xC) = seed * 0x25 + 0xB;
                    r = (float)((seed >> 0x10) & 0xFFFF);
                    r = r / 65535.0f;
                    func_001CFA60(&spB0, D_700036E0, *(float *)(p + 0x14),
                                  r + 9.999999747378752e-05f);
                    func_001CFBE0(handle, 1, D_0025A180, &spB0, 0);
                }
                *(float *)(p + 0x14) += 0.03999999910593033f;
                break;
            }
        }
        if (*(unsigned char *)(arg0 + 4) == 2) {
            if (died == 0x3C && !(*(float *)(p + 0x14) < 1.5f)) {
                *(unsigned char *)(arg0 + 4) = 3;
            }
        } else {
            if (*(unsigned char *)(arg0 + 0xD) == 1) {
                f = 2.0f + ((float)func_00122BB8() / 2147483648.0f);
                *(float *)0x70003A20 = f;
                func_001CD520(0, 2, ctx + 0x100,
                              shl64((long long)0x20041605, 0x20) | 0x113222CD,
                              0x609090, f, f, 2.0f);
            }
        }
        break;
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
