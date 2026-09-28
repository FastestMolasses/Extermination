// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00824E40 (splat/link name 00824E00; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: sets block floats +0xD8 = 0.6 and +0xEC = 2.6, starts clip 6 with
//  func_001C67E0(self, 6, 6.0, 0.0), sets short +0xD0 = 240; returns 1.
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);

int func_overlay_AREA00_00824E00(unsigned char *self) {
    unsigned char *anim = self + 0x1F0;
    *(float *)(anim + 0xD8) = 0.6f;
    *(float *)(anim + 0xEC) = 2.6f;
    func_001C67E0(self, 6, 6.0f, 0.0f);
    *(short *)(anim + 0xD0) = 0xF0;
    return 1;
}
