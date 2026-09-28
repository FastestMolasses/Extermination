// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00826BE0 (splat/link name 00826BA0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 sets +0xD = 0x12 when D_00810784 is 0xFF and calls
//  func_001C6380 unless func_001B0FD0 is set; state 1 calls the +0x4C handler
//  unless D_00810804 is 2.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810784;
extern unsigned char D_00810804;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826BA0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (D_00810784 == 0xFF) {
            self[0xD] = 0x12;
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        if (D_00810804 != 2) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
