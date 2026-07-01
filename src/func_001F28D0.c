// NEARMISS func_001F28D0  (vram 0x001F28D0, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation across the two 8-iteration loops (same class as sibling func_001F8350, which caps at 92.7% for the identical reason). mwcc-233 needs one extra saved register (s0-s7 vs target's s0-s6), cascading into different physical coloring throughout; logic/structure fully rec...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (mwcc 2.3.3, 82.98% objdiff; 991202 81.98%). 4-state particle-ring
// tick keyed on p[4] (arg0=p; scratch block at p+0x1F0). State 0 initializes 8
// ring slots' age floats (-0.02*i) from a table of matrix pointers indexed by
// D_0025A330, tracks the max duration via func_001CCF70, then seeds an RNG
// (func_00122BB8) and advances to state 1. State 1 runs an 8-slot particle
// emission loop: derives a pseudo-random spawn value from the RNG high word,
// and for slots with age < 1.7 builds a scratch matrix (func_001029C0 /
// func_00102918) and emits two particles (func_001CFAE0 + func_001CFBE0 into
// D_0025A210/D_0025A2A0), ageing the slot by 0.025; if no particles were
// emitted this tick, advances to state 3. States 2/3 both call
// func_001AFC10(p) (teardown).
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a);
extern int func_00122BB8(void);
extern int func_001AFC10(void *p);
extern int func_001CCF70(void *a0);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFBE0(int handle, int n, void *p, void *src);

extern int D_0025A330[8];
extern char D_0025A210[];
extern char D_0025A2A0[];
extern char D_700036A0[];

void func_001F28D0(unsigned char *p) {
    char sp80[0x60];
    int s2;
    unsigned char *s3;
    unsigned char *s4;
    int *s5;
    int s1;
    int s6;
    float f;
    int t;
    int *tbl;

    tbl = *(int **)(p + 0x24);
    s3 = p + 0x1F0;
    switch (p[4]) {
    case 0:
        *(int *)(s3 + 0xA8) = 0;
        s1 = 0;
        s4 = s3;
        s5 = D_0025A330;
        s6 = 0;
        do {
            *(float *)(s4 + 0x80) = -0.02f * (float) s1;
            func_00102948(s3 + (s6 << 4), (void *)(*(int *)((char *)tbl + *s5 * 4 + 0x110) + 0xC0));
            t = func_001CCF70(s3 + (s6 << 4));
            if (*(int *)(s3 + 0xA8) < t) {
                *(int *)(s3 + 0xA8) = t;
            }
            s1 = s1 + 1;
            s4 += 4;
            s5 += 1;
            s6 += 1;
        } while (s1 < 8);
        *(int *)(s3 + 0xA8) = *(int *)(s3 + 0xA8) + 8;
        *(int *)(s3 + 0xA0) = 0;
        *(int *)(s3 + 0xA4) = func_00122BB8();
        p[4] = 1;
        /* fallthrough */
    case 1:
        s1 = *(int *)(s3 + 0xA4);
        s2 = 0;
        s6 = 0;
        s4 = s3;
        s5 = (int *)s3;
        do {
            f = (float) ((s1 >> 16) & 0xFFFF) / 65535.0f;
            s1 = s1 * 0x25 + 0xB;
            *(float *)0x70003A20 = f + 0.0001f;
            if (*(float *)(s4 + 0x80) < 1.7f) {
                func_001029C0(D_700036A0);
                func_00102918(D_700036A0, D_700036A0, s5);
                func_001CFAE0(sp80, 0, D_700036A0, *(float *)(s4 + 0x80),
                              *(float *)0x70003A20, 1.0f, 9.999999974752427e-07f);
                func_001CFBE0(*(int *)(s3 + 0xA8), 1, D_0025A210, sp80);
                func_001CFBE0(*(int *)(s3 + 0xA8), 1, D_0025A2A0, sp80);
                s2 = s2 + 1;
                *(float *)(s4 + 0x80) = *(float *)(s4 + 0x80) + 0.025f;
            }
            s6 = s6 + 1;
            s4 += 4;
            s5 = (int *)((char *)s5 + 0x10);
        } while (s6 < 8);
        if (*(float *)(s3 + 0x80) <= 0.5f) {
            if (*(int *)(s3 + 0xA0) == 0) {
                *(int *)(s3 + 0xA0) = *(int *)(s3 + 0xA0) + 1;
            }
        }
        if (s2 == 0) {
            p[4] = 3;
        }
        return;
    case 2:
    case 3:
        func_001AFC10(p);
        return;
    }
}
