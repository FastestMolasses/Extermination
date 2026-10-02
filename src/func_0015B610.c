// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sub-mode dispatcher (byte self+5). When the scratchpad byte
// 0x70003B8D is nonzero and not 4 and func_00182B30(self) returns 0, the
// object is forced into state 4: sub-mode 3 goes to sub-mode 12 with self+0x1F0 = 0x17;
// any other sub-mode (after func_00174A50(self, 8.0) for sub-mode 1) goes to
// sub-mode 0 with self+0x1F0 = 0x41; self+6 = 0 and func_00182D70(self) follows.
// Otherwise sub-mode 0..4 runs 00183240, 00183250, 001833F0, 00183440 or
// 001834E0.
extern unsigned char D_70003B8D;
extern int func_00182B30(unsigned char *self);
extern void func_00174A50(unsigned char *e, float f);
extern void func_00182D70(unsigned char *self);
extern void func_00183240(unsigned char *self);
extern void func_00183250(unsigned char *self);
extern void func_001833F0(unsigned char *self);
extern void func_00183440(unsigned char *self);
extern void func_001834E0(unsigned char *self);

void func_0015B610(unsigned char *self) {
    unsigned char m = D_70003B8D;
    unsigned char st;
    if (m != 0 && m != 4 && func_00182B30(self) == 0) {
        st = self[5];
        if (st == 3) {
            self[4] = 4;
            self[5] = 0xC;
            self[6] = 0;
            self[0x1F0] = 0x17;
        } else {
            if (st == 1) {
                func_00174A50(self, 8.0f);
            }
            self[4] = 4;
            self[5] = 0;
            self[6] = 0;
            self[0x1F0] = 0x41;
        }
        func_00182D70(self);
        return;
    }
    switch (self[5]) {
    case 0:
        func_00183240(self);
        break;
    case 1:
        func_00183250(self);
        break;
    case 2:
        func_001833F0(self);
        break;
    case 3:
        func_00183440(self);
        break;
    case 4:
        func_001834E0(self);
        break;
    }
}
