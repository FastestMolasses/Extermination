// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Starts a clip by two flags: with self+0x2F1 == 1,
// clip 0xC8 (0xC9 when self+0x25C is 3); otherwise 0xC1 (0xC2 when self+0x25C is
// 3); each through func_001749A0(self, clip, 0, 1.0).
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);

void func_00181430(unsigned char *self) {
    if (self[0x2F1] == 1) {
        if (self[0x25C] != 3) {
            func_001749A0(self, 0xC8, 0, 1.0f);
        } else {
            func_001749A0(self, 0xC9, 0, 1.0f);
        }
    } else if (self[0x25C] != 3) {
        func_001749A0(self, 0xC1, 0, 1.0f);
    } else {
        func_001749A0(self, 0xC2, 0, 1.0f);
    }
}
