// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA00 overlay, runtime 0x008262D0 (splat/link name 00826290; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: same code as 0x825C80 with +0x30 = &D_002758B8 and script
//  0x82B990.
extern int D_002758B8;
extern char D_overlay_AREA00_0082B990[];
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826290(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        *(int **)(self + 0x30) = &D_002758B8;
        self[8] = 3;
        self[0] = 1;
        self[4] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082B990);
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
