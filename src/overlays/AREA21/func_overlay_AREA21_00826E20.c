// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00826E60 (splat/link name 00826E20; overlay code
//  is linked 0x40 below where it runs), 0x1D8 bytes.
// Byte-identical at link (jump table pinned; overlay_match.py check AREA21
//  reports 99.98-99.99, rodata-needs-pin only; lane A03C).
// Role: door sub 0 placements [27], [33] (door entries 0 / 2). The AREA19
//  0x823580 door steps without the D_00810CCD / func_001C4760 branch; +0 is
//  1 while the lock bit (+0x34) is set and 2 otherwise; script 0x82CB00 on
//  step 1. The step switch is a jump table at 0x82DC20 (jt_pin.py).
extern unsigned char D_00810841[];
extern char D_overlay_AREA21_0082CB00[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00826E20(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        break;
    case 1:
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
