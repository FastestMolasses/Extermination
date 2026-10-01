// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008292A0 (splat/link name 00829260; overlay code
//  is linked 0x40 below where it runs), 0xF8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [49]..[54]: state 3 at once when D_008107F4 bit 5 is
//  set; else when func_001B0FD0 returns 0: func_001C6380, +0x28 = 0, state
//  0x64. State 0x64: func_001B1B70 and the +0x4C method; while bit 5 is set
//  +0x28 counts down and below 0 goes to state 1, which sets state 3. State 3
//  and others func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107F4;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00829260(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (D_008107F4 & 0x20) {
            self[4] = 3;
            break;
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 0x64;
            *(short *)(self + 0x28) = 0;
        }
        break;
    case 0x64:
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        if (*(unsigned char *)0x8107F4 & 0x20) {
            *(short *)(self + 0x28) -= 1;
            if (*(short *)(self + 0x28) < 0) {
                self[4] = 1;
            }
        }
        break;
    case 1:
        self[4] = 3;
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
