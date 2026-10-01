// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008237E0 (splat/link name 008237A0; overlay code is
// linked 0x40 below where it runs), 0x130 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: talk owner. State 0: inert (state 3) when func_001BA1C0(self, 0)
// is set; else func_001B10B0(self, +0xD, 0x4A), func_001C63E0(self, 8),
// func_001CA6F0(self, 2), func_001BA8E0(self, +0xD), +0x30 = 0x828BD0,
// +0x58 = D_0028A5C4, state 1 and +0 = 1. State 1 by D_008107D8: 0 runs
// 0x823910, bit 7 set runs 0x823C40, otherwise 0x823B70. States 2/3:
// func_001BA540 and func_001AFC10.
extern unsigned char D_008107D8;
extern int D_0028A5C4;
extern char D_overlay_AREA11_00828BD0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001CA6F0(unsigned char *self, int a1);
extern void func_001BA8E0(unsigned char *self, int type);
extern void func_overlay_AREA11_00823910(unsigned char *self);
extern void func_overlay_AREA11_00823B70(unsigned char *self);
extern void func_overlay_AREA11_00823C40(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_008237A0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x4A);
        func_001C63E0(self, 8);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        *(char **)(self + 0x30) = D_overlay_AREA11_00828BD0;
        *(int *)(self + 0x58) = D_0028A5C4;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (D_008107D8 == 0) {
            func_overlay_AREA11_00823910(self);
        } else if (D_008107D8 & 0x80) {
            func_overlay_AREA11_00823C40(self);
        } else {
            func_overlay_AREA11_00823B70(self);
        }
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
