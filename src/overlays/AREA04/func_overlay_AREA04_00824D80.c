// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824DC0 (splat/link name 00824D80;
// overlay code is linked 0x40 below where it runs), 0x130 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: func_001BA1C0(self, 0x14) -> state 3, else +0 = 1, state
// 1. State 1 when D_0081076C == 1: sub 0 starts script 0x828BE0, sub 1 at
// script end D_70003B8D = 3, D_0081076C = 0xFF, state 3. 2/3
// func_001AFC10.
extern unsigned char D_0081076C;
extern unsigned char D_70003B8D;
extern char D_overlay_AREA04_00828BE0[];
extern int func_001BA1C0(unsigned char *self, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00824D80(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x14) != 0) {
            self[4] = 3;
        } else {
            self[0] = 1;
            self[4] = 1;
        }
        break;
    case 1:
        switch (D_0081076C) {
        case 0:
            break;
        case 1:
            switch (self[5]) {
            case 0:
                func_001BA1A0(blk, D_overlay_AREA04_00828BE0);
                self[5] = 1;
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    D_70003B8D = 3;
                    D_0081076C = 0xFF;
                    self[4] = 3;
                }
                break;
            case 2:
                break;
            }
            break;
        case 0xFF:
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
