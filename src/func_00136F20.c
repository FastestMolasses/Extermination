// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Selects one of four float coefficient sets (sel 0..3), storing them as raw
// IEEE bit patterns into the scratch block at 0x700038A0..0x700038AC (the 4th
// word is always 1.0f). Picks an animation clip id per case (0xF/0x19/3/0xD),
// indexes the table pointed to by D_00275B40 by that id, and feeds the row+0x90
// to func_001026A0 with the scratch block as both src and dst matrices. Finally
// kicks func_001EFD90 with tag 0x80000036 / table D_70003600 / arg p+0xC0.
// Assigning `clip` before the stores in each case is load-bearing: it makes
// mwcc color the clip into $v0 (reusing the case-3 comparator constant 3 in
// case 2), matching the target. mwcc 2.3.3 only; pinned 991202 reaches 87.7%
// (fills the clean-store delay-slot nop). objdiff 100.0 vs expected.
extern int func_001026A0(void *dst, int src, void *m);
extern void func_001EFD90(int a, void *b, int c);
extern int D_00275B40;
extern char D_70003600[];
extern char D_700038A0[];

void func_00136F20(int p, int sel) {
    int clip;

    switch (sel) {
    case 0:
        clip = 0xF;
        *(int *)0x700038A0 = 0x3FD78D50;
        *(int *)0x700038A4 = 0x4067DF3B;
        *(int *)0x700038A8 = 0x3E841893;
        *(int *)0x700038AC = 0x3F800000;
        break;
    case 1:
        clip = 0x19;
        *(int *)0x700038A0 = 0x409147AE;
        *(int *)0x700038A4 = 0x3D072B02;
        *(int *)0x700038A8 = 0xBD656042;
        *(int *)0x700038AC = 0x3F800000;
        break;
    case 2:
        clip = 3;
        *(int *)0x700038A0 = 0x40177CEE;
        *(int *)0x700038A4 = 0x40B12F1B;
        *(int *)0x700038A8 = 0x403CAC08;
        *(int *)0x700038AC = 0x3F800000;
        break;
    case 3:
        clip = 0xD;
        *(int *)0x700038A0 = 0x409E147B;
        *(int *)0x700038A4 = 0x3D54FDF4;
        *(int *)0x700038A8 = 0xBCBC6A7F;
        *(int *)0x700038AC = 0x3F800000;
        break;
    }

    func_001026A0(D_700038A0, *(int *)(D_00275B40 + clip * 4) + 0x90, D_700038A0);
    func_001EFD90(0x80000036, D_70003600, p + 0xC0);
}
