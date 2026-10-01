// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00823E90 (splat/link name 00823E50; overlay code
//  is linked 0x40 below where it runs), 0x148 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placement [44]. State 0: state 3 when func_001BA1C0(self, 0x1C)
//  is set, else func_001B0FD0, +0x30 = &D_002759B8, +8 = 3, state 1, +0 = 1,
//  +0xA = 0, +0x34 = 4. State 1 dispatches on D_008107F4 & 0xF: 0 ->
//  0x824160, 1 -> 0x824180, 2 -> 0x824390, 3 -> 0x824520, 4 -> 0x824960.
//  States 2/3 func_001AFC10.
extern int D_002759B8;
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B0FD0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA13_00824160(unsigned char *self);
extern void func_overlay_AREA13_00824180(unsigned char *self);
extern void func_overlay_AREA13_00824390(unsigned char *self);
extern void func_overlay_AREA13_00824520(unsigned char *self);
extern void func_overlay_AREA13_00824960(unsigned char *self);

void func_overlay_AREA13_00823E50(unsigned char *self) {
    int step = *(unsigned char *)0x8107F4 & 0xF;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x1C) != 0) {
            self[4] = 3;
            break;
        }
        func_001B0FD0(self);
        *(int **)(self + 0x30) = &D_002759B8;
        self[8] = 3;
        self[4] = 1;
        self[0] = 1;
        self[0xA] = 0;
        *(short *)(self + 0x34) = 4;
        break;
    case 1:
        switch (step) {
        case 0:
            func_overlay_AREA13_00824160(self);
            break;
        case 1:
            func_overlay_AREA13_00824180(self);
            break;
        case 2:
            func_overlay_AREA13_00824390(self);
            break;
        case 3:
            func_overlay_AREA13_00824520(self);
            break;
        case 4:
            func_overlay_AREA13_00824960(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
