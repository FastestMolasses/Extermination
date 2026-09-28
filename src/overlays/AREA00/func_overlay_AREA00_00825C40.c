// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA00 overlay, runtime 0x00825C80 (splat/link name 00825C40; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 sets +0x30 = &D_002758A8, +8 = 3, +0 = 1; state 1 starts
//  script 0x82A720 on +0xB bit 2 and clears +0xB/+5 when it ends; state 3
//  calls func_001AFC10. Same code as 0x8261E0 and 0x8262D0 (other data).
extern int D_002758A8;
extern char D_overlay_AREA00_0082A720[];
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00825C40(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        *(int **)(self + 0x30) = &D_002758A8;
        self[8] = 3;
        self[0] = 1;
        self[4] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082A720);
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
        break;
    case 2:
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
