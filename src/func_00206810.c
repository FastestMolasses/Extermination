// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// 8-argument layout splitter (args 5-8 arrive in t0-t3 under the EE ABI).
// Given two paired spans (a1+a3 vs t1+t3 totals), it clamps the t1/t3 lengths so
// the totals balance, then emits up to three func_00206970(dst, src, len) runs
// choosing one of three split shapes by comparing the (clamped) t1/t3 lengths.
// The inner (a1 - t1) subexpression is inlined at every use (idiom-19): mwcc
// would otherwise CSE it into a callee-saved reg, dropping the target's repeated
// 'subu s0,s6,s3' and shifting every branch target by 4. Inlining keeps the
// recompute and the exact register coloring/branch offsets. Verified objdiff
// 100.0% with mwcc 2.3.3 (the pinned 991202 build walls at 88.4%).
extern void func_00206970(int a, int b, int c);

int func_00206810(int a0, int a1, int a2, int a3, int t0, int t1, int t2, int t3) {
    int total_lo;
    int total_hi;
    int rem;

    total_lo = a1 + a3;
    total_hi = t1 + t3;
    if (total_lo < total_hi) {
        rem = total_hi - total_lo;
        if (rem >= t3) {
            t1 -= rem - t3;
            t3 = 0;
        } else {
            t3 -= rem;
        }
    }
    if (t1 >= a1) {
        func_00206970(a0, t0, a1);
        func_00206970(a2, t0 + a1, t1 - a1);
        func_00206970((a2 + t1) - a1, t2, t3);
    } else if (t3 >= a1 - t1) {
        func_00206970(a0, t0, t1);
        func_00206970(a0 + t1, t2, a1 - t1);
        func_00206970(a2, (t2 + a1) - t1, t3 - (a1 - t1));
    } else {
        func_00206970(a0, t0, t1);
        func_00206970(a0 + t1, t2, t3);
    }
    return t1 + t3;
}
