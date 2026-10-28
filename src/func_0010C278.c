// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Cache helper (SDK libkernel): rounds both addresses of a range down to a
// 64-byte cache-line boundary and tail-calls the cache-line loop
// func_0010C158 (which is hand-written: cache ops + sync, no C form).

extern void func_0010C158(unsigned int start, unsigned int end);

void func_0010C278(unsigned int start, unsigned int end) {
    func_0010C158(start & ~0x3F, end & ~0x3F);
}
