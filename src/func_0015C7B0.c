// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Forwarder: calls func_0015C1F0 with the same argument. mwcc 2.3.3 emits
// this as a bare jump (it does perform this tail call), so the C is the
// whole function.
extern void func_0015C1F0(char *arg0);

void func_0015C7B0(char *arg0) {
    func_0015C1F0(arg0);
}
