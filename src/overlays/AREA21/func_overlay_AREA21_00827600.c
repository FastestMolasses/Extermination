// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827640 (splat/link name 00827600; overlay code
//  is linked 0x40 below where it runs), 0x234 bytes.
// Byte-identical at link (jump table pinned; overlay_match.py check AREA21
//  reports 99.98-99.99, rodata-needs-pin only; lane A03C).
// Role: sub 0 placement [51]. State 0 saves +0xB0 / +0xB4 in +0x1F0 /
//  +0x1F4. State 1 offsets +0xB0 / +0xB4 from them by an 8-step pattern of
//  +0x28 & 7 (0.075 / 0.05 / 0.06), then func_001C6380, func_001B17A0 and the
//  +0x4C method. The step switch is a jump table at 0x82DC60 (jt_pin.py).
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827600(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        F(0x1F0) = F(0xB0);
        F(0x1F4) = F(0xB4);
        break;
    case 1:
        *(short *)(self + 0x28) += 1;
        switch (*(short *)(self + 0x28) & 7) {
        case 0:
            F(0xB0) = F(0x1F0);
            F(0xB4) = F(0x1F4);
            break;
        case 1:
            F(0xB0) = F(0x1F0) - 0.075f;
            F(0xB4) = F(0x1F4);
            break;
        case 2:
            F(0xB0) = F(0x1F0);
            F(0xB4) = F(0x1F4) - 0.05f;
            break;
        case 3:
            F(0xB0) = F(0x1F0);
            F(0xB4) = 0.075f + F(0x1F4);
            break;
        case 4:
            F(0xB0) = 0.075f + F(0x1F0);
            F(0xB4) = F(0x1F4);
            break;
        case 5:
            F(0xB0) = F(0x1F0) - 0.05f;
            F(0xB4) = 0.06f + F(0x1F4);
            break;
        case 6:
            F(0xB0) = 0.075f + F(0x1F0);
            F(0xB4) = F(0x1F4) - 0.05f;
            break;
        case 7:
            F(0xB0) = 0.05f + F(0x1F0);
            F(0xB4) = 0.05f + F(0x1F4);
            break;
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
