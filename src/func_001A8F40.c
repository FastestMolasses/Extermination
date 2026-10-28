// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.21% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad floats 0x70003A20 / 0x70003A24 are relocated externs
//    (idiom-32, one per address), spelled like the byte-matched twin
//    func_00156620: the radius is stored, squared in place (`*=`), and the
//    compare reads both scratchpad words. No float local is needed; the
//    old "FP-coloring wall" came from the literal addresses and the local.
//
// Semantics: runs func_001028D0 over the two 0xB0 sub-blocks (arg1+0xB0 and
// arg0+0xB0) into scratch D_700038A0; zeroes the word at 0x700038AC; stores
// func_00102738(scratch, scratch) (the squared distance) into 0x70003A20.
// Then the radius 3.0f + *(*(float **)(arg0 + 0x30)) is stored at 0x70003A24
// and squared in place; if the squared distance is <= the squared radius,
// writes the short 0x2014 to arg1+0x36 and zeroes the short at 0x70003B88.
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern int D_700038A0;
extern float D_70003A20;
extern float D_70003A24;

void func_001A8F40(char *arg0, char *arg1) {
    func_001028D0(&D_700038A0, arg1 + 0xB0, arg0 + 0xB0);
    *(int *)0x700038AC = 0;
    D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
    D_70003A24 = 3.0f + **(float **)(arg0 + 0x30);
    D_70003A24 *= D_70003A24;
    if (D_70003A20 <= D_70003A24) {
        *(short *)(arg1 + 0x36) = 0x2014;
        *(short *)0x70003B88 = 0;
    }
}
