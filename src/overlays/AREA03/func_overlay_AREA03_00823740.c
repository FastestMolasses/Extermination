// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00823780 (splat/link name 00823740; overlay code
//  is linked 0x40 below where it runs), 0x84 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Covers the splat pieces 00823740, 00823780 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: state-0 step of 0x823810 (called with self). +0x60/64/68 = 1.0;
//  returns 1 when func_001B1020(self, +0xD, -1, 0) is nonzero; otherwise
//  func_001C6380, +0 = 1, +8 = 3, +0x30 = &D_00275908 (gp-relative),
//  func_001F1110(self, 0) and returns 0.
extern int D_00275908;
extern int func_001B1020(unsigned char *self, int id, int a2, int a3);
extern void func_001C6380(unsigned char *self);
extern void func_001F1110(unsigned char *self, int a);

int func_overlay_AREA03_00823740(unsigned char *self) {
    *(float *)(self + 0x68) = 1.0f;
    *(float *)(self + 0x64) = 1.0f;
    *(float *)(self + 0x60) = 1.0f;
    if (func_001B1020(self, self[0xD], -1, 0) != 0) {
        return 1;
    }
    func_001C6380(self);
    self[0] = 1;
    self[8] = 3;
    *(int **)(self + 0x30) = &D_00275908;
    func_001F1110(self, 0);
    return 0;
}
