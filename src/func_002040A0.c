// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// arg0 is a 3-int record [arg0[0]=base, arg0[1], arg0[2]=count].
// Computes a 28-bit index: idx = (arg0[1] + (arg0[2]+1)*16) & 0x0FFFFFFF.
// If arg1 equals that index, returns 0; otherwise returns the logical
// (arg1 - arg0[0]) >> 11. The & 0x0FFFFFFF mask is what makes mwcc lower
// the truncation to the dsll32 4 / dsrl32 4 pair CW emits (matching the
// 28-bit field width) instead of an andi. The terminal `return 0` is the
// clean-store idiom-13 case: CW (and mwcc 2.3.3) leave the bne delay slot
// a nop and put the zeroing `paddub v0,zero,zero` in the b delay slot;
// the pinned 991202 build instead fills the bne slot (92.5%). Built with
// mwcc 2.3.3 (mwcps2-2.3.3-000906); objdiff 100% byte-identical.
unsigned int func_002040A0(int *arg0, int arg1) {
    if (arg1 == (arg0[1] + ((arg0[2] + 1) * 0x10) & 0x0FFFFFFF)) {
        return 0U;
    }
    return (unsigned int)(arg1 - arg0[0]) >> 0xB;
}
