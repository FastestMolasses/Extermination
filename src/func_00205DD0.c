// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sibling of func_00205D00 with different field positions. Builds a 64-bit
// value and tail-calls func_00205A50(a0, 0x40, packed) (emitted as `j`):
//   packed = ((u32)b4 << 48) | ((u32)b3 << 32) | ((u32)b2 << 16) | (u32)b1
// Same recipe: widen each operand via (unsigned long long)(unsigned int) so
// shifts lower to dsll/dsll32, and group the OR tree as
// ((b3<<32) | (b1 | (b2<<16))) | (b4<<48) to match CW's order/regalloc.
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906); the 991202 build schedules the
// zero-extensions differently (48.75%). objdiff 100% byte-identical.
extern int *func_00205A50(int *a0, int a1, unsigned long long a2);

int *func_00205DD0(int *a0, int b1, int b2, int b3, int b4) {
    return func_00205A50(a0, 0x40,
        ((((unsigned long long)(unsigned int)b3 << 32)
          | ((unsigned long long)(unsigned int)b1
             | ((unsigned long long)(unsigned int)b2 << 16)))
         | ((unsigned long long)(unsigned int)b4 << 48)));
}
