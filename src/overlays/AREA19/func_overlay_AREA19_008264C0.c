// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826500 (splat/link name 008264C0; overlay code
//  is linked 0x40 below where it runs), 0x3C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: script callback (script 0x82C6E0, record 0x82C7A0): +0x2EC =
//  func_001FBD50(self, 0x135, 0, 300.0); returns 1.
extern int func_001FBD50(unsigned char *self, int id, int a2, float vol);

int func_overlay_AREA19_008264C0(unsigned char *self) {
    *(int *)(self + 0x2EC) = func_001FBD50(self, 0x135, 0, 300.0f);
    return 1;
}
