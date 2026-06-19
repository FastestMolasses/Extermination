// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: decode two 64-bit values via func_00126BE8, combine, finalize.
extern void func_00126BE8(long long *a0, int *a1);
extern int func_00126C88(int *a0, int *a1, int *a2);
extern int func_00126AB8(int a0);

int func_00126EC8(long long a0, long long a1) {
    long long s0 = a0;
    long long s1 = a1;
    int A[8];
    int B[8];
    int C[8];
    func_00126BE8(&s0, A);
    func_00126BE8(&s1, B);
    return func_00126AB8(func_00126C88(A, B, C));
}
