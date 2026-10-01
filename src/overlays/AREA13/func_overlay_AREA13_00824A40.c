// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824A80 (splat/link name 00824A40; overlay code
//  is linked 0x40 below where it runs), 0x12C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placement [7]. State 0: state 3 when func_001BA1C0(self, 0x42)
//  is set, else state 1, +0 = 1. +5 0: script 0x82C510 and D_0081081A = 0xFF
//  when func_001B1EA0(0, D_00810350, 0x82E220, 4) is set; +5 1:
//  func_001B6660(0x82A230) and state 3 when the script ends. States 2/3
//  func_001AFC10.
extern float D_00810350[];
extern char D_overlay_AREA13_0082E220[];
extern char D_overlay_AREA13_0082C510[];
extern char D_overlay_AREA13_0082A230[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void *func_001B6660(void *p);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00824A40(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x42) != 0) {
            self[4] = 3;
            break;
        }
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA13_0082E220, 4) != 0) {
                func_001BA1A0(talk, D_overlay_AREA13_0082C510);
                self[5] = 1;
                *(unsigned char *)0x81081A = 0xFF;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001B6660(D_overlay_AREA13_0082A230);
                self[4] = 3;
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
