// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008242D0 (splat/link name 00824290; overlay code
//  is linked 0x40 below where it runs), 0x104 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [39]. State 0: func_001C6380 after func_001B0FD0.
//  State 1: +0x28 counts to 180; below that the +0x4C method (when
//  func_001B17A0) runs only with flag 0x2D, D_00810805 or +0x28 above 120.
//  State 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_00810805;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00824290(unsigned char *self) {
    short n;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        n = S16(0x28);
        if (n < 180) {
            S16(0x28) = n + 1;
            if (func_001BA1C0(self, 0x2D) == 0 && D_00810805 == 0 && S16(0x28) <= 120) {
                break;
            }
            if (func_001B17A0(self) != 0) {
                (*(ActorFn *)(self + 0x4C))(self);
            }
        } else {
            if (func_001B17A0(self) != 0) {
                (*(ActorFn *)(self + 0x4C))(self);
            }
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
