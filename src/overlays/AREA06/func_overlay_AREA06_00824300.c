// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA06 overlay, runtime 0x00824340 (splat/link name 00824300; overlay code is
// linked 0x40 below where it runs), 0x214 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role: the AREA06 examine-switch behaviour of docs/FINDINGS.md (record [6]).
// State 0: func_001B0FD0, func_001C6380, +0x30 = &D_00275940; with bit 5 of
// D_00810845 set: sub-state 2 when D_00810C87 != 0, else +0 = 1 and script
// 0x827040 primed (func_001BA1A0 on self + 0x1F0); with it clear: +0 = 1 and
// script 0x826D40. State 1, sub-state +5: 0 waits for bit 2 of +0xB, then
// writes (0, 0, 9.5, 1) to 0x700038A0, func_001B6F00(self, 0x700038A0, pi)
// and advances; 1 at script end (func_001BA1F0) re-primes 0x827040 (calling
// func_001C4760(7, 1) first when D_00810CCA == 0) or 0x826D40 by the same
// flag, then +5 = 0, +0xB = 0, +0 = 1; 2 does nothing. Then the +0x4C method
// when func_001B17A0 is nonzero. States 2/3 func_001AFC10.
extern unsigned char D_00810845[];
extern unsigned char D_00810C87[];
extern unsigned char D_00810CCA[];
extern int D_00275940;
extern int D_700038A0[];
extern char D_overlay_AREA06_00826D40[];
extern char D_overlay_AREA06_00827040[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *v, float f12);
extern void func_001C4760(int a0, int a1);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA06_00824300(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        if (D_00810845[0] & 0x20) {
            if (D_00810C87[0] != 0) {
                self[5] = 2;
            } else {
                self[0] = 1;
                func_001BA1A0(blk, D_overlay_AREA06_00827040);
            }
        } else {
            self[0] = 1;
            func_001BA1A0(blk, D_overlay_AREA06_00826D40);
        }
        *(int **)(self + 0x30) = &D_00275940;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A8 = 9.5f;
                *(float *)0x700038AC = 1.0f;
                func_001B6F00(self, D_700038A0, 3.1415927f);
                self[5]++;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                if (D_00810845[0] & 0x20) {
                    if (D_00810CCA[0] == 0) {
                        func_001C4760(7, 1);
                    }
                    func_001BA1A0(blk, D_overlay_AREA06_00827040);
                } else {
                    func_001BA1A0(blk, D_overlay_AREA06_00826D40);
                }
                self[5] = 0;
                self[0xB] = 0;
                self[0] = 1;
            }
            break;
        case 2:
            break;
        }
        if (func_001B17A0(self) != 0) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
