// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825260 (splat/link name 00825220; overlay code is
// linked 0x40 below where it runs), 0xAC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: (runtime 0x825260, the behaviour stored by 0x824690) state 0: once
//  func_001B10B0(self, 0x68, 0x69) is clear func_001C63E0(self, 0) and state
//  1. State 1: func_001C68C0 and the +0x4C method when func_001B17A0. State
//  3/other func_001AFC10.
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00825220(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B10B0(self, 0x68, 0x69) == 0) {
            func_001C63E0(self, 0);
            self[4] = 1;
        }
        break;
    case 1:
        func_001C68C0(self);
        if (func_001B17A0(self) != 0) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
