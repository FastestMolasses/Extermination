// NEARMISS func_0021AAC0  (vram 0x0021AAC0, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was 93.44%; matching wave 2026-10-27: case 0 is an explicit `case 0:` with case 1 ending in `break`, the emitter block at arg0+0x1F0 is a struct (6 vec4 slots, two float[6] tables, fade value, count, timer), `> 0x1E` / `> 0x78` compares, compound `+= 0.02f`, locals declared counter/index first, loop-2 increments ordered n, field, p, r). The LOGIC and STRUCTURE are faithful; the residual
// diff is register colouring only:
// Remaining differences: in the per-slot draw loop the slot counter and the vec4 slot pointer swap saved registers s3/s5 (6 instructions, register-only); declaration-order sweeps, for/while spellings, typed pointers, merging the two counters and a 540 s 2.3.3 permuter run did not fix it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void copy_qw4(char *, char *);
extern void func_001026A0(char *, char *, void *);
extern void func_00102918(void *, void *, void *);
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

typedef struct { float x, y, z, w; } V4;
typedef struct {
    V4 v[6];
    float a[6];
    float b[6];
    float f90;
    int n;
    int t;
} Emit;

void func_0021AAC0(char *arg0) {
    int i;
    int idx;
    char *s1;
    Emit *e;
    char *p;
    int n;
    char *r;
    unsigned char st;
    int cnt;
    char *q;
    char sp80[0x60];

    s1 = *(char **)(arg0 + 0x24);
    st = *(unsigned char *)(arg0 + 4);
    e = (Emit *)(arg0 + 0x1F0);

    switch (st) {
    case 0:
        i = 0;
        p = (char *)e;
        do {
            *(float *)(p + 0x78) = (float) func_00122BB8() / 2.1474836e9f;
            *(int *)(p + 0x60) = 0;
            i += 1;
            p += 4;
        } while (i < 6);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x3F800000;
        *(int *)0x700038AC = 0x3F800000;
        copy_qw4(arg0 + 0xD0, s1 + 0xD0);
        func_001026A0(arg0 + 0xB0, arg0 + 0xD0, &D_700038A0);
        e->t = 0;
        e->n = 0;
        e->f90 = 0.0f;
        *(unsigned char *)(arg0 + 4) = 1;
    case 1:
        e->t = e->t + 1;
        cnt = e->t;
        if (cnt > 0x1E) {
            if ((cnt % 6) == 1) {
                if (e->n < 6) {
                    e->v[e->n].x = 0.0f;
                    e->v[e->n].y = e->f90;
                    e->v[e->n].z = 0.0f;
                    e->v[e->n].w = 1.0f;
                    q = (char *)&e->v[e->n];
                    func_001028B8(q, q, arg0 + 0xB0);
                    e->f90 = e->f90 - 8.0f;
                    e->n = e->n + 1;
                }
            }
            n = 0;
            p = (char *)e;
            r = (char *)e;
            while (n < e->n) {
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, 1.5707964f);
                func_00102918(&D_700036A0, &D_700036A0, p);
                idx = func_001CCF70(&D_700036D0);
                func_001CFA60(sp80, &D_700036A0, *(float *)(r + 0x60), *(float *)(r + 0x78));
                func_001CFBE0(idx, 2, &D_002669C0, sp80, 0);
                func_001CFBE0(idx, 1, &D_00266A50, sp80, 0);
                n += 1;
                *(float *)(r + 0x60) += 0.02f;
                p += 0x10;
                r += 4;
            }
        }
        if ((e->t < 0x3C) && ((e->t % 10) == 1)) {
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
        if (e->t == 0x3C) {
            *(char *)(s1 + 4) = 3;
        }
        if (e->t > 0x78) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
