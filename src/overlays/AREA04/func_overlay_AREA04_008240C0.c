// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824100 (splat/link name 008240C0;
// overlay code is linked 0x40 below where it runs), 0xE4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 func_001B10B0(self, 0x4E, 0x52) == 0 -> clip 0, state 1;
// state 1 animates (func_001C64F0 1.0, func_001C68C0, +0x4C method when
// func_001B17A0) and goes to state 3 when D_008107E4 == 2; 2/3
// func_001AFC10.
extern unsigned char D_008107E4;
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_008240C0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B10B0(self, 0x4E, 0x52) == 0) {
            func_001C63E0(self, 0);
            self[4] = self[4] + 1;
        }
        break;
    case 1:
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        if (func_001B17A0(self) != 0) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        if (D_008107E4 == 2) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
