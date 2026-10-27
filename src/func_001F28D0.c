// NEARMISS func_001F28D0  (vram 0x001F28D0, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a scheduling order that no tried source change fixes:
// Remaining differences: at the func_001CFAE0 call the original schedules the upper half of
// D_70003A20 (the f13 argument) ahead of the 1.0f constant for f14; we emit the 1.0f first
// (3 instructions swapped). Permuter (8 min, mwcc 2.3.3) found nothing.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Improved 2026-10-27 (82.98% -> 98.80%), fixes read from the original instructions:
//  - func_001CFBE0 takes five arguments (its definition in src/): a trailing 0 copy flag.
//  - State 0 tracks the max duration with an unconditional store (select, not a guarded store),
//    and walks the matrix slots with a pointer (m += 0x10), not an index.
//  - The 0xA0 latch is bumped only when slot 0's age is NOT <= 0.5 (the old C had it inverted).
//  - Separate state-1 locals (rng, cnt, j, age2, mat) and the declaration order give the
//    original saved-register assignment; the spawn value is staged through f (convert,
//    divide, add 1e-4) and `+= 0.025f` ages the slot.
//  - The scratchpad float 0x70003A20 is the relocated extern D_70003A20 (idiom-32).
//
// 4-state particle-ring tick keyed on p[4] (arg0=p; scratch block at p+0x1F0). State 0
// initializes 8 ring slots' age floats (-0.02*i) and their matrices from a table of matrix
// pointers indexed by D_0025A330, tracks the max duration via func_001CCF70, then seeds an
// RNG (func_00122BB8) and advances to state 1. State 1 runs an 8-slot particle emission
// loop: derives a pseudo-random spawn value from the RNG high word, and for slots with
// age < 1.7 builds a scratch matrix (func_001029C0 / func_00102918) and emits two
// particles (func_001CFAE0 + func_001CFBE0 into D_0025A210/D_0025A2A0), ageing the slot
// by 0.025; if no particles were emitted this tick, advances to state 3. States 2/3 both
// call func_001AFC10(p) (teardown).
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a);
extern int func_00122BB8(void);
extern int func_001AFC10(void *p);
extern int func_001CCF70(void *a0);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFBE0(int handle, unsigned int n, void *p, void *src, int copy);

extern int D_0025A330[8];
extern char D_0025A210[];
extern char D_0025A2A0[];
extern char D_700036A0[];
extern float D_70003A20;

void func_001F28D0(unsigned char *p) {
    char sp80[0x60];
    int i;
    int rng;
    int *tbl;
    int cnt;
    unsigned char *base;
    unsigned char *age;
    unsigned char *age2;
    int *ip;
    unsigned char *mat;
    unsigned char *m;
    int j;
    float f;
    int t;

    tbl = *(int **)(p + 0x24);
    base = p + 0x1F0;
    switch (p[4]) {
    case 0:
        *(int *)(base + 0xA8) = 0;
        i = 0;
        age = base;
        ip = D_0025A330;
        m = base;
        do {
            *(float *)(age + 0x80) = -0.02f * (float) i;
            func_00102948(m, (void *)(*(int *)((char *)tbl + *ip * 4 + 0x110) + 0xC0));
            t = func_001CCF70(m);
            *(int *)(base + 0xA8) = (*(int *)(base + 0xA8) < t) ? t : *(int *)(base + 0xA8);
            i = i + 1;
            age += 4;
            ip += 1;
            m += 0x10;
        } while (i < 8);
        *(int *)(base + 0xA8) = *(int *)(base + 0xA8) + 8;
        *(int *)(base + 0xA0) = 0;
        *(int *)(base + 0xA4) = func_00122BB8();
        p[4] = 1;
        /* fallthrough */
    case 1:
        rng = *(int *)(base + 0xA4);
        cnt = 0;
        j = 0;
        age2 = base;
        mat = base;
        do {
            f = (float) ((rng >> 16) & 0xFFFF);
            f = f / 65535.0f;
            f += 0.0001f;
            D_70003A20 = f;
            rng = rng * 0x25 + 0xB;
            if (*(float *)(age2 + 0x80) < 1.7f) {
                func_001029C0(D_700036A0);
                func_00102918(D_700036A0, D_700036A0, mat);
                func_001CFAE0(sp80, 0, D_700036A0, *(float *)(age2 + 0x80),
                              D_70003A20, 1.0f, 9.999999974752427e-07f);
                func_001CFBE0(*(int *)(base + 0xA8), 1, D_0025A210, sp80, 0);
                func_001CFBE0(*(int *)(base + 0xA8), 1, D_0025A2A0, sp80, 0);
                cnt = cnt + 1;
                *(float *)(age2 + 0x80) += 0.025f;
            }
            j = j + 1;
            age2 += 4;
            mat += 0x10;
        } while (j < 8);
        if (!(*(float *)(base + 0x80) <= 0.5f)) {
            if (*(int *)(base + 0xA0) == 0) {
                *(int *)(base + 0xA0) = *(int *)(base + 0xA0) + 1;
            }
        }
        if (cnt == 0) {
            p[4] = 3;
        }
        return;
    case 2:
    case 3:
        func_001AFC10(p);
        return;
    }
}
