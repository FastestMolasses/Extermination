// COMPILER: eegcc
// CFLAGS: -O2
// SDK: copies one 128-bit quadword, *dst = *src. The register variable only
// reproduces the original's choice of a2.
typedef unsigned int u128 __attribute__((mode(TI)));

void func_00102948(u128 *dst, u128 *src) {
    register u128 r0 asm("$6") = *src;
    *dst = r0;
}
