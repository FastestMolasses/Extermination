// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sub-mode 3 handler: self+0x23F = 2, self+0x24C = 1, func_001662D0(self).
// While bit 12 of self+0x200 is set, self+0x268 += 1.0; once it reaches 4.0 the
// byte 0x70003B8D = 0 and the object enters state 1 sub-mode 12 (step 0,
// self+0x1F0 = 0x17).
extern unsigned char D_70003B8D;
extern void func_001662D0(unsigned char *self);

void func_00183440(unsigned char *self) {
    self[0x23F] = 2;
    *(int *)(self + 0x24C) = 1;
    func_001662D0(self);
    if (*(int *)(self + 0x200) & 0x1000) {
        *(float *)(self + 0x268) += 1.0f;
        if (!(*(float *)(self + 0x268) < 4.0f)) {
            D_70003B8D = 0;
            self[4] = 1;
            self[5] = 0xC;
            self[6] = 0;
            self[0x1F0] = 0x17;
        }
    }
}
