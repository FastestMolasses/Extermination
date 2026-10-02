// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00826170 (splat/link name 00826130; overlay code
//  is linked 0x40 below where it runs), 0xBC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [37]. State 0: func_001C6380, +0 = 1 after
//  func_001B0FD0. State 1: once flag 0x2E is set func_001C6380, func_001B1B70
//  and the +0x4C method. States 2 / 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00826130(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
        }
        break;
    case 1:
        if (func_001BA1C0(self, 0x2E) != 0) {
            func_001C6380(self);
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
