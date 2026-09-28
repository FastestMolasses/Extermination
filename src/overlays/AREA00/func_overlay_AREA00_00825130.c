// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825170 (splat/link name 00825130; overlay code is
// linked 0x40 below where it runs), 0x270 bytes. Byte-identical C, linked from
// its compiled object: the text and the jump table (7 entries at runtime
// 0x0082D420) match the original. The compiled .rodata is placed by
// tools/overlay/jt_pin.py at the table's original address, with the table
// address and its case-label entries resolved at runtime addresses (link +
// 0x40), which is what the original stores.
// Role: owner with state +4. 0: func_001BBDA0, script 0x8294E0 unless
//  D_0081075B is 0xFF, +0 = 1. 1: talk program on +5 through a jump table (+5 0
//  branches on D_00810702 == 5 / D_0081075B / +0xB bit 2; +5 1 starts script
//  0x8299A0; +5 6 at script end sets D_0081075B = 1 with func_001C4760(4, 1) the
//  first time and restarts script 0x8294E0), then func_001BC300.
extern unsigned char D_0081075B;
extern unsigned char D_008107DB;
extern unsigned char D_00810702;
extern char D_overlay_AREA00_008294E0[];
extern char D_overlay_AREA00_008299A0[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern int func_001C4760(int a0, int a1);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00825130(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        if (D_0081075B != 0xFF) {
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008294E0);
        }
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810702 == 5) {
                D_0081075B = 0xFF;
                if (func_001BBE40(self, talk, 0)) {
                    D_008107DB = 0xFF;
                    self[5] = 3;
                }
            } else if (D_0081075B != 0xFF) {
                if (self[0xB] & 4) {
                    self[5] = 6;
                }
            } else if (func_001BBE40(self, talk, 0)) {
                self[5] = 3;
            }
            break;
        case 1:
            if (func_001BC0E0(self, talk)) {
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008299A0);
                self[5]++;
            }
            break;
        case 2:
            if (func_001BC0E0(self, talk)) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        case 3:
            if (func_001BC0E0(self, talk)) {
                self[5]++;
            }
            break;
        case 4:
            func_001BC240(self, talk);
            self[5]++;
            break;
        case 5:
            if (func_001BC290(self, talk)) {
                self[5] = 0;
            }
            break;
        case 6:
            if (func_001BA1F0(self)) {
                if (D_0081075B == 0) {
                    D_0081075B = 1;
                    func_001C4760(4, 1);
                }
                self[5] = 0;
                self[0xB] = 0;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008294E0);
            }
            break;
        }
        func_001BC300(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
