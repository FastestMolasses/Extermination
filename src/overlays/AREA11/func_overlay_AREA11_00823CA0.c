// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823CE0 (splat/link name 00823CA0; overlay code is
// linked 0x40 below where it runs), 0x1A0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: state 0: inert (state 3) when func_001BA1C0(self, 0x30), else
// state 1 and +0 = 1. State 1, +5 0: once D_00810788 is set, +0x28 = 0,
// script 0x828C70, +5 = 1, D_008106C8 &= 0xF1FFFF8F, func_001D2830(8, 1),
// func_001C1DC0 and func_001FABB0. +5 1 at the script end:
// D_008106C8 |= 0x40, func_001D2830(8, 0), func_001C1DC0,
// func_001AEE10(4, 0), func_001C4760(0x1A, 1), func_001FAE70(0),
// +0x2E = 0xFFFF, D_00810808 = 0xFF, state 3. Then func_001B17A0. States
// 2/3: func_001AFC10.
extern unsigned char D_00810788;
extern unsigned char D_00810808;
extern unsigned int D_008106C8;
extern char D_overlay_AREA11_00828C70[];
extern int func_001BA1C0(unsigned char *self, int idx);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001D2830(int a0, int a1);
extern void func_001C1DC0(void);
extern void func_001FABB0(void);
extern void func_001AEE10(int a0, int a1);
extern void func_001C4760(int a0, int a1);
extern void func_001FAE70(int a0);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_00823CA0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x30) != 0) {
            self[4] = 3;
            break;
        }
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810788 != 0) {
                *(short *)(self + 0x28) = 0;
                func_001BA1A0(talk, D_overlay_AREA11_00828C70);
                self[5] = 1;
                D_008106C8 &= 0xF1FFFF8F;
                func_001D2830(8, 1);
                func_001C1DC0();
                func_001FABB0();
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                D_008106C8 |= 0x40;
                func_001D2830(8, 0);
                func_001C1DC0();
                func_001AEE10(4, 0);
                func_001C4760(0x1A, 1);
                func_001FAE70(0);
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                D_00810808 = 0xFF;
                self[4] = 3;
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
