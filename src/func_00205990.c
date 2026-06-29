// CFLAGS: -O4,p -sdatathreshold 0
// GIFtag/qword bit-packing leaf: assembles a 64-bit value from six fields
// (each zero-extended to 32 bits) and stores it at *dst, returning dst+0x10
// (next 16-byte slot). Fields: a1 -> bit 63, a3 -> bit 31, t0 -> bit 28,
// t1 -> bit 26, t2 -> low 32 bits, and (a2 & ~0xF) -> bits 32+. The OR must be
// right-associated with (a1<<63)|(a2_masked<<32) innermost so mwcc keeps the
// three flag fields (t3/a3/a1) live in separate regs and folds them late --
// the exact CW OR-fold order. Verified objdiff 100% on both builds; mwcc
// (991202) preferred since both reach 100.
long long *func_00205990(long long *dst, unsigned int a1, int a2, unsigned int a3,
                         unsigned int t0, unsigned int t1, unsigned int t2) {
    *dst = (long long)(unsigned int)t2
        | (((long long)(unsigned int)t1 << 26)
        | (((long long)(unsigned int)t0 << 28)
        | (((long long)(unsigned int)a3 << 31)
        | (((long long)(unsigned int)a1 << 63)
        | ((long long)(unsigned int)(a2 & ~0xF) << 32)))));
    return (long long *)((int)dst + 0x10);
}
