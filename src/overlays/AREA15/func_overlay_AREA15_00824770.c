// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008247B0 (splat/link name 00824770; overlay code is
// linked 0x40 below where it runs), 0x1D4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824770, 008247B0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x8247B0) state 0: state 3 when func_001BA1C0(self, 0x25) is
//  set or both 0x28 and 0x29 are clear; otherwise func_001B10B0(.., 0x52),
//  func_001C63E0(self, 4), +0x58 = D_0028A5DC, func_001CA6F0(self, 2),
//  func_001BA8E0, +0x30 = 0x828BB0, state 1, +0 = 1. State 1 sub-state +5: 0
//  waits for bit 2 of +0xB, script 0x828A40, +5 = 1; 1 at script end
//  func_001C67E0(self, 4, 20.0, 0.0), +0xB = +5 = 0. Then the common
//  func_001BA580 / func_001C64F0(1.0) / func_001B17A0 / func_001C68C0 / +0x4C
//  tail. States 2/3 func_001BA540 and func_001AFC10.
extern int D_0028A5DC;
extern char D_overlay_AREA15_00828BB0[];
extern char D_overlay_AREA15_00828A40[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001BA580(unsigned char *self, int a1);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00824770(unsigned char *self) {
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
        func_001B10B0(self, self[0xD], 0x52);
        func_001C63E0(self, 4);
        *(int *)(self + 0x58) = D_0028A5DC;
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        *(char **)(self + 0x30) = D_overlay_AREA15_00828BB0;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(blk, D_overlay_AREA15_00828A40);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001C67E0(self, 4, 20.0f, 0.0f);
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
