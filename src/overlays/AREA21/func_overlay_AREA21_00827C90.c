// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827CD0 (splat/link name 00827C90; overlay code
//  is linked 0x40 below where it runs), 0x1A0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [58]. +5 = 2 once D_0081078E == 0xFF; +5 0 waits
//  for D_0081080E == 3, +5 1 counts +0x28 to 830; +5 2 calls func_0019C6F0(n,
//  1) for n = 0x10, 0x13, 0x14, 0x16..0x1B, then func_001B1B70 and the +0x4C
//  method.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_0081078E[];
extern unsigned char D_0081080E[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_0019C6F0(int id, int a1);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827C90(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[5] = 0;
        break;
    case 1:
        if (D_0081078E[0] == 0xFF) {
            self[5] = 2;
        }
        switch (self[5]) {
        case 0:
            if (D_0081080E[0] == 3) {
                S16(0x28) = 0;
                self[5]++;
            }
            break;
        case 1:
            S16(0x28)++;
            if (S16(0x28) >= 0x33E) {
                self[5]++;
            }
            break;
        case 2:
            func_0019C6F0(0x10, 1);
            func_0019C6F0(0x13, 1);
            func_0019C6F0(0x14, 1);
            func_0019C6F0(0x16, 1);
            func_0019C6F0(0x17, 1);
            func_0019C6F0(0x18, 1);
            func_0019C6F0(0x19, 1);
            func_0019C6F0(0x1A, 1);
            func_0019C6F0(0x1B, 1);
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
