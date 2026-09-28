// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008245F0 (splat/link name 008245B0;
// overlay code is linked 0x40 below where it runs), 0xBC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 008245B0, 008245F0 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: sub-state +5 (called as 0x8245F0 from 0x824320): 0 starts script
// 0x8280D0 when +0xB bit 2 is set; 1 at script end clears +0xB/+5 and
// sets clip 0 (blend 20); then func_001BA580(self, +0xD) and
// func_001C64F0 1.0.
extern char D_overlay_AREA04_008280D0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);

void func_overlay_AREA04_008245B0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            func_001BA1A0(blk, D_overlay_AREA04_008280D0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[5] = 2;
            self[0xB] = 0;
            self[5] = 0;
            func_001C67E0(self, 0, 20.0f, 0.0f);
        }
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
}
