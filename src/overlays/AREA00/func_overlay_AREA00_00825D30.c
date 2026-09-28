// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825D70 (splat/link name 00825D30; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 goes to state 3 when func_001BA1C0(self, 0x2B) is set,
//  else stores func_001B6660(0x827EF0) in D_008106C0; state 1 runs 0x825E80
//  while D_00810803 is 0..2, otherwise 0x825FC0.
extern unsigned char D_00810803;
extern unsigned char *D_008106C0;
extern char D_overlay_AREA00_00827EF0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001B6660(void *p);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA00_00825E80(unsigned char *self);
extern void func_overlay_AREA00_00825FC0(unsigned char *self);

void func_overlay_AREA00_00825D30(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2B) != 0) {
            self[4] = 3;
            break;
        }
        func_001B0FD0(self);
        func_001C6380(self);
        D_008106C0 = func_001B6660(D_overlay_AREA00_00827EF0);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (D_00810803) {
        case 0:
        case 1:
        case 2:
            func_overlay_AREA00_00825E80(self);
            break;
        default:
            func_overlay_AREA00_00825FC0(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
