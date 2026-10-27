// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 63.66% NEARMISS).
// Fixes read from the original instructions:
//  - the packet setup never writes +0x2C/+0x48/+0x4C/+0x68/+0x6C; +0x28 is one
//    64-bit store of 0x43431.
//  - func_001B0070 takes no arguments (its definition in src/); func_001CB760 takes
//    three; func_001D63B0 takes five (the colour quad is passed twice).
//  - the context pointer D_00275670 is read once into a local per block.
//  - D_0026E810..40 and D_007635C0 are not small-data (absolute addressing); the
//    colour-table select is a 128-bit copy through u128 casts.
//  - the upload loop fills the low record fully, then the high one; locals
//    declared lo, hi before q (stack layout).
//
// Configures GS context 3 (config calls), emits a GIFtag packet with the
// fixed register block, picks one of three colour quads from func_001B0070's
// flags, uploads 224 rows (two per call) via func_001D63B0, then closes the
// chain and sends the packet through func_001CB760.
extern int func_001B0070(void);
extern void func_001CB760(void *, int, void *);
extern void func_001D1F20(int);
extern void func_001D1FF0(int, int);
extern void func_001D2040(int, int);
extern void func_001D63B0(int, void *, void *, void *, void *);
extern void func_001D6B60(int, int, int, int, void *);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D6C90();
typedef struct { int a, b, c, d; } quad;
extern quad D_0026E810;
extern quad D_0026E820;
extern quad D_0026E830;
extern quad D_0026E840;
extern char *D_00275670;
extern int D_0027568C;
extern char D_007635C0[];
typedef int u128 __attribute__((mode(TI)));

void func_001DDB70(void) {
    char *base;
    char *e;
    int r;
    quad lo, hi;
    quad q;
    int i;
    char *ctx;

    base = *(char **)(D_00275670 + 0x1C);
    func_001D6B60(3, D_0027568C, 8, 8, &D_0026E810);
    func_001D6BA0(3, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(3, 3);
    func_001D2040(3, 0);
    func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0,
                  (long long)1, (long long)2, (long long)0, (long long)1, (long long)0, (long long)1, (long long)0);
    ctx = D_00275670;
    *(char *)(*(char **)(ctx + 0x1C) + 3) = 0x10;
    *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
    *(short *)(*(char **)(ctx + 0x1C) + 0) = 7;
    e = *(char **)(ctx + 0x1C);
    *(char **)(ctx + 0x1C) = *(char **)(ctx + 0x1C) + 0x80;
    *(u128 *)(e + 0x10) = 0;
    *(int *)(e + 0x1C) = 0x50000006;
    *(long long *)(e + 0x20) = (long long)(int)0x50AF4000 << 32 | (unsigned)0x8001;
    *(long long *)(e + 0x28) = 0x43431;
    *(int *)(e + 0x30) = 0;
    *(int *)(e + 0x34) = 0xFF;
    *(int *)(e + 0x38) = 0;
    *(int *)(e + 0x3C) = 0xFF;
    *(int *)(e + 0x40) = 8;
    *(int *)(e + 0x44) = 8;
    *(int *)(e + 0x50) = 0x7000;
    *(int *)(e + 0x54) = 0x7900;
    *(int *)(e + 0x58) = -1;
    *(int *)(e + 0x5C) = 0x80;
    *(int *)(e + 0x60) = 0x1008;
    *(int *)(e + 0x64) = 0x1008;
    *(int *)(e + 0x70) = 0x9000;
    *(int *)(e + 0x74) = 0x8700;
    *(int *)(e + 0x78) = -1;
    *(int *)(e + 0x7C) = 0x80;

    r = func_001B0070();
    if (r & 0x80) {
        *(u128 *)&q = *(u128 *)&D_0026E820;
    } else if (r & 0x10000000) {
        *(u128 *)&q = *(u128 *)&D_0026E840;
    } else {
        *(u128 *)&q = *(u128 *)&D_0026E830;
    }

    i = 0;
    do {
        lo.a = 0x7000;
        lo.b = (i + 0x790) * 0x10;
        lo.c = -1;
        lo.d = 0x80;
        hi.a = 0x9000;
        hi.b = (i + 0x790) * 0x10;
        hi.c = -1;
        hi.d = 0x80;
        func_001D63B0(3, &lo, &q, &hi, &q);
        i += 2;
    } while (i < 0xE0);

    func_001D1F20(3);
    ctx = D_00275670;
    *(char *)(*(char **)(ctx + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
    *(short *)(*(char **)(ctx + 0x1C) + 0) = 0;
    *(char **)(ctx + 0x1C) = *(char **)(ctx + 0x1C) + 0x10;
    func_001CB760(D_007635C0, 0xFFF000, base);
}
