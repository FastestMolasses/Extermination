// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin packer/wrapper: builds a 64-bit value from four byte-fields and
// tail-calls func_00205A50(a0, 0x4C, packed) (emitted as a `j`, not jal).
//   packed = ((u32)b4 << 32) | ((u32)b3 << 24) | ((u32)b2 << 16) | (u32)b1
// Each operand must be widened via (unsigned long long)(unsigned int) so the
// shifts lower to 64-bit dsll/dsll32 (not 32-bit sll), and the OR tree must be
// grouped ((b3<<24) | (b1 | (b2<<16))) | (b4<<32) to match CW's evaluation
// order / register allocation. Built with mwcc 2.3.3 (mwcps2-2.3.3-000906):
// the 991202 scheduler orders the dsll32/dsrl32 zero-extensions differently
// (58.75%). objdiff 100% byte-identical.
extern int *func_00205A50(int *a0, int a1, unsigned long long a2);

int *func_00205D00(int *a0, int b1, int b2, int b3, int b4) {
    return func_00205A50(a0, 0x4C,
        ((((unsigned long long)(unsigned int)b3 << 24)
          | ((unsigned long long)(unsigned int)b1
             | ((unsigned long long)(unsigned int)b2 << 16)))
         | ((unsigned long long)(unsigned int)b4 << 32)));
}
