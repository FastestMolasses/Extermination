// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Writes a 16-byte record at *a0 and returns a0 advanced past it (a0+0x10):
//   +0x00  low  32 bits of the 64-bit value a2 (zero-extended via dsll32/dsrl32)
//   +0x04  high 32 bits of a2, sign-extended (dsrl32 to grab the high half,
//          then dsll32/dsra32 to sign-extend the stored int)
//   +0x08  a1 (a 32-bit value/handle)
//   +0x0C  0
// Returns a0 + 0x10. Looks like an append-a-(u64,word) entry to a record
// stream/cursor, with a trailing zero pad word.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the optimizer folds the 64->32 low-word truncation directly into
// `sw a2,0(a0)` and drops the dsll32/dsrl32 materialization CW emits (63.6%).
// The 2.3.3 build keeps the explicit shift-pair materialization, matching CW
// byte-for-byte. The low word is written through `(unsigned int)(a2 & 0xFFFFFFFF)`
// (the &0xFFFFFFFF form is what makes mwcc materialize the zero-extended low
// half instead of folding). Verified objdiff 100% byte-identical vs
// build/expected/func_00205A50.o.
int *func_00205A50(int *a0, int a1, unsigned long long a2) {
    *(unsigned int *)a0 = (unsigned int)(a2 & 0xFFFFFFFF);
    a0[1] = (int)(a2 >> 32);
    a0[2] = a1;
    a0[3] = 0;
    return a0 + 4;
}
