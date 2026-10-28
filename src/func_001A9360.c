// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.45% NEARMISS).
// Fixes read from the original instructions:
// - func_0011E748 is the float square root and takes one float argument (its own
//   definition in src/); the response path passes it the squared length stored at
//   0x70003A20, which is why that value was loaded early into the first FP argument.
// - The scratch words at 0x70003A20 / 0x70003A24 are relocated extern floats
//   (idiom-32); the sum is stored at 0x70003A24, then squared in place from that word,
//   and the test re-reads both words.
//
// Overlay-arena physics step using the scratch globals at 0x700038A0/B0/AC and
// 0x70003A20/A24/B88. Seeds the scratch matrices: func_00183C40(arg1,&D_700038B0),
// func_001028D0(&D_700038A0,&D_700038B0, arg0+0xB0), clears the 0x700038AC flag,
// and stores the squared length func_00102738(&D_700038A0,&D_700038A0) at
// 0x70003A20. Stores r = *(*(arg0+0x30)) + *(*(arg1+0x30)) at 0x70003A24 and squares
// it in place. If the squared length <= r*r, runs the response path:
// func_001A91C0(arg0, arg1, sqrt(squared length)), re-seeds the scratch via
// func_001028D0, sets 0x700038AC = 1.0f, func_00102760(arg1+0x70, &D_700038A0),
// and clears the short at 0x70003B88.
extern void func_00183C40(char *p, void *m);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern float func_0011E748(float x);
extern void func_001A91C0(char *p, char *q, float f);
extern void func_00102760(void *a, void *b);
extern char D_700038A0[];
extern char D_700038B0[];
extern float D_70003A20;
extern float D_70003A24;

void func_001A9360(char *arg0, char *arg1) {
    func_00183C40(arg1, &D_700038B0);
    func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
    *(int *)0x700038AC = 0;
    D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
    D_70003A24 = **(float **)(arg0 + 0x30) + **(float **)(arg1 + 0x30);
    D_70003A24 = D_70003A24 * D_70003A24;
    if (D_70003A20 <= D_70003A24) {
        func_001A91C0(arg0, arg1, func_0011E748(D_70003A20));
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0x3F800000;
        func_00102760(arg1 + 0x70, &D_700038A0);
        *(short *)0x70003B88 = 0;
    }
}
