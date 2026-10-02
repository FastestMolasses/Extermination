// COMPILER: eegcc
// CFLAGS: -O2
// SDK: copies a 4x4 matrix as four 128-bit quadwords (all four loaded
// before the stores, so dst may alias src). The register variables (a2, a3,
// t0, t1) and the empty barriers carry no meaning of their own: they
// reproduce the original's register choice and its in-order stores.
typedef unsigned int u128 __attribute__((mode(TI)));

void copy_qw4(u128 *dst, volatile u128 *src) {
    register u128 r0 asm("$6") = src[0];
    register u128 r1 asm("$7") = src[1];
    register u128 r2 asm("$8") = src[2];
    register u128 r3 asm("$9") = src[3];

    dst[0] = r0;
    asm volatile("" ::: "memory");
    dst[1] = r1;
    asm volatile("" ::: "memory");
    dst[2] = r2;
    asm volatile("" ::: "memory");
    dst[3] = r3;
}
