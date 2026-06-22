// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Re-arms a scratchpad-region (0x70003600) state block and dispatches.
// Clears the 0x70003600 record's fields at +0x10/+0x14(=arg3 float)/+0x18/+0x1C,
// accumulates the elapsed-time float into +0x04, then forwards to func_0019AB20
// with the record base (&D_70003600) and field block (&D_70003610). The 4th
// dispatch arg is 6 when arg2!=0, else 0x80000006 (high bit = flag variant).
// arg0 is passed through; arg1 is an unused-here middle int parameter (held in
// $a1 so arg2 lands in $a2). Returns void.
//
// 100% on mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 = 89.2% (wall #13: it fills
// the `beqz` clean-store delay slot where CW/2.3.3 leave it). Verified objdiff
// 100% vs build/expected/func_001760C0.o.
extern int func_001031E0(void *);
extern int func_0019AB20(int, void *, void *, int);
extern int D_70003600;
extern int D_70003610;

void func_001760C0(int arg0, int arg1, int arg2, float fparg0) {
    func_001031E0(&D_70003600);
    *(int *)0x70003610 = 0;
    *(float *)0x70003614 = fparg0;
    *(int *)0x70003618 = 0;
    *(int *)0x7000361C = 0;
    *(float *)0x70003604 = *(float *)0x70003604 + fparg0;
    if (arg2 != 0) {
        func_0019AB20(arg0, &D_70003600, &D_70003610, 6);
        return;
    }
    func_0019AB20(arg0, &D_70003600, &D_70003610, 0x80000006);
}
