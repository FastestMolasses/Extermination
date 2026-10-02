// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008250E0 (splat/link name 008250A0; overlay code
//  is linked 0x40 below where it runs), 0xCC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [41]. State 1: flag 0x2F sets state 3; func_001B1B70
//  and the +0x4C method unless D_70003B92 and D_00810787 are both set. State
//  3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810787;
extern unsigned char D_70003B92[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_008250A0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        if (func_001BA1C0(self, 0x2F) != 0) {
            self[4] = 3;
        }
        if (D_70003B92[0] == 0 || D_00810787 == 0) {
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
