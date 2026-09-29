// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00826C20 (splat/link name 00826BE0; overlay code is
// linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: (self) plays func_001FBD50(self, 0x13D, 0, 300.0) and returns 1.
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

int func_overlay_AREA16_00826BE0(unsigned char *self) {
    func_001FBD50(self, 0x13D, 0, 300.0f);
    return 1;
}
