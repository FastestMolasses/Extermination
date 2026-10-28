// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick driver: dispatches the state byte self+4 to its handler with
// (self, self+0x1F0) -- 0 func_001516B0, 1 func_00151770, 2 func_00152CC0, 3
// func_00152E80 (state 3 returns at once). Otherwise counts down the timers
// in the sub-block (bytes +0x51, +0x52, +0x50 and shorts +0x42, +0x44, each
// only while nonzero) and calls func_001F91C0(self). func_00152E80's own
// definition takes one argument; the original passes the sub-block as a
// second one, so it is declared here with two.
extern void func_001516B0(char *arg0, char *arg1);
extern void func_00151770(unsigned char *self, unsigned char *st);
extern void func_00152CC0(int arg0, int arg1);
extern void func_00152E80(char *obj, char *st);
extern void func_001F91C0(char *p);

void func_00151590(char *self) {
    char *st = self + 0x1F0;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        func_001516B0(self, st);
        break;
    case 1:
        func_00151770((unsigned char *)self, (unsigned char *)st);
        break;
    case 2:
        func_00152CC0((int)self, (int)st);
        break;
    case 3:
        func_00152E80(self, st);
        return;
    }
    if (*(signed char *)(st + 0x51) != 0) {
        *(signed char *)(st + 0x51) -= 1;
    }
    if (*(signed char *)(st + 0x52) != 0) {
        *(signed char *)(st + 0x52) -= 1;
    }
    if (*(short *)(st + 0x42) != 0) {
        *(short *)(st + 0x42) -= 1;
    }
    if (*(signed char *)(st + 0x50) != 0) {
        *(signed char *)(st + 0x50) -= 1;
    }
    if (*(short *)(st + 0x44) != 0) {
        *(short *)(st + 0x44) -= 1;
    }
    func_001F91C0(self);
}
