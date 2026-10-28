// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.47% NEARMISS).
// Fixes read from the original instructions:
// - The scratch words at 0x70003A20 / 0x70003A24 are relocated extern floats
//   (idiom-32), spelled as in the byte-matched twin func_001A9360: the radius sum
//   is stored at 0x70003A24, squared in place from that word, and the test
//   re-reads both words (this gives the early 0x70003A20 reload and the
//   original's float register use).
// - The special-item test is inverted: if the squared length is beyond the first
//   radius the 12-entry table scan runs, else hit = 1 (the original places the
//   hit = 1 arm after the loop).
// - Local declaration order i, hit, rec (i in the lowest saved register).
//
// Overlay-arena proximity test of actor arg0 against arg1, using the scratch
// globals at 0x700038A0/B0/AC and 0x70003A20/A24/B88. Seeds the scratch via
// func_00183C40(arg1, &D_700038B0) and func_001028D0(&D_700038A0, &D_700038B0,
// arg0+0xB0), clears 0x700038AC and stores the squared length
// func_00102738(&D_700038A0, &D_700038A0) at 0x70003A20; the radius is
// *(*(arg0+0x30)) + *(*(arg1+0x30)), squared. When arg1+3 == 0xB (special item)
// and that test fails, it scans the 12 indices in D_0024A840: each selects a
// record pointer at arg1+0x110 + 4*index, whose +0xC0 point is tested against
// arg0+0xB0 with radius 6.0f + *(*(arg0+0x30)). On any hit it sets the short at
// arg1+0x36 to 0x400A, calls func_00102948(arg1+0x70, arg0+0xC0) and clears the
// short at 0x70003B88.
extern void func_00183C40(char *p, void *m);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern void func_00102948(char *dst, char *src);
extern unsigned char D_0024A840[];
extern char D_700038A0[];
extern char D_700038B0[];
extern float D_70003A20;
extern float D_70003A24;
void func_001A9480(char *arg0, char *arg1) {
    int i;
    int hit;
    char *rec;
    if (*(unsigned char *)(arg1 + 3) == 0xB) {
        hit = 0;
        func_00183C40(arg1, &D_700038B0);
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0;
        D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
        D_70003A24 = **(float **)(arg0 + 0x30) + **(float **)(arg1 + 0x30);
        D_70003A24 = D_70003A24 * D_70003A24;
        if (D_70003A20 > D_70003A24) {
            for (i = 0; i < 0xC; i++) {
                rec = *(char **)(arg1 + 0x110 + (D_0024A840[i] << 2));
                func_001028D0(&D_700038A0, rec + 0xC0, arg0 + 0xB0);
                *(int *)0x700038AC = 0;
                D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
                D_70003A24 = 6.0f + **(float **)(arg0 + 0x30);
                D_70003A24 = D_70003A24 * D_70003A24;
                if (D_70003A20 <= D_70003A24) {
                    hit = 1;
                    break;
                }
            }
        } else {
            hit = 1;
        }
        if (hit == 0) {
            return;
        }
    } else {
        func_00183C40(arg1, &D_700038B0);
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0;
        D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
        D_70003A24 = **(float **)(arg0 + 0x30) + **(float **)(arg1 + 0x30);
        D_70003A24 = D_70003A24 * D_70003A24;
        if (D_70003A20 > D_70003A24) {
            return;
        }
    }
    *(short *)(arg1 + 0x36) = 0x400A;
    func_00102948(arg1 + 0x70, arg0 + 0xC0);
    *(short *)0x70003B88 = 0;
}
