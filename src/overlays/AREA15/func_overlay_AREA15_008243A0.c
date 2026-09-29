// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008243E0 (splat/link name 008243A0; overlay code is
// linked 0x40 below where it runs), 0x124 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 008243A0, 008243E0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x8243E0) sub-state +5: 0 waits for bit 2 of +0xB and starts
//  script 0x8287B0 (+0xD 0x50) or 0x828670, +5 = 1; 1 at script end calls
//  func_001C67E0(self, 9 or 0, 20.0, 0.0) by the same +0xD test, clears +0xB
//  and +5. Then func_001BA580(self, +0xD), func_001C64F0(1.0), func_001B17A0,
//  func_001C68C0 and the +0x4C method.
extern char D_overlay_AREA15_008287B0[];
extern char D_overlay_AREA15_00828670[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA15_008243A0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            if (self[0xD] == 0x50) {
                func_001BA1A0(blk, D_overlay_AREA15_008287B0);
            } else {
                func_001BA1A0(blk, D_overlay_AREA15_00828670);
            }
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            if (self[0xD] == 0x50) {
                func_001C67E0(self, 9, 20.0f, 0.0f);
                self[0xB] = 0;
            } else {
                func_001C67E0(self, 0, 20.0f, 0.0f);
                self[0xB] = 0;
            }
            self[5] = 0;
        }
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
    func_001B17A0(self);
    func_001C68C0(self);
    (*(void (**)(unsigned char *))(self + 0x4C))(self);
}
