// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Flag check: byte at offset 2 masked with ~0xE0 (== -0xE1) must equal 4, and
// byte at offset 3 must equal 2 -> returns 1, otherwise 0. mwcc emits the
// 'paddub v0,zero,zero' SIMD zero-move idiom for the zero results.
int func_0017D040(unsigned char *a0) {
    if ((a0[2] & ~0xE0) == 4) {
        if (a0[3] == 2) {
            return 1;
        }
    }
    return 0;
}
