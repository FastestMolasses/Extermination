// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.07% NEARMISS).
// Fixes read from the original instructions:
// - The first pass is the fall-through arm: the test is `if (i == 0)` with the
//   first-pass staging first and the later-pass arm in the else.
// - Saved-register roles come from local declaration order (q, i, p, base,
//   label, n, buffer cursor, buffer).
// - The entity address is an integer sum (p + *p + p[0x30]) so the 0x30 word is
//   the first addend of the final add.
//
// Setup loop over an entity resolved from D_0028A49C (base = p + *(int*)p, then
// + *(int*)(p+0x30)); func_001FE460 primes it and func_001FE480 looks up entry q
// for arg0. If q is live (*q != 0), func_001FE660 gives an upper count n; the
// loop runs up to 10 passes while (arg1*0xA + i) < n, staging each pass through
// func_001FE530 into a 0x280 stack buffer (0x40 stride) and emitting via
// func_001FC770(0x38, label, ..., D_00264CF0) with label starting at 0x2F and
// advancing +0xC after the first pass. Returns 1.
extern void func_001FE460(char *p);
extern unsigned char *func_001FE480(char *p, int idx);
extern unsigned char *func_001FE530(char *buf, unsigned char *p, int n);
extern int func_001FE660(unsigned char *p);
extern void func_001FC770(int a, int b, char *buf, char *tbl);
extern char *D_0028A49C;
extern char D_00264CF0[];

int func_001FCF90(int arg0, int arg1) {
    unsigned char *q;
    int i;
    char *p;
    int base;
    int label;
    int n;
    char *vs5;
    char buf[0x280];

    base = arg1 * 0xA;
    label = 0x2F;
    p = D_0028A49C;
    p = (char *)((int)p + *(int *)p + *(int *)(p + 0x30));
    func_001FE460(p);
    q = func_001FE480(p, arg0);
    if (*q != 0) {
        n = func_001FE660(q);
        vs5 = buf;
        i = 0;
        while (i < 0xA && base + i < n) {
            if (i == 0) {
                q = func_001FE530(buf, q, base);
                func_001FC770(0x38, label, buf, D_00264CF0);
            } else {
                label += 0xC;
                q = func_001FE530(vs5, q, 0);
                func_001FC770(0x38, label, vs5, D_00264CF0);
            }
            vs5 += 0x40;
            i++;
        }
    }
    return 1;
}
