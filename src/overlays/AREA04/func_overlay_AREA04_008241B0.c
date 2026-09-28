// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008241F0 (splat/link name 008241B0;
// overlay code is linked 0x40 below where it runs), 0x130 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: state 3 if func_001BA1C0(self, 0xD) or (self, 0x65),
// else +0 = 1 and state 1. State 1 while D_0081083D: sub 0 starts script
// 0x827D10, sub 1 at script end func_001B6660(0x826600), D_008107E5 =
// 0xFF, state 3. 2/3 func_001AFC10.
extern unsigned char D_0081083D;
extern unsigned char D_008107E5;
extern char D_overlay_AREA04_00827D10[];
extern char D_overlay_AREA04_00826600[];
extern int func_001BA1C0(unsigned char *self, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6660(char *p);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_008241B0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0xD) != 0) {
            self[4] = 3;
        } else if (func_001BA1C0(self, 0x65) != 0) {
            self[4] = 3;
        } else {
            self[0] = 1;
            self[4] = 1;
        }
        break;
    case 1:
        if (D_0081083D != 0) {
            switch (self[5]) {
            case 0:
                func_001BA1A0(blk, D_overlay_AREA04_00827D10);
                self[5] = 1;
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    func_001B6660(D_overlay_AREA04_00826600);
                    D_008107E5 = 0xFF;
                    self[4] = 3;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
