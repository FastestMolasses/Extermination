// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x008266A0 (splat/link name 00826660; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 1: when D_0081075E is 0xFF and +0xD is 0x19, sets +0xD = 0x1A,
//  calls func_001AF800, func_001CB5B0(+9) and re-inits; animates; states 2,
//  3 and others call func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075E;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AF800(unsigned char *self);
extern void func_001CB5B0(int a0);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826660(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        break;
    case 1:
        if (D_0081075E == 0xFF) {
            if (self[0xD] == 0x19) {
                self[0xD] = 0x1A;
                func_001AF800(self);
                func_001CB5B0(self[9]);
                func_001B0FD0(self);
                func_001C6380(self);
                self[4] = 1;
            }
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
