// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824320 (splat/link name 008242E0;
// overlay code is linked 0x40 below where it runs), 0x170 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: func_001BA1C0(self, 0x1A) -> state 3, D_008107E9 =
// 0xFF; else (+0xC4 = -pi/2 when D_008107E9 == 1) model setup
// func_001B10B0(self, +0xD, 0x52), clip 0, func_001CA6F0(self, 2),
// func_001BA8E0(self, +0xD), +0x58 = D_0028A5DC, +0x30 = 0x828210, state
// 1, +0 = 1. State 1 runs 0x824490 (D_008107E9 0) or 0x8245F0 (1), then
// func_001C68C0, func_001B17A0, the +0x4C method. 2/3 func_001BA540 +
// func_001AFC10.
extern unsigned char D_008107E9;
extern int D_0028A5DC;
extern char D_overlay_AREA04_00828210[];
extern int func_001BA1C0(unsigned char *self, int n);
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001CA6F0(unsigned char *self, int a1);
extern void func_001BA8E0(unsigned char *self, int a1);
extern void func_overlay_AREA04_00824490(unsigned char *self);
extern void func_overlay_AREA04_008245F0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_008242E0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x1A) != 0) {
            self[4] = 3;
            D_008107E9 = 0xFF;
        } else {
            if (D_008107E9 == 1) {
                *(float *)(self + 0xC4) = -1.5707964f;
            }
            func_001B10B0(self, self[0xD], 0x52);
            func_001C63E0(self, 0);
            func_001CA6F0(self, 2);
            func_001BA8E0(self, self[0xD]);
            *(int *)(self + 0x58) = D_0028A5DC;
            *(char **)(self + 0x30) = D_overlay_AREA04_00828210;
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (D_008107E9) {
        case 0:
            func_overlay_AREA04_00824490(self);
            break;
        case 1:
            func_overlay_AREA04_008245F0(self);
            break;
        }
        func_001C68C0(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
