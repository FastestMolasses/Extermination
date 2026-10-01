// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008261A0 (splat/link name 00826160; overlay code
//  is linked 0x40 below where it runs), 0x124 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [42]. State 0: state 3 if func_001BA1C0(self,
//  0x34), else state 1, +0 = 1. State 1: +5 0: script 0x82B240 once
//  D_0081078C != 0; +5 1: at the script end func_001FAE70(0), +0x2E = 0xFFFF,
//  state 3, D_0081080C = 0xFF. func_001B17A0 every frame of state 1.
extern unsigned char D_0081078C[];
extern unsigned char D_0081080C[];
extern char D_overlay_AREA21_0082B240[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FAE70(int a);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00826160(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x34) != 0) {
            self[4] = 3;
        } else {
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081078C[0] != 0) {
                func_001BA1A0(talk, D_overlay_AREA21_0082B240);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001FAE70(0);
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[4] = 3;
                D_0081080C[0] = 0xFF;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
