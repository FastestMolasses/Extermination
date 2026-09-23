// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m2-matching lane, 96.05% -> 100.0%): (1) the table store is
// `if ((unsigned char)d == 3 || (unsigned char)d == 4) ... else ...` on an int
// d, indexing with `(unsigned char)d`. That gives the target's per-use
// byte mask (d & 0xFF) + shift and its equal / not-equal branch-pair `||` lowering with the dead
// fall-through copy. (2) The 0x70003B86 clear is a relocated extern
// (idiom-32), so its `lui` is no longer speculated into the state-test slot.
// func_0021BD10 takes no arguments (its byte-matched definition). The target
// happens to hold arg0[0] in $a0 at that call; passing it or not gives the same
// bytes.
//
// Circle/box overlap test between two entities (arg0, arg1). Computes planar
// distance via func_0011E748 (length of dx,dy from arg0+0xA0/0xA8 vs
// arg1+0xB0/0xB8) and compares against the sum of radii at *(arg0+0x30)[0] +
// *(arg1+0x30)[0]; then an axis-overlap test on the 0xA4/0xB4 components using
// half-widths from [+4]. On overlap it fires the arg1 vtable handler at +0x34,
// and if arg0's state byte==1 picks a float from one of two tables
// (D_0024A740/D_0024A780, selected by global D_0081070A) indexed by arg1's
// type byte at +0xD, storing to arg0+0x22C (types 3,4) or arg0+0x224 (other),
// sets state=3 and writes normalize((arg0+0xA0) - (arg1+0xB0)) (w = 1) to
// arg0+0x70 via scratch D_700038A0. Any overlap clears the flag D_70003B86.
extern float func_0011E748(float);
extern int func_0021BD10(void);
extern int func_001028D0(void *, void *, void *);
extern int func_00102760(void *, void *);
extern float D_0024A740;
extern float D_0024A780;
extern unsigned char D_0081070A;
extern char D_700038A0;
extern short D_70003B86;

void func_001A8660(char *arg0, char *arg1) {
    char *va;
    char *vb;
    float *base;
    float dx, dy;
    float t3, f2;
    int d;

    dx = *(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0);
    dy = *(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8);
    if (func_0011E748(dx * dx + dy * dy) <= (*(float *)*(char **)(arg0 + 0x30) + *(float *)*(char **)(arg1 + 0x30))) {
        va = *(char **)(arg0 + 0x30);
        vb = *(char **)(arg1 + 0x30);
        t3 = *(float *)(va + 4) / 2.0f;
        f2 = (*(float *)(arg0 + 0xA4) + t3) - *(float *)(arg1 + 0xB4);
        if (f2 < 0.0f) {
            f2 = -f2;
        }
        if (f2 <= (t3 + *(float *)(vb + 4) / 2.0f)) {
            (*(void (**)(char *, char *, char *))(arg1 + 0x34))(arg1, arg0, arg0 + 0xB0);
            if (*(unsigned char *)arg0 == 1) {
                if (*(unsigned char *)(arg1 + 0xD) == 0xB) {
                    if (func_0021BD10() != 0) {
                        *(char *)(arg0 + 0xF) = 2;
                    }
                }
                if (D_0081070A == 0) {
                    base = &D_0024A740;
                } else {
                    base = &D_0024A780;
                }
                d = *(unsigned char *)(arg1 + 0xD);
                if ((unsigned char)d == 3 || (unsigned char)d == 4) {
                    *(float *)(arg0 + 0x22C) = base[(unsigned char)d];
                } else {
                    *(float *)(arg0 + 0x224) = base[(unsigned char)d];
                }
                *(unsigned char *)arg0 = 3;
                func_001028D0(&D_700038A0, arg0 + 0xA0, arg1 + 0xB0);
                *(int *)0x700038AC = 0x3F800000;
                func_00102760(arg0 + 0x70, &D_700038A0);
            }
            D_70003B86 = 0;
        }
    }
}
