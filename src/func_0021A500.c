// NEARMISS func_0021A500  (vram 0x0021A500, 0x5BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was 89.22%).
// Matching wave 2026-10-28 fixes read from the original instructions:
//  - FIDELITY: the billboard loop's PRNG seed and the packed draw colour are two
//    variables; the old C overwrote the seed with the colour, so the second particle's
//    random fraction came from the colour instead of the advanced seed.
//  - the state dispatch is `switch (e[4])` (case 0 falls into case 1; cases 2 and 3
//    share the func_001AFC10 hand-off placed last).
//  - the init loop's counter increments after both random draws; the eased value
//    is a separate temporary; the random fraction includes its +0.0001 before the
//    call; the +0.04 step is a compound add on the field; local declaration order.
// Remaining differences: the state-1 loop's pointer and counter take each other's
// saved registers (s1/s4), the per-orb end-slot index adds base and offset in the
// other operand order, one load is scheduled one slot later, and the 1 - t/1.1 fade
// is computed into its saved float register directly instead of being copied there
// after the scratchpad store.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Twinkle-light effect, state machine on e[4]: state 0 seeds per-orb random phase
// offsets and falls into state 1, which transforms the orb chain into D_00821400 through
// the scratchpad vector, submits it with a fade via func_001CE860, eases the fade toward
// 1 and, when e[5] == 1, draws two billboard particles (random size from a 37x+11 PRNG,
// colour scaled by 1 - t/1.1); past the limits it moves to state 3. States 2 and 3
// hand off to func_001AFC10.

extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float);
extern void func_001026A0(void *dst, void *mtx, void *vec);
extern void func_00102948(void *dst, void *src);
extern void func_00103230(void *dst, void *src, float scale);
extern float func_0011E2A8(float);
extern int func_00122BB8(void);
extern void func_001AFC10(unsigned char *e);
extern int func_001CCF70(void *p);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);
extern void func_001CE860(int a0, int a1, void *a2, void *a3, float f12, int t0, long long t1);
extern void func_001CFB50(void *p, int a1, void *a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern unsigned char D_00266930[16];
extern unsigned char D_00821400[];
extern float D_700038A0;
extern float D_700038B0;

void func_0021A500(unsigned char *e) {
    int clip;
    int seed;
    unsigned char *s3;
    unsigned char *p2;
    int n;
    unsigned char *p;
    int i;
    unsigned char *tbl;
    int st;
    int color;
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
    switch (st) {
    case 0:
        *(int *)(s3 + 0x18) = 0;
        *(int *)(s3 + 0x48) = 0;
        p = s3 + 4;
        *(int *)(s3 + (*(int *)s3) * 4 + 0x14) = 0;
        *(int *)(s3 + (*(int *)s3) * 4 + 0x44) = 0;
        n = 1;
        while (n < *(int *)s3 - 1) {
            *(float *)(p + 0x18) = 2.5f * func_0011E2A8(6.2831855f * ((float)func_00122BB8() / 2147483648.0f));
            *(float *)(p + 0x48) = 2.5f * func_0011E2A8(6.2831855f * ((float)func_00122BB8() / 2147483648.0f));
            n += 1;
            p += 4;
        }
        *(int *)(s3 + 0xC) = func_00122BB8();
        *(float *)(s3 + 0x10) = 0.0f;
        *(float *)(s3 + 0x14) = 0.0f;
        *(float *)(s3 + 4) = *(float *)(s3 + 4) / (float)(*(int *)s3 - 1);
        e[4] = 1;

    case 1:
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
        frac = cur + ((1.0f - cur) / 8.0f);
        *(float *)(s3 + 0x10) = frac;
        *(float *)(s3 + 0x10) = frac + 0.0001f;

        if (e[5] == 0) {
            if (!(*(float *)(s3 + 0x10) <= 1.0f)) {
                e[4] = 3;
                return;
            }
        }
        if (e[5] == 1) {
            seed = *(int *)(s3 + 0xC);
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
                frac = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f;
                seed = seed * 0x25 + 0xB;
                func_001CFB50(sp100, 0, p2, *(float *)(s3 + 0x14), frac, 1.0f, 1.0000000116860974e-06f, 5.0f);
                func_001CFBE0(clip, 1, D_00266930, sp100, 0);
                frac = 1.0f - (*(float *)(s3 + 0x14) / 1.1f);
                *(float *)0x70003A20 = frac;
                color = float_to_int(128.0f * frac);
                color |= float_to_int(128.0f * frac) << 8;
                color |= float_to_int(192.0f * frac) << 16;
                func_001CD520(0, 2, p2 + 0x30, 0x20045BA5154222DCLL, color, 7.0f, 7.0f, 5.0f);
                n += 1;
                p2 += 0x40;
            } while (n < 2);
            *(float *)(s3 + 0x14) += 0.04f;
            if (!(*(float *)(s3 + 0x14) <= 1.1f)) {
                e[4] = 3;
                return;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(e);
        break;
    }
}
