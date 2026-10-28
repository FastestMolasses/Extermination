// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick driver: dispatches the state byte self+4 to its handler with
// (self, self+0x1F0) -- 0 func_00153A10, 1 func_00153A90, 2 func_00153B50, 3
// func_00153EA0 (state 3 returns at once). Otherwise, if the sub-block byte
// +0x10 is set, calls func_001B5360(self). func_00153B50 and func_00153EA0 are
// declared with the two arguments the original passes (their definitions
// take three and one).
extern void func_00153A10(char *self, char *st);
extern void func_00153A90(char *arg0, int *arg1);
extern void func_00153B50(char *self, char *st);
extern void func_00153EA0(char *self, char *st);
extern void func_001B5360(char *self);

void func_00153950(char *self) {
    char *st = self + 0x1F0;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        func_00153A10(self, st);
        break;
    case 1:
        func_00153A90(self, (int *)st);
        break;
    case 2:
        func_00153B50(self, st);
        break;
    case 3:
        func_00153EA0(self, st);
        return;
    }
    if (*(signed char *)(st + 0x10) != 0) {
        func_001B5360(self);
    }
}
