// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA03 overlay, runtime 0x00826880 (splat/link name 00826840; overlay code
//  is linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: script callback (script 0x8282D0 op09 records 0x8283D0, 0x828A50;
//  script 0x828CD0 op09 record 0x828ED0): func_001FBD50(self, 0x40B, 0,
//  300.0); returns 1.
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

int func_overlay_AREA03_00826840(unsigned char *self) {
    func_001FBD50(self, 0x40B, 0, 300.0f);
    return 1;
}
