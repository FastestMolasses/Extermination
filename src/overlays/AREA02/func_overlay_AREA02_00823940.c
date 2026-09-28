// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00823980 (splat/link name 00823940; overlay
// code is linked 0x40 below where it runs), 0x3EC bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00823940, 00823980 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: owner with state +4. 0: if D_00810761 is set, both D_00810761 and
//  D_008107E1 = 0xFF, +4 = 3, func_0019C6F0(0x1D, 1) and (0x1E, 1); else
//  +0x30 = 0x827340, +4 = 1, +0 = 1, +0x28 = +0x2A = 0. 1: by D_008107E1:
//  0 -> func_001F4BF0 with the scratchpad position +0xB0 + (1, 15, -3.5) and
//  the word vector (0, 0x80, 0, 0x80); when +0xB bit 2 is set both flags = 1
//  and script 0x826780 starts. 1 -> at script end func_001FABB0,
//  func_001FA790(0, 0x13), D_008107E1 |= 2. 0xFF -> once (+6 == 0 and
//  D_00282154 == 0) +6++ and func_001FAE70(0). Then func_001B17A0; if
//  D_008107E1 > 0, sub-state +5: 0 -> result of 0x824D50 (1: +5 = 1,
//  2: +5 = 8); 1 -> script 0x826F40, +5 = 2; 2 -> +5 = 3 at script end,
//  D_00810350 <= -25 is set to -25 and then D_00810358 <= -66 to -66;
//  8 -> script 0x826B40, +5 = 9; 9 -> +5 = 10 at script end, the same
//  D_00810350 clamp. 2, 3: func_001AFC10.
// Matching: the scratchpad stores are externs in source order (idiom-32).
extern unsigned char D_00810761;
extern unsigned char D_008107E1;
extern signed char D_00282154;
extern float D_00810350;
extern float D_00810358;
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA02_00827340[];
extern char D_overlay_AREA02_00826780[];
extern char D_overlay_AREA02_00826F40[];
extern char D_overlay_AREA02_00826B40[];
extern int func_0019C6F0(int a0, int a1);
extern void func_001F4BF0(void *a, void *b);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FABB0(void);
extern void func_001FA790(int, int);
extern void func_001FAE70(int a0);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern int func_overlay_AREA02_00824D50(unsigned char *self);

void func_overlay_AREA02_00823940(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (D_00810761 != 0) {
            D_00810761 = 0xFF;
            D_008107E1 = 0xFF;
            self[4] = 3;
            func_0019C6F0(0x1D, 1);
            func_0019C6F0(0x1E, 1);
        } else {
            *(char **)(self + 0x30) = D_overlay_AREA02_00827340;
            self[4] = 1;
            self[0] = 1;
            *(short *)(self + 0x28) = 0;
            *(short *)(self + 0x2A) = 0;
        }
        break;
    case 1:
        switch (D_008107E1) {
        case 0:
            D_700038A0[0] = *(float *)(self + 0xB0) + 1.0f;
            D_700038A0[1] = *(float *)(self + 0xB4) + 15.0f;
            D_700038A0[2] = *(float *)(self + 0xB8) - 3.5f;
            D_700038A0[3] = 1.0f;
            D_700038B0[0] = 0;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
            func_001F4BF0(D_700038A0, D_700038B0);
            if (self[0xB] & 4) {
                D_00810761 = 1;
                D_008107E1 = 1;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826780);
            }
            break;
        case 1:
            if (func_001BA1F0(self)) {
                func_001FABB0();
                func_001FA790(0, 0x13);
                D_008107E1 |= 2;
            }
            break;
        case 2:
            break;
        case 0xFF:
            switch (self[6]) {
            case 0:
                if (D_00282154 == 0) {
                    self[6]++;
                    func_001FAE70(0);
                }
                break;
            }
            break;
        }
        func_001B17A0(self);
        if (D_008107E1 > 0) {
            switch (self[5]) {
            case 0:
                switch (func_overlay_AREA02_00824D50(self)) {
                case 1:
                    self[5] = 1;
                    break;
                case 2:
                    self[5] = 8;
                    break;
                }
                break;
            case 1:
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826F40);
                self[5] = 2;
                break;
            case 2:
                if (func_001BA1F0(self)) {
                    self[5] = 3;
                }
                if (D_00810350 <= -25.0f) {
                    D_00810350 = -25.0f;
                    if (D_00810358 <= -66.0f) {
                        D_00810358 = -66.0f;
                    }
                }
                break;
            case 3:
                break;
            case 8:
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826B40);
                self[5] = 9;
                break;
            case 9:
                if (func_001BA1F0(self)) {
                    self[5] = 10;
                }
                if (D_00810350 <= -25.0f) {
                    D_00810350 = -25.0f;
                }
                break;
            case 10:
                break;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
