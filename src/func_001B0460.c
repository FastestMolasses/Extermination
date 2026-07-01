// NEARMISS func_001B0460  (vram 0x001B0460, 0x358 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two isolated residuals after extensive iteration (ternary->if/else restructure fixed the var_v0 branch-constant scheduling entirely; scoped `float c = <const>;` idiom, borrowed from the 100%-matching sibling func_001B0300.c, fixed both add.s operand-order mismatches): (1) D_00275BE0 is addressed ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B0250(void *a, int b);
extern void func_001B0B50(void);
extern void func_00102948();
extern void func_001029C0(void *a);
extern void func_00102C58(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001B0080(void *a, float b);
extern void func_0018C0D0(void *a, int b);
extern void func_001DD980(float *a, float *b);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_008101E0;
extern signed char D_008101E1;
extern signed char D_008101E2;
extern signed char D_008101E3;
extern unsigned char D_008101E7;
extern short D_008101E8;
extern int D_008104E0;
extern signed char D_008106BE;
extern int D_008106C8;
extern signed char D_008106CD;
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

    base = &D_008101E0;
    p = (unsigned char *)((int *)D_0024D650[D_00810700][D_00810701]);
    p += D_00810702 * 0x30;
    func_001B0250((void *)D_0024D650[D_00810700], D_00810701 * 4);
    func_001B0B50();
    D_008101E8 = 0;
    D_008101E7 = 0;
    D_008106CD = (signed char)(D_008106C8 >> 16);
    f = *(float *)(p + 0x18);
    D_00810244 = f;
    D_008101EC = f;
    D_008101E1 = 0;
    D_008101E2 = 0;
    D_008101E3 = 0;
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
            func_00102948((float *)(base + 0x20), &D_00810350, m);
            {
                float c = 15.0f;
                *(float *)(base + 0x24) = *(float *)(base + 0x24) + c;
            }
            func_00102948(&D_008105E0, (float *)(base + 0x20));
            func_00102948(&D_008105D0, (float *)(base + 0x10));
        }
    } else if (*(base + 6) == 0xA) {
        D_008106BE = 2;
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
        func_00102948(&D_008105E0, (float *)(base + 0x20));
        func_00102948(&D_008105D0, (float *)(base + 0x10));
    } else {
        func_001B0080(base, 2.0f);
        v1 = *(int *)(p + 0x14);
        if (v1 == 5) {
            *(base + 6) = 0xD;
        } else if (v1 == 4 && D_00810700 == 0x13) {
            *(base + 6) = 0xF;
        }
    }
    func_0018C0D0(base, 1);
    func_001DD980(&D_008105D0, &D_008105E0);
}
