// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00825D60 (splat/link name 00825D20;
// overlay code is linked 0x40 below where it runs), 0x90 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 func_001B0FD0 == 0 -> func_001C6380, state 1, +0 = 1;
// state 1 runs the +0x4C method; other states func_001AFC10.
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00825D20(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
