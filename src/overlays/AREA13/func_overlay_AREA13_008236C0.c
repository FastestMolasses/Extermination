// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823700 (splat/link name 008236C0; overlay code
//  is linked 0x40 below where it runs), 0x12C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placement [3]. State 0: func_001B10B0(self, +0xD, 0x62),
//  func_001C63E0(self, 0), func_001CA6F0(self, 2), func_001BA8E0(self, +0xD),
//  +0x30 = 0x82A760, +0x58 = D_0028A61C, state 1, +0 = 1. State 1: state 3
//  when func_001BA1C0(self, 0x1A) is set, else by D_008107F1: 0 -> 0x823830,
//  1 -> 0x823940. States 2/3: func_001BA540, func_001AFC10.
extern unsigned char D_008107F1;
extern int D_0028A61C;
extern char D_overlay_AREA13_0082A760[];
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int id);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA13_00823830(unsigned char *self);
extern void func_overlay_AREA13_00823940(unsigned char *self);

void func_overlay_AREA13_008236C0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B10B0(self, self[0xD], 0x62);
        func_001C63E0(self, 0);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        *(void **)(self + 0x30) = D_overlay_AREA13_0082A760;
        *(int *)(self + 0x58) = D_0028A61C;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (func_001BA1C0(self, 0x1A) != 0) {
            self[4] = 3;
            break;
        }
        switch (D_008107F1) {
        case 0:
            func_overlay_AREA13_00823830(self);
            break;
        case 1:
            func_overlay_AREA13_00823940(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
