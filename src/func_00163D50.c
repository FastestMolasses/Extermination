// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State handler (step byte self+7). Step 0 advances and starts clip
// 0x6D (func_001749A0(self, 0x6D, 0, 1.0)). Step 1 advances when bit 12 of
// self+0x200 is set. Step 2 runs func_00174AC0(self, 0); with self+0x23F above 1
// the step advances and func_0017C440(self, 0) runs, otherwise self+0x25C = 0
// and func_0017C540(self). Step 3 runs func_00178B90(self, 0) and, unless bit 15
// of self+0x200 is set, func_0017C540(self). Every frame then: func_001764E0,
// self+0xB4 += -0.2, func_00175900(self, 1), func_001796C0.
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00174AC0(unsigned char *e, int n);
extern void func_00178B90(unsigned char *e, int f);
extern void func_0017C440(unsigned char *e, int f);
extern void func_0017C540(unsigned char *e);
extern void func_001764E0(unsigned char *e);
extern int func_00175900(unsigned char *e, int f);
extern void func_001796C0(unsigned char *e);

void func_00163D50(unsigned char *self) {
    unsigned char st = self[7];
    switch (st) {
    case 0:
        self[7] = st + 1;
        func_001749A0(self, 0x6D, 0, 1.0f);
        break;
    case 1:
        if (*(int *)(self + 0x200) & 0x1000) {
            self[7] = st + 1;
        }
        break;
    case 2:
        func_00174AC0(self, 0);
        if (self[0x23F] > 1) {
            self[7]++;
            func_0017C440(self, 0);
        } else {
            self[0x25C] = 0;
            func_0017C540(self);
        }
        break;
    case 3:
        func_00178B90(self, 0);
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            func_0017C540(self);
        }
        break;
    }
    func_001764E0(self);
    *(float *)(self + 0xB4) += -0.2f;
    func_00175900(self, 1);
    func_001796C0(self);
}
