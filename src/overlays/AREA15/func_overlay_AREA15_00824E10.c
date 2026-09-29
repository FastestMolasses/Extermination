// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824E50 (splat/link name 00824E10; overlay code is
// linked 0x40 below where it runs), 0x1E0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824E10, 00824E50 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824E50) state 0: state 3 when func_001BA1C0(self, 0x2C) is
//  set or 0x26 is clear; otherwise func_001B10B0(.., 0x5B),
//  func_001C63E0(self, 2), func_001CA6F0(self, 2), +0x58 = D_0028A600, state
//  1, +0 = 1. State 1 sub-state +5: 0 waits for D_00810702 == 1 and D_00810784
//  == 0, script 0x829330, +5 = 1; 1 at script end func_001C4760(0x15, 1),
//  func_001AEE10(4, 0), D_00810804 = 1, +0x2E = 0xFFFF, +5 = 2,
//  func_001FAE70(0); 2 nothing. While D_00810804 != 0: func_001C64F0(1.0),
//  func_001B17A0, func_001C68C0, +1 = 1, +0x4C method. States 2/3
//  func_001AFC10.
extern unsigned char D_00810702;
extern unsigned char D_00810784;
extern unsigned char D_00810804;
extern int D_0028A600;
extern char D_overlay_AREA15_00829330[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int a0, int a1);
extern void func_001AEE10(int a0, int a1);
extern void func_001FAE70(int a);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00824E10(unsigned char *self) {
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
        func_001B10B0(self, self[0xD], 0x5B);
        func_001C63E0(self, 2);
        func_001CA6F0(self, 2);
        *(int *)(self + 0x58) = D_0028A600;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810702 == 1 && D_00810784 == 0) {
                func_001BA1A0(blk, D_overlay_AREA15_00829330);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001C4760(0x15, 1);
                func_001AEE10(4, 0);
                D_00810804 = 1;
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[5] = 2;
                func_001FAE70(0);
            }
            break;
        case 2:
            break;
        }
        if (D_00810804 != 0) {
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            self[1] = 1;
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
