// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code is
// linked 0x40 below where it runs), 0x294 bytes. Byte-identical C, linked from
// its compiled object: the text and the jump table (8 entries at runtime
// 0x0082D400) match the original. The compiled .rodata is placed by
// tools/overlay/jt_pin.py at the table's original address, with the table
// address and its case-label entries resolved at runtime addresses (link +
// 0x40), which is what the original stores.
// Role: owner with state +4. 0: func_001BBDA0; +0 = 1, or (D_0081075A not 0xFF)
//  +0 = 2, +5 = 5 and script 0x8284E0. 1: the talk program selected by +5 through
//  a jump table (func_001BBE40 / func_001BC0E0 / func_001BC240 / func_001BC290 steps;
//  +5 0 branches on D_0081075E / D_0081075D with func_0019C6F0; +5 5 ends the talk
//  and restarts clip 0; +5 6 starts script 0x8286E0 on +0xB bit 2; +5 7 calls
//  func_001B0C60(1, 0xFF, 0) at script end), then func_001BC300. 2, 3: func_001AFC10.
extern unsigned char D_0081075A;
extern unsigned char D_0081075D;
extern unsigned char D_0081075E;
extern char D_overlay_AREA00_008284E0[];
extern char D_overlay_AREA00_008286E0[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern int func_0019C6F0(int a0, int a1);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);
extern void func_001B0C60(int a0, int a1, int a2);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA00_func_00823540(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        if (D_0081075A != 0xFF) {
            self[0] = 2;
            self[5] = 5;
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008284E0);
        } else {
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081075E == 0xFF) {
                self[5] = 1;
                func_0019C6F0(2, 0);
                func_0019C6F0(0, 1);
            } else if (D_0081075D == 0xFF) {
                self[5] = 6;
                func_0019C6F0(2, 0);
            } else if (func_001BBE40(self, talk, 0)) {
                self[5] = 2;
            }
            break;
        case 1:
            if (func_001BBE40(self, talk, 0)) {
                self[5]++;
            }
            break;
        case 2:
            if (func_001BC0E0(self, talk)) {
                self[5]++;
            }
            break;
        case 3:
            func_001BC240(self, talk);
            self[5]++;
            break;
        case 4:
            if (func_001BC290(self, talk)) {
                self[5] = 0;
            }
            break;
        case 5:
            if (func_001BA1F0(self)) {
                self[0] = 1;
                self[0xB] = 0;
                self[5] = 0;
                func_001C67E0(self, 0, 0.0f, 0.0f);
            }
            break;
        case 6:
            if (self[0xB] & 4) {
                self[5]++;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_008286E0);
            }
            break;
        case 7:
            if (func_001BA1F0(self)) {
                func_001B0C60(1, 0xFF, 0);
                self[5]++;
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
