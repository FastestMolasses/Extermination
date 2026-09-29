// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x00825E20 (splat/link name 00825DE0; overlay code is
// linked 0x40 below where it runs), 0x178 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role: state 0: state 3 when func_001BA1C0(self, 0x2A), else state 1 and
// +0 = 1. State 1, sub-state +5: 0 waits for func_001BA1C0(self, 0x31) with
// D_70003B8D != 4, then func_001FABB0 and script 0x8276C0 (sub 1); 1 at
// script end sub 2; 2 sets D_00810802 = 0xFF, state 3, bit 4 of D_00810845,
// then func_001FABB0, func_001FB0B0(0x15), func_001C4760(0x1F, 1). Then
// func_001B17A0. States 2/3 func_001AFC10.
extern unsigned char D_70003B8D;
extern unsigned char D_00810802;
extern unsigned char D_00810845;
extern char D_overlay_AREA06_008276C0[];
extern int func_001BA1C0(unsigned char *self, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FABB0(void);
extern void func_001FB0B0(int a0);
extern void func_001C4760(int a0, int a1);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA06_00825DE0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2A) != 0) {
            self[4] = 3;
        } else {
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 0x31) != 0 && D_70003B8D != 4) {
                func_001FABB0();
                func_001BA1A0(blk, D_overlay_AREA06_008276C0);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5] = 2;
            }
            break;
        case 2:
            D_00810802 = 0xFF;
            self[4] = 3;
            D_00810845 |= 0x10;
            func_001FABB0();
            func_001FB0B0(0x15);
            func_001C4760(0x1F, 1);
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
