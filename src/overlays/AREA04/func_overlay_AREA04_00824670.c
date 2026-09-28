// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008246B0 (splat/link name 00824670;
// overlay code is linked 0x40 below where it runs), 0x178 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: func_001BA1C0(self, 0x12) -> state 3; else
// func_001B10B0(self, +0xD, 0x6D), clip 0, +0x58 = D_0028A6F8, state 1,
// +0 = 1. State 1 while D_0081076A: D_008107EA 0/0x10 -> 0x824830,
// 1/2/0x20/0xFF -> 0x824930; nonzero D_008107EA then animates (1.0) and
// runs the +0x4C method. 2/3 func_001AFC10.
extern unsigned char D_0081076A;
extern unsigned char D_008107EA;
extern int D_0028A6F8;
extern int func_001BA1C0(unsigned char *self, int n);
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_overlay_AREA04_00824830(unsigned char *self);
extern void func_overlay_AREA04_00824930(unsigned char *self);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00824670(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x12) != 0) {
            self[4] = 3;
        } else {
            func_001B10B0(self, self[0xD], 0x6D);
            func_001C63E0(self, 0);
            *(int *)(self + 0x58) = D_0028A6F8;
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        if (D_0081076A != 0) {
            switch (D_008107EA) {
            case 0:
            case 0x10:
                func_overlay_AREA04_00824830(self);
                break;
            case 1:
            case 2:
            case 0x20:
            case 0xFF:
                func_overlay_AREA04_00824930(self);
                break;
            }
            if (D_008107EA != 0) {
                func_001C64F0(self, 1.0f);
                func_001B17A0(self);
                func_001C68C0(self);
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
