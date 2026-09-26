// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008261A0 (splat/link name 00826160; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0/sub-1 placement owner (O1 bridge halves); calls 0x826200 (+0xD
//  == 2) or 0x826440 (+0xD == 3).
extern void func_overlay_AREA01_00826200(unsigned char *self);
extern void func_overlay_AREA01_00826440(unsigned char *self);

void func_overlay_AREA01_00826160(unsigned char *self) {
    if (self[4] == 0) {
        self[0] = 1;
    }
    if (self[0xD] == 2) {
        func_overlay_AREA01_00826200(self);
    }
    if (self[0xD] == 3) {
        func_overlay_AREA01_00826440(self);
    }
}
