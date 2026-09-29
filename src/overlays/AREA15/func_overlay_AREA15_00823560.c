// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008235A0 (splat/link name 00823560; overlay code is
// linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: state 3 when func_001BA1C0(self, 0x23) is set; otherwise
//  func_001B10B0(self, +0xD, 0x65), func_001C63E0(self, 0),
//  func_001CA6F0(self, 2), +0 = 1, +0x30 = 0x8272B0, +0x58 = D_0028A6E8, state
//  1. State 1 runs 0x8236B0 when D_008107FA is 0 and 0x823780 when it is 1.
//  States 2/3 func_001AFC10.
extern unsigned char D_008107FA;
extern int D_0028A6E8;
extern char D_overlay_AREA15_008272B0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA15_008236B0(unsigned char *self);
extern void func_overlay_AREA15_00823780(unsigned char *self);

void func_overlay_AREA15_00823560(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x23) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x65);
        func_001C63E0(self, 0);
        func_001CA6F0(self, 2);
        self[0] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA15_008272B0;
        *(int *)(self + 0x58) = D_0028A6E8;
        self[4] = 1;
        break;
    case 1:
        switch (D_008107FA) {
        case 0:
            func_overlay_AREA15_008236B0(self);
            break;
        case 1:
            func_overlay_AREA15_00823780(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
