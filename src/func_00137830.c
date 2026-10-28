// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick update of a sub-object block at self+0x1F0 (sub), dispatched on
// the state byte self+4: 0 func_00137920, 1 func_00137A50, 2 func_00137C80,
// 3 func_00137E80, each called as (self, sub). State 3 returns right after
// its handler; the other states then count down the four signed-byte timers
// sub+0x42, sub+0x41, sub+0x40, sub+0x44 toward zero.
// func_00137E80 is passed sub as a second argument here although its own
// definition takes one parameter (the extra argument is ignored).

extern void func_00137920(char *arg0, char *arg1);
extern void func_00137A50(char *p, char *q);
extern void func_00137C80(char *arg0, char *arg1);
extern void func_00137E80(char *obj, char *sub);

void func_00137830(char *self) {
    char *sub = self + 0x1F0;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        func_00137920(self, sub);
        break;
    case 1:
        func_00137A50(self, sub);
        break;
    case 2:
        func_00137C80(self, sub);
        break;
    case 3:
        func_00137E80(self, sub);
        return;
    }
    if (*(signed char *)(sub + 0x42) != 0) {
        *(signed char *)(sub + 0x42) -= 1;
    }
    if (*(signed char *)(sub + 0x41) != 0) {
        *(signed char *)(sub + 0x41) -= 1;
    }
    if (*(signed char *)(sub + 0x40) != 0) {
        *(signed char *)(sub + 0x40) -= 1;
    }
    if (*(signed char *)(sub + 0x44) != 0) {
        *(signed char *)(sub + 0x44) -= 1;
    }
}
