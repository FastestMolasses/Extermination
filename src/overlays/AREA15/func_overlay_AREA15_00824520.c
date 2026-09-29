// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824560 (splat/link name 00824520; overlay code is
// linked 0x40 below where it runs), 0x244 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824520, 00824560 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824560) state 0: state 3 when func_001BA1C0(self, 0x25) is
//  set or both 0x28 and 0x29 are clear; otherwise func_001B10B0(.., 0x65),
//  func_001C63E0(self, 3), func_001CA6F0(self, 2), func_001BA8E0, +0x58 =
//  D_0028A6E8, state 1, +0x30 = 0x828BA0, +0 = 1. State 1 sub-state +5: 0
//  waits for bit 2 of +0xB, script 0x828900, +5 = 1, +0x28 = 0; 1 counts +0x28
//  and at 520 copies 0x828B80 / 0x828B90 to D_008105D0 / D_008105E0 and
//  D_008101F0 / D_00810200; at script end func_001C67E0(self, 3, 20.0, 0.0),
//  +0xB = +5 = 0. Then func_001BA580, func_001C64F0(1.0), func_001B17A0,
//  func_001C68C0, the +0x4C method. States 2/3 func_001BA540 and
//  func_001AFC10.
extern int D_0028A6E8;
extern char D_008105D0[];
extern char D_008105E0[];
extern char D_008101F0[];
extern char D_00810200[];
extern char D_overlay_AREA15_00828BA0[];
extern char D_overlay_AREA15_00828900[];
extern char D_overlay_AREA15_00828B80[];
extern char D_overlay_AREA15_00828B90[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00824520(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x25) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 0x28) == 0 && func_001BA1C0(self, 0x29) == 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x65);
        func_001C63E0(self, 3);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        *(int *)(self + 0x58) = D_0028A6E8;
        self[4] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA15_00828BA0;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(blk, D_overlay_AREA15_00828900);
                self[5] = 1;
                *(short *)(self + 0x28) = 0;
            }
            break;
        case 1:
            (*(short *)(self + 0x28))++;
            if (*(short *)(self + 0x28) == 0x208) {
                func_00102948(D_008105D0, D_overlay_AREA15_00828B80);
                func_00102948(D_008105E0, D_overlay_AREA15_00828B90);
                func_00102948(D_008101F0, D_overlay_AREA15_00828B80);
                func_00102948(D_00810200, D_overlay_AREA15_00828B90);
            }
            if (func_001BA1F0(self) != 0) {
                func_001C67E0(self, 3, 20.0f, 0.0f);
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        }
        func_001BA580(self, self[0xD]);
        func_001C64F0(self, 1.0f);
        func_001B17A0(self);
        func_001C68C0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
