// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-packing thunk: assembles the low byte of each of 5 unsigned-int args
// into a 40-bit value (b0 | b1<<8 | b2<<16 | b3<<24 | (u64)b4<<32) and
// tail-calls func_00205A50(dst, 1, packed). func_00205A50 stores packed.lo32
// at dst+0, packed.hi32 (sign-extended) at dst+4, the constant 1 at dst+8,
// and 0 at dst+0xC. The `j` (not jal) tail-call with the OR in its delay slot
// is CW codegen the pinned 991202 build cannot reproduce (it reorders the two
// dsrl32 ops and mis-fills the j delay slot -> walled at 79%); mwcc 2.3.3
// (mwcps2-2.3.3-000906) is byte-identical. Verified objdiff 100% vs
// build/expected/func_00205C80.o.
extern void func_00205A50(void *dst, int kind, long long packed);

void func_00205C80(void *dst, unsigned int b0, unsigned int b1, unsigned int b2, unsigned int b3, unsigned int b4) {
    func_00205A50(dst, 1,
        (long long)b0
        | ((long long)b1 << 8)
        | ((long long)b2 << 16)
        | ((long long)b3 << 24)
        | ((long long)b4 << 32));
}
