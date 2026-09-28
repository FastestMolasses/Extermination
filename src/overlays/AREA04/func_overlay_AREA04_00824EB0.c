// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824EF0 (splat/link name 00824EB0;
// overlay code is linked 0x40 below where it runs), 0x14C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: func_001BA1C0(self, 0x28) -> state 3; else
// func_001B10B0(self, +0xD, 0x5B), clip 0, func_001CA6F0(self, 2),
// func_001BA8E0(self, +0xD), +0x58 = D_0028A600, +0x30 = 0x829520, state
// 1, +0 = 1. State 1 runs 0x825040 (D_00810800 0) or 0x8251C0 (1), then
// func_001C68C0, func_001B17A0, +1 = 1, the +0x4C method. 2/3
// func_001BA540 + func_001AFC10.
extern unsigned char D_00810800;
extern int D_0028A600;
extern char D_overlay_AREA04_00829520[];
extern int func_001BA1C0(unsigned char *self, int n);
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001CA6F0(unsigned char *self, int a1);
extern void func_001BA8E0(unsigned char *self, int a1);
extern void func_overlay_AREA04_00825040(unsigned char *self);
extern void func_overlay_AREA04_008251C0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00824EB0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x28) != 0) {
            self[4] = 3;
        } else {
            func_001B10B0(self, self[0xD], 0x5B);
            func_001C63E0(self, 0);
            func_001CA6F0(self, 2);
            func_001BA8E0(self, self[0xD]);
            *(int *)(self + 0x58) = D_0028A600;
            *(char **)(self + 0x30) = D_overlay_AREA04_00829520;
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (D_00810800) {
        case 0:
            func_overlay_AREA04_00825040(self);
            break;
        case 1:
            func_overlay_AREA04_008251C0(self);
            break;
        }
        func_001C68C0(self);
        func_001B17A0(self);
        self[1] = 1;
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
