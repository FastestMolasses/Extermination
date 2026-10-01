// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827040 (splat/link name 00827000; overlay code
//  is linked 0x40 below where it runs), 0x210 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [53]. State 0 (after func_001B0FD0): func_001C6380,
//  state 1, +0 = 1. State 1: +5 0: once func_001BA1C0(self, 0x36),
//  D_008106C0 = func_001B6660(group 0x82A600); +5 1: when D_0081080F != 0,
//  D_0081078F = 1, script 0x82CBD0, func_001D2830(0x24, 1); +5 2: at the
//  script end (result r) func_001AEDE0(4, 0), D_70003B93 = 2 (r == 3) or 1,
//  func_001FBC50(), D_00810858 = D_008104D0 = 100, D_0081085C = D_008104D8 =
//  0, +0x2E = 0xFFFF, state 3, D_0081080F = 0xFF, D_008106B8 = 1. While
//  D_0081080F != 0: +0xC0 = 1.308997 and func_001C6380. Then func_001B1B70
//  and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern int D_008106C0[];
extern unsigned char D_0081080F[];
extern unsigned char D_0081078F[];
extern unsigned char D_70003B93[];
extern float D_00810850[];
extern float D_008104D0[];
extern unsigned char D_008106B8[];
extern char D_overlay_AREA21_0082A600[];
extern char D_overlay_AREA21_0082CBD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B6660(void *group);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001D2830(int a0, int a1);
extern void func_001AEDE0(int a0, int a1);
extern void func_001FBC50(void);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827000(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int r;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 0x36) != 0) {
                D_008106C0[0] = func_001B6660(D_overlay_AREA21_0082A600);
                self[5] = 1;
            }
            break;
        case 1:
            if (D_0081080F[0] != 0) {
                D_0081078F[0] = 1;
                func_001BA1A0(talk, D_overlay_AREA21_0082CBD0);
                self[5] = 2;
                func_001D2830(0x24, 1);
            }
            break;
        case 2:
            if ((r = func_001BA1F0(self)) != 0) {
                func_001AEDE0(4, 0);
                if (r == 3) {
                    D_70003B93[0] = 2;
                } else {
                    D_70003B93[0] = 1;
                }
                func_001FBC50();
                D_00810850[2] = 100.0f;
                D_008104D0[0] = 100.0f;
                D_00810850[3] = 0.0f;
                D_008104D0[2] = 0.0f;
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[4] = 3;
                D_0081080F[0] = 0xFF;
                D_008106B8[0] = 1;
            }
            break;
        }
        if (D_0081080F[0] != 0) {
            *(float *)(self + 0xC0) = 1.308997f;
            func_001C6380(self);
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
