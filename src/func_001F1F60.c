// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// fx_render: 6x10 particle burst effect driven by the state byte at arg0+4 (owner
// record = *(arg0+0x24)); acquires 6 fx slots, emits/updates the particle quads with
// an LCG jitter, sprays the expanding ring sprites, and tears down at state 3.

extern int D_00275B40;
extern char D_0025A0F0[];
extern char D_0025A180[];
extern char D_700036A0[];
extern char D_700036E0[];
extern char D_70003710[];
extern char D_700038A0[];

extern int func_001F1550(void *, int);
extern void func_00102948(void *dst, void *src);
extern int func_00122BB8(void);
extern void func_001029C0(void *);
extern void func_00102760(void *a, void *b);
extern void func_001CD390(void *, void *);
extern void func_001F1DD0(void *, void *, void *);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102870(void *, void *, float);
extern void func_001028B8(void *a, void *b, void *c);
extern unsigned int func_00128250(float);
extern int func_001CD520(int, int, void *, long long, int, float, float, float);
extern int func_001CCF70(void *);
extern void func_001CFA60(void *, void *, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern void func_00102A60(void *, void *, float);
extern void func_00102B08(void *, void *, float);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_001AFC10(void *);

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
    float g;

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
            r = (float)((seed >> 0x10) & 0xFFFF);
            life = (char *)*(int *)(D_00275B40 + i * 4);
            r = r / 65535.0f;
            *(int *)(p + 0xC) = seed * 0x25 + 0xB;
            quad = life;
            r += 9.999999747378752e-05f;
            *(float *)0x70003A24 = r;
            for (j = 0; j < 0xA; j++, life += 4, quad += 0x10) {
                if (!(*(float *)(life + 0xA0) < 0.0f)) {
                    mode = *(unsigned char *)(arg0 + 0xD);
                    switch (mode) {
                    case 0:
                        *(int *)0x700038A0 = 0x43400000;
                        *(int *)0x700038A4 = 0x43400000;
                        *(int *)0x700038A8 = 0x43400000;
                        *(float *)0x700038AC = 48.0f * (1.0f - *(float *)(life + 0xA0));
                        g = *(float *)0x700038A0;
                        *(float *)0x70003A20 = 2.0f + (2.0f * *(float *)(life + 0xA0));
                        packed = func_00128250(g);
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
                        g = *(float *)0x700038A0;
                        *(float *)0x70003A20 = 2.0f + (3.0f * *(float *)(life + 0xA0));
                        packed = func_00128250(g);
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
                r = (float)((seed >> 0x10) & 0xFFFF);
                r = r / 65535.0f;
                *(int *)(p + 0xC) = seed * 0x25 + 0xB;
                r += 9.999999747378752e-05f;
                func_001CFA60(&spB0, arg0 + 0xD0, *(float *)(p + 0x14), r);
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
                    r = (float)((seed >> 0x10) & 0xFFFF);
                    r = r / 65535.0f;
                    *(int *)(p + 0xC) = seed * 0x25 + 0xB;
                    r += 9.999999747378752e-05f;
                    func_001CFA60(&spB0, D_700036E0, *(float *)(p + 0x14), r);
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
