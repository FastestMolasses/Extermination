// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008251C0 (splat/link name 00825180;
// overlay code is linked 0x40 below where it runs), 0xB4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 00825180, 008251C0 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: sub-state +5 (called as 0x8251C0): 0 starts script 0x8293A0 when
// +0xB bit 2 is set; 1 at script end sets clip 0 (blend 20) and clears
// +0xB/+5; then func_001BA580(self, +0xD) and func_001C64F0 1.0.
extern char D_overlay_AREA04_008293A0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);

void func_overlay_AREA04_00825180(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            func_001BA1A0(blk, D_overlay_AREA04_008293A0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            { int zi = 0; float z = (float)zi; func_001C67E0(self, 0, 20.0f, z); }
            self[0xB] = 0;
            self[5] = 0;
        }
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
}
