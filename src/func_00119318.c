// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Sound system: reset with argument 0 (tail call of func_001190B8).

extern void func_001190B8(int arg0);

void func_00119318(void) {
    func_001190B8(0);
}
