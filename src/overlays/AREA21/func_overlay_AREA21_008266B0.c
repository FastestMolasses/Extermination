// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008266F0 (splat/link name 008266B0; overlay code
//  is linked 0x40 below where it runs), 0x148 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [45]. State 0: +5 = 0xFF when func_001BA1C0(self,
//  0x36); after func_001B0FD0: func_001C6380, state 1, +0 = 1. State 1: by
//  D_0081080E: 1 / 4 / 0x10 / 0x11 -> 0x826840, 2 / 3 / 0xFF -> 0x8268F0;
//  then func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081080E[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_overlay_AREA21_00826840(unsigned char *self);
extern void func_overlay_AREA21_008268F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_008266B0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x36) != 0) {
            self[5] = 0xFF;
        }
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (D_0081080E[0]) {
        case 0:
            break;
        case 1:
        case 4:
        case 0x10:
        case 0x11:
            func_overlay_AREA21_00826840(self);
            break;
        case 2:
        case 3:
        case 0xFF:
            func_overlay_AREA21_008268F0(self);
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
