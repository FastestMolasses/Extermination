// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA03 overlay, runtime 0x008268B0 (splat/link name 00826870; overlay code
//  is linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: script callback (script 0x8282D0 op09 record 0x828950; script
//  0x828CD0 op09 record 0x828E10): func_001FBD50(self, 0x3F7, 0, 300.0);
//  returns 1.
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

int func_overlay_AREA03_00826870(unsigned char *self) {
    func_001FBD50(self, 0x3F7, 0, 300.0f);
    return 1;
}
