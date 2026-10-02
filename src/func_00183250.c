// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sub-mode 1 handler (step byte self+6). Step 0 advances, clears +7, sets
// self+0x38 = 0.3 and self+0x25C = 2, starts the clip func_0017B490(self, 1,
// self+0x235, self+0x25C) and sets the timer self+0x28 = 50. Step 1: when the
// timer (post-decremented) was 0, the step advances and the timer is 30. Step 2:
// the same, else func_00178B90(self, 0). Step 3: when the timer was 0 the
// object returns to state 1 / sub-mode 0 / step 0, self+0x1F0 = 0 and 0x70003B8D = 0;
// else func_00178B90(self, 0) and self+0x38 -= 0.011363637; below 0 it is
// clamped to 0 and func_00174A50(self, 12.0) runs. Every frame then self+0xB4
// += -0.2 and func_00175900(self, 1).
extern unsigned char D_70003B8D;
extern int func_0017B490(unsigned char *e, int a1, int variant, int a3);
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00178B90(unsigned char *e, int f);
extern void func_00174A50(unsigned char *e, float f);
extern int func_00175900(unsigned char *e, int f);

void func_00183250(unsigned char *self) {
    unsigned char st = self[6];
    short t;
    switch (st) {
    case 0:
        self[6] = st + 1;
        self[7] = 0;
        *(float *)(self + 0x38) = 0.3f;
        self[0x25C] = 2;
        func_001749A0(self, (short)func_0017B490(self, 1, self[0x235], self[0x25C]), 0, 1.0f);
        *(short *)(self + 0x28) = 0x32;
        break;
    case 1:
        t = *(short *)(self + 0x28);
        *(short *)(self + 0x28) = t - 1;
        if (t == 0) {
            self[6]++;
            *(short *)(self + 0x28) = 0x1E;
        }
        break;
    case 2:
        t = *(short *)(self + 0x28);
        *(short *)(self + 0x28) = t - 1;
        if (t == 0) {
            self[6]++;
            *(short *)(self + 0x28) = 0x1E;
        } else {
            func_00178B90(self, 0);
        }
        break;
    case 3:
        t = *(short *)(self + 0x28);
        *(short *)(self + 0x28) = t - 1;
        if (t == 0) {
            self[4] = 1;
            self[5] = 0;
            self[6] = 0;
            self[0x1F0] = 0;
            D_70003B8D = 0;
        } else {
            func_00178B90(self, 0);
            *(float *)(self + 0x38) -= 0.011363637f;
            if (*(float *)(self + 0x38) < 0.0f) {
                *(float *)(self + 0x38) = 0.0f;
                func_00174A50(self, 12.0f);
            }
        }
        break;
    }
    *(float *)(self + 0xB4) += -0.2f;
    func_00175900(self, 1);
}
