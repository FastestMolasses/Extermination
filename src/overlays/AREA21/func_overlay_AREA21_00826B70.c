// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00826BB0 (splat/link name 00826B70; overlay code
//  is linked 0x40 below where it runs), 0x2A8 bytes.
// Byte-identical at link (jump table pinned; overlay_match.py check AREA21
//  reports 99.98-99.99, rodata-needs-pin only; lane A03C).
// Role: door sub 0 placement [30] (door table entry 1). The door steps of
//  0x826E60, preceded, while D_0081080E == 3, by a timer in +7 / +0x28 that
//  sets bit +0x34 of the area lock byte D_00810841[D_00810700] at frame 1492
//  and clears it at frame 2000. The door step switch is a jump table at
//  0x82DC00 (tools/overlay/jt_pin.py).
extern unsigned char D_00810841[];
extern unsigned char D_0081080E[];
extern char D_overlay_AREA21_0082CB00[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00826B70(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        break;
    case 1:
        if (D_0081080E[0] == 3) {
            switch (self[7]) {
            case 0:
                self[7] = 1;
                *(short *)(self + 0x28) = 0;
                break;
            case 1:
                *(short *)(self + 0x28) += 1;
                if (*(short *)(self + 0x28) == 1492) {
                    D_00810841[*(unsigned char *)0x810700] |= 1 << self[0x34];
                }
                if (*(short *)(self + 0x28) == 2000) {
                    D_00810841[*(unsigned char *)0x810700] &= ~(1 << self[0x34]);
                    self[7] = 2;
                }
                break;
            case 2:
                break;
            }
            break;
        }
        switch (self[5]) {
        case 0:
            if (D_00810841[*(unsigned char *)0x810700] & (1 << *(short *)(self + 0x34))) {
                self[0] = 1;
                if (func_001BBE40(self, talk, 0)) {
                    self[5] = 3;
                }
            } else {
                self[0] = 2;
                if (func_001BBE40(self, talk, 1)) {
                    self[5]++;
                }
            }
            break;
        case 1:
            if (func_001BC0E0(self, talk)) {
                func_001BA1A0(talk, D_overlay_AREA21_0082CB00);
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
        }
        func_001BC300(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
