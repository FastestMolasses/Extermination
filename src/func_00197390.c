// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.30% NEARMISS).
// Fixes read from the original instructions:
//  - The trailing test on other's state at +0x230 is a switch (cases that do
//    nothing, default clears), not an && chain; that also restored the
//    func_00102948 argument-setup order. mwcc emits the case compares in
//    reverse source order, so the cases are listed 0x2C, 8, 9.
//  - func_00102948 declared (void *, void *) like its other callers.
//
// Spawn/place helper. Calls func_001916C0(self, other, 0), then copies three
// floats from per-type tables indexed by the global type byte D_00810702: row
// stride is 12 bytes (idx*3 words), D_0024A6C8[idx*3] -> self+0x10,
// D_0024A6CC[idx*3] -> self+0x14, D_0024A6D0[idx*3] -> self+0x18 (an x/y/z or
// offset vector). Calls func_0018D7B0(self, 5), then
// func_00102948(&D_008105D0, self+0x10) (apply the vector via a matrix/xform).
// Finally, unless other's state at +0x230 is one of 9 / 8 / 0x2C, it clears the
// bytes at self+6, self+1, self+2.
extern void func_001916C0(char *self, char *other, int c);
extern float D_0024A6C8[];
extern float D_0024A6CC[];
extern float D_0024A6D0[];
extern unsigned char D_00810702;
extern void func_0018D7B0(char *self, int n);
extern void func_00102948(void *, void *);
extern float D_008105D0;

void func_00197390(char *self, char *other) {
    func_001916C0(self, other, 0);
    *(float *)(self + 0x10) = D_0024A6C8[D_00810702 * 3];
    *(float *)(self + 0x14) = D_0024A6CC[D_00810702 * 3];
    *(float *)(self + 0x18) = D_0024A6D0[D_00810702 * 3];
    func_0018D7B0(self, 5);
    func_00102948(&D_008105D0, self + 0x10);
    switch (*(int *)(other + 0x230)) {
    case 0x2C:
    case 8:
    case 9:
        break;
    default:
        self[6] = 0;
        self[1] = 0;
        self[2] = 0;
        break;
    }
}
