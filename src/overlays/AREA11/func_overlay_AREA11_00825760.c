// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008257A0 (splat/link name 00825760; overlay code is
// linked 0x40 below where it runs), 0x154 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: state 0: inert (state 3) when func_001BA1C0(self, 0x3C) or while
// D_00810788 is 0, else state 1 and +0 = 1. State 1: +5 0 waits for the
// player inside the area 0x82ACA0, starts script 0x829E80, +5 = 1; +5 1 at
// the script end calls func_001DFE40, state 3 and D_00810814 = 1. Then
// func_001B17A0. States 2/3: func_001AFC10.
extern unsigned char D_00810788;
extern unsigned char D_00810814;
extern char D_00810350[];
extern char D_overlay_AREA11_0082ACA0[];
extern char D_overlay_AREA11_00829E80[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a0, void *pos, void *poly, int n);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001DFE40(void);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_00825760(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x3C) != 0) {
            self[4] = 3;
            break;
        }
        if (D_00810788 == 0) {
            self[4] = 3;
            break;
        }
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA11_0082ACA0, 4) != 0) {
                func_001BA1A0(talk, D_overlay_AREA11_00829E80);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001DFE40();
                self[4] = 3;
                D_00810814 = 1;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
