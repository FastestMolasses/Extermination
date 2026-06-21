// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Selects one of two func_001FB9F0 calls based on the byte at arg0+3. If that byte
// is 0x12 or 0x2F, calls func_001FB9F0(0x3F3, 0x1000, 0x1000, 0x1000); otherwise
// calls func_001FB9F0(0x3F4, 0x1000, 0x1000, 0x1000). The repeated 0x1000 args are
// emitted via paddub register copies (idiom-17). Always returns 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (89%): the
// residual was the idiom-13 conditional-branch delay-slot nop (991202 fills it,
// CW/2.3.3 leave it). 2.3.3 is byte-identical. Verified objdiff 100% vs
// build/expected/func_00158130.o.
extern void func_001FB9F0(int a0, int a1, int a2, int a3);

int func_00158130(char *arg0) {
    unsigned char v1;

    v1 = *(unsigned char *)(arg0 + 3);
    if (v1 == 0x12 || v1 == 0x2F) {
        func_001FB9F0(0x3F3, 0x1000, 0x1000, 0x1000);
    } else {
        func_001FB9F0(0x3F4, 0x1000, 0x1000, 0x1000);
    }
    return 1;
}
