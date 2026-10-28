// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.71% NEARMISS).
// Fixes read from the original instructions:
//  - Statement order in the outer-loop preamble: the quadword copy comes first,
//    then the row angle, then the inner counter reset (j = 0, hoisted out of the
//    for header), then the two row pointers. The "FP coloring" residual was only
//    this order.
//
// Builds two 16x16 grids of transforms (bases D_0081C0F0 and D_0081D0F0, each
// row stride 0x100, each cell 0x10). For each (i,j) in [0,16): seeds a 0x40-byte
// workspace `sp90` via func_001029C0, rotates it about one axis by
// (PI/2 * i)/15 (func_00102A60) and about another by ((2*PI * j)/15) - PI
// (func_00102BB0), then func_001026A0 composes it with the per-cell base and the
// constant quadword D_00253520 (copied into `sp80`) into both grids.
typedef struct { int a, b, c, d; } qw;
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001029C0(void *p);
extern void func_00102A60(void *a, void *b, float f);
extern void func_00102BB0(void *a, void *b, float f);
extern qw D_00253520;
extern char D_0081C0F0;
extern char D_0081D0F0;

void func_001E0380(void) {
    qw sp80;
    char sp90[0x40];
    int i;
    int j;
    char *s2;
    char *s3;
    char *s4;
    char *s5;

    s4 = &D_0081C0F0;
    s5 = &D_0081D0F0;
    for (i = 0; i < 0x10; i++) {
        float ang;
        sp80 = D_00253520;
        ang = (1.5707964f * (float)i) / 15.0f;
        j = 0;
        s2 = s4;
        s3 = s5;
        for (; j < 0x10; j++) {
            func_001029C0(sp90);
            func_00102A60(sp90, sp90, ang);
            func_00102BB0(sp90, sp90, ((6.2831855f * (float)j) / 15.0f) - 3.1415927f);
            func_001026A0(s2, sp90, &sp80);
            func_001026A0(s3, sp90, &sp80);
            s2 += 0x10;
            s3 += 0x10;
        }
        s4 += 0x100;
        s5 += 0x100;
    }
}
