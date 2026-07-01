// NEARMISS func_0021A500  (vram 0x0021A500, 0x5BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.22% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 89.22% — full logic recovered (state machine on e[4] driving a particle/twinkle-light init+animate system: per-orb phase-offset init loop, VU0-scratch mat/vec transform loop into D_00821400, a fade/damping GS update via func_001CE860, and the 2x billboard-particle submit loop with a PRNG-advance ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float);
extern void func_001026A0(void *dst, void *mtx, void *vec);
extern void func_00102948(void *dst, void *src);
extern void func_00103230(void *dst, void *src, float scale);
extern float func_0011E2A8(float);
extern int func_00122BB8(void);
extern void func_001AFC10(unsigned char *e);
extern int func_001CCF70(void *p);
extern void func_001CD520(int mode, int n, void *dst, long long packedConst, float f12, float f13, float f14, int packed);
extern void func_001CE860(int a0, int a1, void *a2, void *a3, float f12, int t0, long long t1);
extern void func_001CFB50(void *p, int a1, void *a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern unsigned char D_00266930[16];
extern unsigned char D_00821400[];
extern float D_700038A0;
extern float D_700038B0;

void func_0021A500(unsigned char *e) {
    unsigned char *s3;
    unsigned char *p;
    unsigned char *tbl;
    unsigned char *p2;
    int st;
    int n;
    int i;
    int color;
    int clip;
    float frac;
    float cur;
    unsigned char sp80[0x30];
    unsigned char spB0[0x10];
    unsigned char spC0[0x10];
    unsigned char spD0[0x10];
    unsigned char spE0[0x10];
    unsigned char spF0[0x10];
    unsigned char sp100[0x60];

    st = e[4];
    s3 = e + 0x1F0;
    if (st != 3 && st != 2) {
        if (st == 1) {
            goto state1;
        }
        if (st != 0) {
            return;
        }
    } else {
        func_001AFC10(e);
        return;
    }

    *(int *)(s3 + 0x18) = 0;
    *(int *)(s3 + 0x48) = 0;
    p = s3 + 4;
    *(int *)(s3 + (*(int *)s3) * 4 + 0x14) = 0;
    *(int *)(s3 + (*(int *)s3) * 4 + 0x44) = 0;
    n = 1;
    while (n < *(int *)s3 - 1) {
        *(float *)(p + 0x18) = 2.5f * func_0011E2A8(6.2831855f * ((float)func_00122BB8() / 2147483648.0f));
        n += 1;
        *(float *)(p + 0x48) = 2.5f * func_0011E2A8(6.2831855f * ((float)func_00122BB8() / 2147483648.0f));
        p += 4;
    }
    *(int *)(s3 + 0xC) = func_00122BB8();
    *(float *)(s3 + 0x10) = 0.0f;
    *(float *)(s3 + 0x14) = 0.0f;
    *(float *)(s3 + 4) = *(float *)(s3 + 4) / (float)(*(int *)s3 - 1);
    e[4] = 1;

state1:
    tbl = D_00821400;
    i = 0;
    p = s3;
    while (i < *(int *)s3) {
        *(float *)0x700038A0 = *(float *)(p + 0x18) * ((0.2f + *(float *)(s3 + 0x10)) * (2.0f * *(float *)(s3 + 8)));
        *(float *)0x700038A4 = *(float *)(p + 0x48) * ((0.2f + *(float *)(s3 + 0x10)) * (2.0f * *(float *)(s3 + 8)));
        *(float *)0x700038A8 = *(float *)(s3 + 0x10) + (*(float *)(s3 + 4) * (float)i);
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(tbl, e + 0xD0, &D_700038A0);
        *(int *)(tbl + 0xC) = 0x3F800000;
        p += 4;
        tbl += 0x10;
        i += 1;
    }
    *(float *)0x700038B0 = 192.0f * (1.0f - *(float *)(s3 + 0x10));
    *(float *)0x700038B4 = 192.0f * (1.0f - *(float *)(s3 + 0x10));
    *(float *)0x700038B8 = 192.0f * (1.0f - *(float *)(s3 + 0x10));
    func_001CE860(0, 2, D_00821400, &D_700038B0, *(float *)(s3 + 8), *(int *)s3, 0x20045D8555422188LL);

    cur = *(float *)(s3 + 0x10);
    cur = cur + ((1.0f - cur) / 8.0f);
    *(float *)(s3 + 0x10) = cur;
    *(float *)(s3 + 0x10) = cur + 0.0001f;

    if (e[5] == 0) {
        if (!(*(float *)(s3 + 0x10) <= 1.0f)) {
            e[4] = 3;
            return;
        }
    }
    if (e[5] == 1) {
        color = *(int *)(s3 + 0xC);
        copy_qw4(sp80, e + 0xD0);
        func_00102948(spB0, D_00821400);
        func_00103230(spC0, e + 0xD0, -1.0f);
        func_00103230(spD0, e + 0xE0, -1.0f);
        func_00103230(spE0, e + 0xF0, -1.0f);
        func_00102948(spF0, D_00821400 + (*(int *)s3 - 1) * 0x10);
        p2 = sp80;
        n = 0;
        do {
            clip = func_001CCF70(p2 + 0x30);
            frac = (float)((color >> 0x10) & 0xFFFF) / 65535.0f;
            color = color * 0x25 + 0xB;
            func_001CFB50(sp100, 0, p2, *(float *)(s3 + 0x14), frac + 0.0001f, 1.0f, 1.0000000116860974e-06f, 5.0f);
            func_001CFBE0(clip, 1, D_00266930, sp100, 0);
            frac = 1.0f - (*(float *)(s3 + 0x14) / 1.1f);
            *(float *)0x70003A20 = frac;
            color = float_to_int(128.0f * frac);
            color |= float_to_int(128.0f * frac) << 8;
            color |= float_to_int(192.0f * frac) << 16;
            func_001CD520(0, 2, p2 + 0x30, 0x20045BA5154222DCLL, 7.0f, 7.0f, 5.0f, color);
            n += 1;
            p2 += 0x40;
        } while (n < 2);
        cur = *(float *)(s3 + 0x14) + 0.04f;
        *(float *)(s3 + 0x14) = cur;
        if (!(cur <= 1.1f)) {
            e[4] = 3;
            return;
        }
    }
}
