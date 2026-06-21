// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Align-up wrapper: calls func_0010F8F8(a0+0x10), rounds result up to 16.
// Matched via mwcc 2.3.3 (clean-store idiom-13 beqz delay-slot nop that 991202 fills).
extern int func_0010F8F8(int);

int func_001FA6A0(int a0) {
    int v0 = func_0010F8F8(a0 + 0x10);
    if (v0 & 0xF) {
        v0 = (v0 & -0x10) + 0x10;
    }
    return v0;
}
