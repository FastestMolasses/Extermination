// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008299E0 (splat/link name 008299A0; overlay code
//  is linked 0x40 below where it runs), 0xB8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: deferred group 0x829D00 member ([11]): state 0 when
//  func_0015AC00(self, blk) returns 0: +0x2EC = 1, +0x2E8 = +0xB4, +0x28 =
//  0x71C; state 1 func_0015AE20(self, blk) while D_00810774 == 0; other
//  states func_001B1190(+0x9A) and func_001AFC10.
extern unsigned char D_00810774;
extern int func_0015AC00(unsigned char *self, unsigned char *blk);
extern void func_0015AE20(unsigned char *self, unsigned char *blk);
extern void func_001B1190(int uid);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_008299A0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_0015AC00(self, blk) == 0) {
            *(int *)(self + 0x2EC) = 1;
            *(float *)(self + 0x2E8) = *(float *)(self + 0xB4);
            *(short *)(self + 0x28) = 0x71C;
        }
        break;
    case 1:
        if (D_00810774 == 0) {
            func_0015AE20(self, blk);
        }
        break;
    case 2:
    case 3:
    default:
        func_001B1190(self[0x9A]);
        func_001AFC10(self);
        break;
    }
}
