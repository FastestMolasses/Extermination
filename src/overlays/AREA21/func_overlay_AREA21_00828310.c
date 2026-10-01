// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00828350 (splat/link name 00828310; overlay code
//  is linked 0x40 below where it runs), 0xC0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 deferred group 0x82A380 member. State 0:
//  func_0015AC00(self, self + 0x1F0); state 1: func_0015AE20(self, self +
//  0x1F0) and, while D_0081080E == 0x10, state 3 after 251 frames; other
//  states: func_001B1190(+0x9A), func_001AFC10.
extern unsigned char D_0081080E[];
extern void func_0015AC00(unsigned char *self, unsigned char *w);
extern void func_0015AE20(unsigned char *self, unsigned char *w);
extern void func_001B1190(int id);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00828310(unsigned char *self) {
    unsigned char *w = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_0015AC00(self, w);
        break;
    case 1:
        func_0015AE20(self, w);
        if (D_0081080E[0] == 0x10) {
            *(short *)(self + 0x28) += 1;
            if (*(short *)(self + 0x28) > 0xFA) {
                self[4] = 3;
            }
        }
        break;
    case 2:
    case 3:
    default:
        func_001B1190(self[0x9A]);
        func_001AFC10(self);
        break;
    }
}
