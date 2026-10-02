// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00825470 (splat/link name 00825430; overlay code
//  is linked 0x40 below where it runs), 0xE4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [38] (the AREA11 request). State 0: state 1, +0 = 1.
//  State 1: +5 0 script 0x8284F0 once D_00810835 is set; +5 1 at its end
//  func_001B0C60(0xB, 0, 3) (AREA11 entry 3) and state 3. func_001B17A0 each
//  frame. State 3 func_001AFC10.
extern unsigned char D_00810835;
extern char D_overlay_AREA17_008284F0[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B0C60(int area, int sub, int entry);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00825430(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810835 != 0) {
                func_001BA1A0(talk, D_overlay_AREA17_008284F0);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001B0C60(0xB, 0, 3);
                self[4] = 3;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
