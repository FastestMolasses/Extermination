// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Two-step free-space query on the step byte self+6 (returns 0 always).
// Step 0: self+0x5C = self+0x38, then func_00228320(self) (free-space check);
// on success sets self+5 = 1; moves to step 1.
// Step 1: self+0x5C = self+0x3C, func_00228320(self); self+5 = 1 on success,
// 2 on failure; back to step 0.
// (func_00229A00 calls it with a `(void)` declaration; self passes through in
// the first argument register.)
extern int func_00228320(char *arg0);

int func_00229960(char *self) {
    unsigned char st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(int *)(self + 0x5C) = *(int *)(self + 0x38);
        if (func_00228320(self) != 0) {
            *(char *)(self + 5) = 1;
        }
        *(unsigned char *)(self + 6) = 1;
        break;
    case 1:
        *(int *)(self + 0x5C) = *(int *)(self + 0x3C);
        if (func_00228320(self) != 0) {
            *(char *)(self + 5) = 1;
        } else {
            *(char *)(self + 5) = 2;
        }
        *(unsigned char *)(self + 6) = 0;
        break;
    }
    return 0;
}
