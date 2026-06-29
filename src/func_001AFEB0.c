// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Iterates the 24-entry array D_0028B020 (stride 0x2F0); for every entry whose
// first byte is nonzero, calls func_001AF800(entry). The empty-slot test is a
// branch-likely (beqzl) with the loop counter bump in its delay slot. mwcc 2.3.3
// is byte-identical; the pinned 991202 build reaches 99.52% -- it colors the
// loop-limit compare into $at (slti at) instead of reusing $v1 (the just-loaded
// byte reg) as 2.3.3 does. Verified objdiff 100% vs build/expected.
extern void func_001AF800(char *);
extern char D_0028B020[24][0x2f0];

void func_001AFEB0(void) {
    char *p;
    int i;
    p = D_0028B020[0];
    i = 0;
    do {
        if (*(unsigned char *)p != 0) {
            func_001AF800(p);
        }
        i++;
        p += 0x2f0;
    } while (i < 0x18);
}
