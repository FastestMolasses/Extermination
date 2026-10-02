// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008260C0 (splat/link name 00826080; overlay code
//  is linked 0x40 below where it runs), 0xA4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 deferred group 0x826280 member: state 0 func_001C6380 after
//  func_001B1020(self, +0xD, -1, 0); state 1 the +0x4C method, state 3 once
//  D_00810787 is set; others func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810787;
extern int func_001B1020(unsigned char *self, int a1, int a2, int a3);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00826080(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B1020(self, self[0xD], -1, 0) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        if (D_00810787 != 0) {
            self[4] = 3;
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
