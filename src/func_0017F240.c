// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Grab test. Unless self+0x224 and self+0x22C are both 0.0 and bit 1 of
// self+0xF is clear: with held == 0, or bit 12 of self+0x200 set, the object
// enters state 2 sub-mode 6 (old sub-mode saved in self+0x302, step 0) and the
// result is 1; otherwise, when self+0x3C is 3.0 or more, self+0x204 = self+0x3C
// - 2.0. Returns 0 in every other case.
int func_0017F240(unsigned char *self, int held) {
    float z;
    float h;
    if (*(float *)(self + 0x224) != (z = 0.0f) || *(float *)(self + 0x22C) != z || (self[0xF] & 2)) {
        if (held == 0) {
            self[0x302] = self[5];
            self[4] = 2;
            self[5] = 6;
            self[6] = 0;
            return 1;
        }
        if (*(int *)(self + 0x200) & 0x1000) {
            self[0x302] = self[5];
            self[4] = 2;
            self[5] = 6;
            self[6] = 0;
            return 1;
        }
        h = *(float *)(self + 0x3C);
        if (!(h < 3.0f)) {
            *(float *)(self + 0x204) = h - 2.0f;
        }
    }
    return 0;
}
