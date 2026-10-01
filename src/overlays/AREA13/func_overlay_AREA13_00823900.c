// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823940 (splat/link name 00823900; overlay code
//  is linked 0x40 below where it runs), 0xD0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 00823900, 00823940 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x823700 (D_008107F1 == 1). +5 0 starts script 0x82A620
//  when +0xB bit 2 is set; +5 1 clears +0xB / +5 and calls
//  func_001C67E0(self, 0, 20.0, 0.0) when it ends. Every frame:
//  func_001BA580(self, +0xD), func_001C64F0(self, 1.0), func_001B17A0,
//  func_001C68C0 and the +0x4C method.
extern char D_overlay_AREA13_0082A620[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001BA580(unsigned char *self, int id);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA13_00823900(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            func_001BA1A0(blk, D_overlay_AREA13_0082A620);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[0xB] = 0;
            self[5] = 0;
            func_001C67E0(self, 0, 20.0f, 0.0f);
        }
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
    func_001B17A0(self);
    func_001C68C0(self);
    (*(void (**)(unsigned char *))(self + 0x4C))(self);
}
