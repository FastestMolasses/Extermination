// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008293A0 (splat/link name 00829360; overlay code
//  is linked 0x40 below where it runs), 0x63C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [47] / [48]. State 0: with D_008107F4 bit 6 clear it
//  loads the placement record's second pose (+0x2C model, +0x2E, +0x34..
//  position, +0x40.. rotation) and calls func_0019C6F0(0x1F, 1), (0x20, 0);
//  with it set func_0019C6F0(0x1F, 0), (0x20, 1); then when func_001B0FD0
//  returns 0: func_001C6380 and +0x28 = 0x6E0. State 1: counts +0x28 down
//  while bit 6 is set; for model 0x13 at zero it loads the record's first
//  pose (+4 model ..), calls func_001AF800, func_001CB5B0(+9), func_001B0FD0,
//  func_001C6380 and func_0019C6F0(0x1F, 0), (0x20, 1). Then func_001B1B70
//  and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
#define REC ((unsigned char *)(self[0x9A] * 0x28 + (int)D_0024D7C0[D_00810700][D_00810701]))
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_008107F4;
extern void func_0019C6F0(int id, int on);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AF800(unsigned char *self);
extern void func_001CB5B0(int a);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00829360(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (!(D_008107F4 & 0x40)) {
            self[0xD] = REC[0x2C];
            *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
            *(float *)(self + 0xC0) = *(float *)(REC + 0x40);
            *(float *)(self + 0xC4) = *(float *)(REC + 0x44);
            *(float *)(self + 0xC8) = *(float *)(REC + 0x48);
            *(float *)(self + 0xB0) = *(float *)(REC + 0x34);
            *(float *)(self + 0xB4) = *(float *)(REC + 0x38);
            *(float *)(self + 0xB8) = *(float *)(REC + 0x3C);
            func_0019C6F0(0x1F, 1);
            func_0019C6F0(0x20, 0);
        } else {
            func_0019C6F0(0x1F, 0);
            func_0019C6F0(0x20, 1);
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            *(short *)(self + 0x28) = 0x6E0;
        }
        break;
    case 1:
        if (D_008107F4 & 0x40) {
            if (*(short *)(self + 0x28) != 0) {
                *(short *)(self + 0x28) -= 1;
            }
        }
        if (self[0xD] == 0x13) {
            if ((*(unsigned char *)0x8107F4 & 0x40) && *(short *)(self + 0x28) == 0) {
                self[0xD] = REC[4];
                *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 6);
                *(float *)(self + 0xC0) = *(float *)(REC + 0x18);
                *(float *)(self + 0xC4) = *(float *)(REC + 0x1C);
                *(float *)(self + 0xC8) = *(float *)(REC + 0x20);
                *(float *)(self + 0xB0) = *(float *)(REC + 0xC);
                *(float *)(self + 0xB4) = *(float *)(REC + 0x10);
                *(float *)(self + 0xB8) = *(float *)(REC + 0x14);
                func_001AF800(self);
                func_001CB5B0(self[9]);
                func_001B0FD0(self);
                func_001C6380(self);
                self[4] = 1;
                func_0019C6F0(0x1F, 0);
                func_0019C6F0(0x20, 1);
            }
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
