// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00823850 (splat/link name 00823810; overlay code is
// linked 0x40 below where it runs), 0x1A0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: state 3 when func_001BA1C0(self, 0x23) is set or
//  func_001BA1C0(self, 0x22) is clear; model setup by +0xD (0x54:
//  func_001B10B0(.., 0x56), func_001C63E0(self, 8), D_008106C0 =
//  func_001B6660(0x826CD0); 0x5D: func_001B10B0(.., 0x5F), func_001C63E0(self,
//  0), +0x58 = D_0028A610); +0x28 = +0x2A = 0, state 1, +0 = 1. State 1 by
//  D_008107FB: 0 -> 0x8239F0, 1 -> 0x823B40, 2..4 -> 0x823C80. States 2/3
//  func_001AFC10.
extern unsigned char D_008107FB;
extern unsigned char *D_008106C0;
extern int D_0028A610;
extern char D_overlay_AREA15_00826CD0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern unsigned char *func_001B6660(void *p);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA15_008239F0(unsigned char *self);
extern void func_overlay_AREA15_00823B40(unsigned char *self);
extern void func_overlay_AREA15_00823C80(unsigned char *self);

void func_overlay_AREA15_00823810(unsigned char *self) {
    int id;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x23) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 0x22) == 0) {
            self[4] = 3;
            break;
        }
        id = self[0xD];
        if (id == 0x54) {
            func_001B10B0(self, (unsigned char)id, 0x56);
            func_001C63E0(self, 8);
            D_008106C0 = func_001B6660(D_overlay_AREA15_00826CD0);
        }
        id = self[0xD];
        if (id == 0x5D) {
            func_001B10B0(self, (unsigned char)id, 0x5F);
            func_001C63E0(self, 0);
            *(int *)(self + 0x58) = D_0028A610;
        }
        *(short *)(self + 0x28) = 0;
        *(short *)(self + 0x2A) = 0;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (D_008107FB) {
        case 0:
            func_overlay_AREA15_008239F0(self);
            break;
        case 1:
            func_overlay_AREA15_00823B40(self);
            break;
        case 2:
        case 3:
        case 4:
            func_overlay_AREA15_00823C80(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
