// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00826FC0 (splat/link name 00826F80; overlay code
//  is linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: script callback (script 0x82CA50, record 0x82CC90):
//  func_001FBD50(self, 0x40D, 0, 300.0); returns 1.
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

int func_overlay_AREA13_00826F80(unsigned char *self) {
    func_001FBD50(self, 0x40D, 0, 300.0f);
    return 1;
}
