// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00825030 (splat/link name 00824FF0; overlay code is
// linked 0x40 below where it runs), 0x294 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824FF0, 00825030 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x825030) state 0: state 3 when func_001BA1C0(self, 0x2C) is
//  set or 0x26 is clear; otherwise func_001B10B0(.., 0x52),
//  func_001C63E0(self, 7 when D_00810804 else 4), func_001CA6F0(self, 2), +0 =
//  1, +0x30 = 0x8297F0, +0x58 = D_0028A5DC, state 1. State 1 sub-state +5: 0
//  with D_00810804 set places the object at (975.5, 240, 902.5), +0xC4 = 0,
//  func_001C67E0(self, 7, 0, 0), +5 = 1, and animates only while D_00810784 ==
//  0; 1 waits for bit 2 of +0xB, script 0x8296F0, +5 = 2; 2 at script end
//  func_001C67E0(self, 7, 20.0, 0.0), +0xB = 0, +5 = 1; 1 and 2 animate
//  (func_001C64F0(1.0), func_001B17A0, func_001C68C0, +0x4C). States 2/3
//  func_001AFC10.
extern unsigned char D_00810784;
extern unsigned char D_00810804;
extern int D_0028A5DC;
extern char D_overlay_AREA15_008297F0[];
extern char D_overlay_AREA15_008296F0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00824FF0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2C) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 0x26) == 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x52);
        if (D_00810804 != 0) {
            func_001C63E0(self, 7);
        } else {
            func_001C63E0(self, 4);
        }
        func_001CA6F0(self, 2);
        self[0] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA15_008297F0;
        *(int *)(self + 0x58) = D_0028A5DC;
        self[4] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810804 != 0) {
                *(float *)(self + 0xB0) = 975.5f;
                *(float *)(self + 0xB4) = 240.0f;
                *(float *)(self + 0xB8) = 902.5f;
                *(float *)(self + 0xC4) = 0.0f;
                func_001C67E0(self, 7, 0.0f, 0.0f);
                self[5] = 1;
            }
            if (D_00810784 == 0) {
                func_001C64F0(self, 1.0f);
                func_001B17A0(self);
                func_001C68C0(self);
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
            break;
        case 1:
            if (self[0xB] & 4) {
                func_001BA1A0(blk, D_overlay_AREA15_008296F0);
                self[5] = 2;
            }
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            break;
        case 2:
            if (func_001BA1F0(self) != 0) {
                func_001C67E0(self, 7, 20.0f, 0.0f);
                self[0xB] = 0;
                self[5] = 1;
            }
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
