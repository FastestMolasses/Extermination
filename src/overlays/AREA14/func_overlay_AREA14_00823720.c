// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00823760 (splat/link name 00823720; overlay code
//  is linked 0x40 below where it runs), 0x160 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: sub 0 placement [5]. State 0: +0 = 1; with func_001BA1C0(self, 0x2C)
//  set: func_001F6B00(), func_001B6660(group 0x8265A0), state 2; otherwise
//  script 0x826800, state 1. State 1: +0x28 counts to 400, where it calls
//  func_001F6B00 and func_001B6660(0x8265A0); at the script end +0x2E =
//  0xFFFF, D_00810784 = D_00810804 = 0xFF, the same two calls unless +0x28
//  reached 400, state 2. State 2: state 3. State 3 and above: func_001AFC10.
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_00810784[];
extern unsigned char D_00810804[];
extern char D_overlay_AREA14_008265A0[];
extern char D_overlay_AREA14_00826800[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001F6B00(void);
extern int func_001B6660(void *group);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA14_00823720(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        self[0] = 1;
        if (func_001BA1C0(self, 0x2C) != 0) {
            func_001F6B00();
            func_001B6660(D_overlay_AREA14_008265A0);
            self[4] = 2;
        } else {
            func_001BA1A0(talk, D_overlay_AREA14_00826800);
            self[4] = 1;
        }
        break;
    case 1:
        if (S16(0x28) < 400) {
            S16(0x28)++;
            if (S16(0x28) == 400) {
                func_001F6B00();
                func_001B6660(D_overlay_AREA14_008265A0);
            }
        }
        if (func_001BA1F0(self) != 0) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            D_00810784[0] = 0xFF;
            D_00810804[0] = 0xFF;
            if (S16(0x28) != 400) {
                func_001F6B00();
                func_001B6660(D_overlay_AREA14_008265A0);
            }
            self[4] = 2;
        }
        break;
    case 2:
        self[4] = 3;
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
