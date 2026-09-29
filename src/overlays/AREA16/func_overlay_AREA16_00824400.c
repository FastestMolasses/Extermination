// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00824440 (splat/link name 00824400; overlay code is
// linked 0x40 below where it runs), 0xF8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: state 3 when func_001BA1C0(self, 0x31) is set; else once
//  func_001B0FD0 is clear func_001C6380, 0x825660 (+0x2E0 = -1), state 1, +0 =
//  1. State 1 by D_00810809: 0 -> 0x824540, 0xFF -> state 3, 1 nothing. States
//  2/3 func_001AFC10.
extern unsigned char D_00810809;
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA16_00825660(unsigned char *self);
extern void func_overlay_AREA16_00824540(unsigned char *self);

void func_overlay_AREA16_00824400(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x31) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            func_overlay_AREA16_00825660(self);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (D_00810809) {
        case 0:
            func_overlay_AREA16_00824540(self);
            break;
        case 0xFF:
            self[4] = 3;
            break;
        case 1:
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
