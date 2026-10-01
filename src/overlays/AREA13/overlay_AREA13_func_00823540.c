// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x158 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: door [17] (sub 0 placement, door id 3: room move to entry 9 / 4).
//  State 0: func_001BBDA0(self, talk), the owner's (+0x1C) +0xB = 1. State 1
//  by +5: 0 sets +0 and the owner's +0xB from flag 0x1C (D_00810774 == 1: +0
//  = 2, owner +0xB = 0; else 1 / 1) and steps when func_001BBE40(self, talk,
//  0) is set; 1 steps on func_001BC0E0; 2 runs func_001BC240 and steps; 3
//  returns to 0 on func_001BC290. Then func_001BC300. States 2/3
//  func_001AFC10.
extern unsigned char D_00810774;
extern void func_001BBDA0(unsigned char *self, unsigned char *talk);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA13_func_00823540(unsigned char *self) {
    unsigned char *owner = *(unsigned char **)(self + 0x1C);
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self, talk);
        owner[0xB] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810774 == 1) {
                self[0] = 2;
                owner[0xB] = 0;
            } else {
                self[0] = 1;
                owner[0xB] = 1;
            }
            if (func_001BBE40(self, talk, 0)) {
                self[5]++;
            }
            break;
        case 1:
            if (func_001BC0E0(self, talk)) {
                self[5]++;
            }
            break;
        case 2:
            func_001BC240(self, talk);
            self[5]++;
            break;
        case 3:
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
