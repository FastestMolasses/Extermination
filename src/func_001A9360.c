// NEARMISS func_001A9360  (vram 0x001A9360, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + load-scheduling permutation around the v/sq compute: target loads the 0x70003A20 threshold into $f12 early and colors v/sq in $f0/$f1; mwcc uses a different FP coloring/load order. Forcing the early read via a temp lowered the score, so the 92.45% form is kept. Body, absolu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 92.45% (mwcc 2.3.3; 991202 reaches 83.46%). Logic fully recovered.
// Overlay-arena physics step using the scratch globals at 0x700038A0/B0/AC and
// 0x70003A20/A24/B88. Seeds the scratch matrices: func_00183C40(arg1,&D_700038B0),
// func_001028D0(&D_700038A0,&D_700038B0, arg0+0xB0), clears the 0x700038AC flag,
// and stores func_00102738(&D_700038A0,&D_700038A0) at 0x70003A20. Computes
// v = *(*(arg0+0x30)) + *(*(arg1+0x30)) and sq = v*v, stashing v then sq at
// 0x70003A24. If the threshold at 0x70003A20 <= sq, runs the response path:
// func_001A91C0(arg0, arg1, func_0011E748()), re-seeds the scratch via
// func_001028D0, sets 0x700038AC = 1.0f, func_00102760(arg1+0x70, &D_700038A0),
// and clears the short at 0x70003B88.
//
// WALL: sole residual is FP register coloring + load scheduling around the
// v/sq computation -- the target loads the 0x70003A20 threshold into $f12 early
// and colors v/sq in $f0/$f1; mwcc picks a different FP coloring and load order.
// Forcing the early threshold read with a temp lowered the score (90.1%), so the
// 92.45% form is kept. Body, the absolute-address scratch globals, the symbol
// relocs for D_700038A0/B0, and the call sequence all match. Permuter class.
extern void func_00183C40(char *p, void *m);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern float func_0011E748(void);
extern void func_001A91C0(char *p, char *q, float f);
extern void func_00102760(void *a, void *b);
extern char D_700038A0[];
extern char D_700038B0[];

void func_001A9360(char *arg0, char *arg1) {
    float *base;
    float v;
    float sq;

    func_00183C40(arg1, &D_700038B0);
    func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
    *(int *)0x700038AC = 0;
    *(float *)0x70003A20 = func_00102738(&D_700038A0, &D_700038A0);
    base = *(float **)(arg0 + 0x30);
    v = *base + **(float **)(arg1 + 0x30);
    sq = v * v;
    *(float *)0x70003A24 = v;
    *(float *)0x70003A24 = sq;
    if (*(float *)0x70003A20 <= sq) {
        func_001A91C0(arg0, arg1, func_0011E748());
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0x3F800000;
        func_00102760(arg1 + 0x70, &D_700038A0);
        *(short *)0x70003B88 = 0;
    }
}
