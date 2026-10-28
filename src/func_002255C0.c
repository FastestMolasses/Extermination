// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Three-step actor sub-state handler on the step byte self+6.
// Step 0: when the float self+0x224 is nonzero, func_001FBD50(self, 0x146, 0,
// 300.0f) and func_0021C350(self). When the float self+0x220 is <= 0: next step,
// clear self+7, func_001FBD50(self, 0x156, 0, 300.0f), timer self+0x28 = 16.
// Step 1: post-decrements the timer self+0x28; when it was 0: next step,
// func_0021D490(self), then rumble func_001B61C0(1, 0xEE, 0x3C, 1).
// Step 2: func_0021D2E0(self, 0x78, 0) every tick.
// The named `zero` local keeps the original's operand order on the first
// float compare.
extern void func_001B61C0(int big, int small, int dur, int force);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_0021C350(char *e);
extern void func_0021D2E0(char *arg0, short arg1, int arg2);
extern void func_0021D490(char *arg0);

void func_002255C0(char *self) {
    float zero = 0.0f;

    switch (*(unsigned char *)(self + 6)) {
    case 0:
        if (*(float *)(self + 0x224) != zero) {
            func_001FBD50(self, 0x146, 0, 300.0f);
            func_0021C350(self);
        }
        if (*(float *)(self + 0x220) <= 0.0f) {
            *(unsigned char *)(self + 6) += 1;
            *(unsigned char *)(self + 7) = 0;
            func_001FBD50(self, 0x156, 0, 300.0f);
            *(short *)(self + 0x28) = 0x10;
        }
        break;
    case 1:
        if ((*(short *)(self + 0x28))-- == 0) {
            *(unsigned char *)(self + 6) += 1;
            func_0021D490(self);
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 2:
        func_0021D2E0(self, 0x78, 0);
        break;
    }
}
