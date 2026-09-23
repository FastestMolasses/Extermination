// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 1
//
// MATCH NOTE (m1-firstlevel-matching lane):
// - func_00102948 is a two-argument quadword copy (lq from a1, sq to a0, at
//   0x00102948). The former K&R declaration let one call carry a spurious third
//   argument and changed argument evaluation order. The prototype is now exact.
// - Only the byte global D_00275BE0 is gp-relative in the original. The file uses
//   -sdatathreshold 1 and over-declares the other byte globals as arrays
//   (idiom #20) so they stay absolute.
// - The copy sources are spelled (float *)base + 4 / + 8, which gives the
//   original argument order around the jal.

extern void func_001B0250(void *a, int b);
extern void func_001B0B50(void);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a);
extern void func_00102C58(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001B0080(void *a, float b);
extern void func_0018C0D0(void *a, int b);
extern void func_001DD980(float *a, float *b);

extern unsigned char D_00810700[16];
extern unsigned char D_00810701[16];
extern unsigned char D_00810702[16];
extern unsigned char D_008101E0[16];
extern signed char D_008101E1[16];
extern signed char D_008101E2[16];
extern signed char D_008101E3[16];
extern unsigned char D_008101E7[16];
extern short D_008101E8;
extern int D_008104E0;
extern signed char D_008106BE[16];
extern int D_008106C8;
extern signed char D_008106CD[16];
extern unsigned char D_00275BE0;
extern float D_008101EC;
extern float D_00810244;
extern float D_00810350;
extern float D_00810370;
extern float D_008105D0;
extern float D_008105E0;
extern int **D_0024D650[];
extern float D_0024A8D0;
extern float D_70003400;
extern float D_70003600;

void func_001B0460(int arg0) {
    unsigned char *p;
    unsigned char *base;
    int v1;
    float f;
    float *m;

    base = D_008101E0;
    p = (unsigned char *)((int *)D_0024D650[D_00810700[0]][D_00810701[0]]);
    p += D_00810702[0] * 0x30;
    func_001B0250((void *)D_0024D650[D_00810700[0]], D_00810701[0] * 4);
    func_001B0B50();
    D_008101E8 = 0;
    D_008101E7[0] = 0;
    D_008106CD[0] = (signed char)(D_008106C8 >> 16);
    f = *(float *)(p + 0x18);
    D_00810244 = f;
    D_008101EC = f;
    D_008101E1[0] = 0;
    D_008101E2[0] = 0;
    D_008101E3[0] = 0;
    v1 = *(unsigned char *)(p + 0x10);
    if (v1 & 0x80) {
        *(base + 5) = 1;
    } else {
        *(base + 5) = 0;
    }
    *(base + 6) = (unsigned char)(v1 & 0x7F);
    if (D_00275BE0 == 1) {
        *(base + 6) = 0;
        D_00275BE0 = 0;
    }
    if (*(base + 5) == 1) {
        if (arg0 != 0) {
            if (D_008104E0 != 0x10) {
                if (D_008104E0 != 0x12) {
                    goto block_14;
                }
            }
            if (D_008104E0 == 0x10) {
                *(base + 6) = 9;
            } else {
                *(base + 6) = 0xB;
            }
            *(base + 1) = 0;
        } else {
        block_14:
            m = (float *)((char *)&D_0024A8D0 + (*(int *)(p + 0x10) >> 8) * 0xC);
            *(float *)(base + 0x10) = m[0];
            *(float *)(base + 0x14) = m[1];
            *(float *)(base + 0x18) = m[2];
            *(int *)(base + 0x1C) = 0x3F800000;
            func_00102948((float *)(base + 0x20), &D_00810350);
            {
                float c = 15.0f;
                *(float *)(base + 0x24) = *(float *)(base + 0x24) + c;
            }
            func_00102948(&D_008105E0, (float *)base + 8);
            func_00102948(&D_008105D0, (float *)base + 4);
        }
    } else if (*(base + 6) == 0xA) {
        D_008106BE[0] = 2;
        func_00102948((float *)(base + 0x10), &D_00810350);
        {
            float c = 3.0f;
            *(float *)(base + 0x14) = *(float *)(base + 0x14) + c;
        }
        func_001029C0(&D_70003400);
        func_00102C58(&D_70003400, &D_70003400, &D_00810370);
        *(volatile int *)0x70003600 = 0;
        *(volatile int *)0x70003604 = 0;
        *(volatile int *)0x70003608 = 0x40A00000;
        *(volatile int *)0x7000360C = 0;
        func_001026A0((float *)(base + 0x20), &D_70003400, &D_70003600);
        func_001028B8((float *)(base + 0x20), (float *)(base + 0x20), (float *)(base + 0x10));
        func_00102948(&D_008105E0, (float *)base + 8);
        func_00102948(&D_008105D0, (float *)base + 4);
    } else {
        func_001B0080(base, 2.0f);
        v1 = *(int *)(p + 0x14);
        if (v1 == 5) {
            *(base + 6) = 0xD;
        } else if (v1 == 4 && D_00810700[0] == 0x13) {
            *(base + 6) = 0xF;
        }
    }
    func_0018C0D0(base, 1);
    func_001DD980(&D_008105D0, &D_008105E0);
}
