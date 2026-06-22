// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Iterates 8 times over the control-table at D_0026EAC0 (stride 4 = one int
// per step). Seeds two stack quaternions/vectors: sp50 from arg0+0xD0 via
// func_00102900, and sp30 (the running accumulator) from arg0+0x100 via
// func_00102948. Each iteration: func_001028B8(&sp40, &sp30, &sp50) produces
// an interpolated/blended value into sp40, then func_001CD940(2, &sp30,
// &D_0026EA80 + tbl[0]*0x10, &sp40, &D_0026EA80 + tbl[1]*0x10) consumes a pair
// of 0x10-byte records from the D_0026EA80 table (indexed by the two ints at
// the current D_0026EAC0 slot), and func_00102948 copies sp40 back into sp30
// for the next step.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build scores
// 90.34%. Two idioms were load-bearing: the loop counter is `unsigned int`
// (yields `sltiu`, not the signed `slti` 991202/wrong-type produces), and the
// local declaration order (sp30, sp40, sp50, then i, p) fixes mwcc's stack-slot
// assignment so the first buffer lands at sp+0x50 and the accumulator at
// sp+0x30, and pins the loop pointer to a0 / counter to v1. Verified TRUE
// objdiff 100.0 byte-identical vs build/expected/func_001F4F90.o.
struct vec { float x, y, z, w; };
extern void func_00102900(struct vec *dst, void *src);
extern void func_00102948(struct vec *dst, struct vec *src);
extern void func_001028B8(struct vec *a, struct vec *b, struct vec *c);
extern void func_001CD940(int a0, struct vec *a1, void *a2, struct vec *a3, void *a4);
extern unsigned char D_0026EA80;
extern int D_0026EAC0[];

void func_001F4F90(char *arg0) {
    struct vec sp30;
    struct vec sp40;
    struct vec sp50;
    unsigned int i;
    int *p;

    func_00102900(&sp50, arg0 + 0xD0);
    func_00102948(&sp30, (struct vec *)(arg0 + 0x100));
    p = D_0026EAC0;
    i = 0;
    do {
        func_001028B8(&sp40, &sp30, &sp50);
        func_001CD940(2, &sp30,
                      &D_0026EA80 + p[0] * 0x10,
                      &sp40,
                      &D_0026EA80 + p[1] * 0x10);
        func_00102948(&sp30, &sp40);
        i += 1;
        p += 1;
    } while (i < 8);
}
