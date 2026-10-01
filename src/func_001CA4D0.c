// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
typedef unsigned int u128 __attribute__((mode(TI)));

void func_001CA4D0(float *dst, float *a, float *b) {
    float t[4];

    t[0] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0];
    t[1] = -a[0] * b[2] + a[1] * b[3] + a[2] * b[0] + a[3] * b[1];
    t[2] = a[0] * b[1] - a[1] * b[0] + a[2] * b[3] + a[3] * b[2];
    t[3] = -a[0] * b[0] - a[1] * b[1] - a[2] * b[2] + a[3] * b[3];
    *(u128 *)dst = *(u128 *)t;
}
