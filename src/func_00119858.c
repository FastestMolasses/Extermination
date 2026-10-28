// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Sound command queue: posts command 0x18 (a0, a1, 0) through func_001157F0.

extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_00119858(int a0, int a1) {
    func_001157F0(0x18, a0, a1, 0);
}
