// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Dispatch on the byte state field at +6: case 1 calls func_0021D2E0(p,0x78,0)
// directly; case 0 first bumps +6 and clears the +7 byte, then falls through
// into the same call. mwcc emits beql/beqzl (descending case test order) with
// pure-ALU delay-slot fills (li a1,0x78 / addiu v0,a1,1) per idiom-13. The 991202
// build fills the second beql delay-slot copy differently (residual addiu); mwcc
// 2.3.3 reproduces the target byte-for-byte.
extern void func_0021D2E0(unsigned char *p, int a, int b);

void func_00225570(unsigned char *p) {
    switch (p[6]) {
    case 0:
        p[6] = p[6] + 1;
        p[7] = 0;
        /* fallthrough */
    case 1:
        func_0021D2E0(p, 0x78, 0);
        break;
    }
}
