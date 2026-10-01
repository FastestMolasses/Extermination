// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00826FF0 (splat/link name 00826FB0; overlay code
//  is linked 0x40 below where it runs), 0x158 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [58] / [59]. State 0: when func_001B0FD0 returns 0:
//  func_001C6380, +0x30 = &D_002759C0, +8 = 3, +0 = 1, state 1. +5 0 on +0xB
//  bit 2 starts script 0x82CE10 (D_00810350 < 800) or 0x82CFD0; +5 1 clears
//  +0xB / +5 when it ends. Then func_001B17A0 and the +0x4C method. States
//  2/3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int D_002759C0;
extern char D_overlay_AREA13_0082CE10[];
extern char D_overlay_AREA13_0082CFD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00826FB0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            *(int **)(self + 0x30) = &D_002759C0;
            self[8] = 3;
            self[0] = 1;
            self[4] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (*(float *)0x810350 < 800.0f) {
                    func_001BA1A0(talk, D_overlay_AREA13_0082CE10);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA13_0082CFD0);
                }
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[0xB] = 0;
                self[5] = 0;
            }
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
