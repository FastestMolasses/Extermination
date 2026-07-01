// NEARMISS func_0021AAC0  (vram 0x0021AAC0, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Saved-register allocation ORDER permutation: mwcc233 colors arg0->0x24 field and `e`(=arg0+0x1F0) to s0/s1 while the target colors them s1/s2, and this single leading swap cascades ARG_MISMATCH (register-only, no opcode/immediate differences) through nearly the whole 964-byte function including t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void copy_qw4(char *, char *);
extern void func_001026A0(char *, char *, void *);
extern void func_00102918(void *, void *, char *);
extern void func_001028B8(char *, char *, char *);
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern int func_00122BB8(void);
extern void func_001AFC10(char *);
extern int func_001CCF70(void *);
extern void func_001CFA60(void *, void *, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern char *func_001EFEB0(int, void *);

extern int D_700036A0;
extern int D_700036D0;
extern int D_002669C0;
extern int D_00266A50;
extern int D_700038A0;

void func_0021AAC0(char *arg0) {
    char *s1 = *(char **)(arg0 + 0x24);
    unsigned char st = *(unsigned char *)(arg0 + 4);
    char *e = arg0 + 0x1F0;
    int i;
    char *p;
    int cnt;
    int idx;
    int n;
    char *q;
    char *r;
    char sp80[0x60];

    switch (st) {
    default:
        if (st != 0) {
            return;
        }
        i = 0;
        p = e;
        do {
            i += 1;
            *(float *)(p + 0x78) = (float) func_00122BB8() / 2.1474836e9f;
            *(int *)(p + 0x60) = 0;
            p += 4;
        } while (i < 6);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x3F800000;
        *(int *)0x700038AC = 0x3F800000;
        copy_qw4(arg0 + 0xD0, s1 + 0xD0);
        func_001026A0(arg0 + 0xB0, arg0 + 0xD0, &D_700038A0);
        *(int *)(e + 0x98) = 0;
        *(int *)(e + 0x94) = 0;
        *(float *)(e + 0x90) = 0.0f;
        *(unsigned char *)(arg0 + 4) = 1;
    case 1:
        *(int *)(e + 0x98) = *(int *)(e + 0x98) + 1;
        cnt = *(int *)(e + 0x98);
        if (cnt >= 0x1F) {
            if ((cnt % 6) == 1) {
                if (*(int *)(e + 0x94) < 6) {
                    *(int *)(e + *(int *)(e + 0x94) * 0x10) = 0;
                    *(float *)(e + *(int *)(e + 0x94) * 0x10 + 4) = *(float *)(e + 0x90);
                    *(int *)(e + *(int *)(e + 0x94) * 0x10 + 8) = 0;
                    *(int *)(e + *(int *)(e + 0x94) * 0x10 + 0xC) = 0x3F800000;
                    q = e + *(int *)(e + 0x94) * 0x10;
                    func_001028B8(q, q, arg0 + 0xB0);
                    *(float *)(e + 0x90) = *(float *)(e + 0x90) - 8.0f;
                    *(int *)(e + 0x94) = *(int *)(e + 0x94) + 1;
                }
            }
            n = 0;
            p = e;
            r = e;
            while (n < *(int *)(e + 0x94)) {
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, 1.5707964f);
                func_00102918(&D_700036A0, &D_700036A0, p);
                idx = func_001CCF70(&D_700036D0);
                func_001CFA60(sp80, &D_700036A0, *(float *)(r + 0x60), *(float *)(r + 0x78));
                func_001CFBE0(idx, 2, &D_002669C0, sp80, 0);
                func_001CFBE0(idx, 1, &D_00266A50, sp80, 0);
                p += 0x10;
                n += 1;
                *(float *)(r + 0x60) = *(float *)(r + 0x60) + 0.02f;
                r += 4;
            }
        }
        if ((*(int *)(e + 0x98) < 0x3C) && ((*(int *)(e + 0x98) % 10) == 1)) {
            func_001029C0(&D_700036A0);
            func_00102B08(&D_700036A0, &D_700036A0, 1.5707964f);
            func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
            q = func_001EFEB0(0x8000003B, &D_700036A0);
            if (q != 0) {
                *(char *)(q + 5) = 0;
                *(int *)(q + 0x1F0) = 0xC;
                *(int *)(q + 0x1F4) = 0x42400000;
                *(int *)(q + 0x1F8) = 0x3F000000;
            }
        }
        if (*(int *)(e + 0x98) == 0x3C) {
            *(char *)(s1 + 4) = 3;
        }
        if (*(int *)(e + 0x98) >= 0x79) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        return;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
