// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00823BA0 (splat/link name 00823B60; overlay code
//  is linked 0x40 below where it runs), 0x3CC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [30] (counter 0x2D). State 0: state 3 when
//  func_001BA1C0(self, 0x2D); after func_001B0FD0: func_001C6380, state 1, +0
//  = 1, script 0x826A20. State 1 (+5): 0 -> 1; 1: a skip (func_001BA1F0 bit
//  1) goes to 0x19; once D_00810805 is set D_008106C0 = func_001B6660(group
//  0x8263C0), +0x2E = 0xFFFF, +5 2. 2: at D_00810805 == 2 func_001B6660(group
//  0x826420), func_001F6B60(), D_00810805 = 3, the +0x1C object's +0x28 =
//  0x11E, func_0019C6F0(0xE / 0xF, 1); a skip -> 0x19, the script end -> 3
//  with func_001FB0B0(0xB). 0x19 (skip): the same results at once, D_00810805
//  = 4, the +0x1C object's +0x28 = 1, the player at (156.5, 220, 300) heading
//  2.1118484, camera D_008105D0 / D_008105E0 = (116.4, 259, 324.1) / (156.5,
//  237, 300) copied to D_008101F0 / D_00810200, +5 3. 3: at D_00810805 == 8
//  script 0x8270A0 (+5 4); 4: its end -> 5. Then func_001C6380 and the +0x4C
//  method when func_001B17A0. States 2 / 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810805;
extern void *D_008106C0;
extern float D_008101F0[];
extern float D_00810200[];
extern float D_008105D0[];
extern float D_008105E0[];
extern char D_overlay_AREA17_00826A20[];
extern char D_overlay_AREA17_008263C0[];
extern char D_overlay_AREA17_00826420[];
extern char D_overlay_AREA17_008270A0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void *func_001B6660(void *grp);
extern void func_001F6B60(void);
extern void func_0019C6F0(int id, int on);
extern void func_001FB0B0(int a);
extern void func_00102948(void *dst, void *src);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00823B60(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int r;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2D) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            self[0] = 1;
            func_001BA1A0(talk, D_overlay_AREA17_00826A20);
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            self[5] = 1;
        case 1:
            if (func_001BA1F0(self) & 2) {
                self[5] = 0x19;
            }
            if (D_00810805 != 0) {
                D_008106C0 = func_001B6660(D_overlay_AREA17_008263C0);
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[5] = 2;
            }
            break;
        case 2:
            if (D_00810805 == 2) {
                func_001B6660(D_overlay_AREA17_00826420);
                func_001F6B60();
                D_00810805 = 3;
                *(short *)(*(unsigned char **)(self + 0x1C) + 0x28) = 0x11E;
                func_0019C6F0(0xE, 1);
                func_0019C6F0(0xF, 1);
            }
            r = func_001BA1F0(self);
            if (r & 2) {
                self[5] = 0x19;
            } else if (r != 0) {
                self[5] = 3;
                func_001FB0B0(0xB);
            }
            break;
        case 0x19:
            self[5] = 3;
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            if (D_00810805 == 0) {
                D_008106C0 = func_001B6660(D_overlay_AREA17_008263C0);
            }
            if (D_00810805 != 2) {
                func_001FB0B0(0xB);
                func_001B6660(D_overlay_AREA17_00826420);
                func_001F6B60();
            }
            D_00810805 = 4;
            *(short *)(*(unsigned char **)(self + 0x1C) + 0x28) = 1;
            func_0019C6F0(0xE, 1);
            func_0019C6F0(0xF, 1);
            *(float *)0x810350 = 156.5f;
            *(float *)0x810354 = 220.0f;
            *(float *)0x810358 = 300.0f;
            *(float *)0x810374 = 2.1118484f;
            *(float *)0x8105D0 = 116.4f;
            *(float *)0x8105D4 = 259.0f;
            *(float *)0x8105D8 = 324.1f;
            *(float *)0x8105E0 = 156.5f;
            *(float *)0x8105E4 = 237.0f;
            *(float *)0x8105E8 = 300.0f;
            func_00102948(D_008101F0, D_008105D0);
            func_00102948(D_00810200, D_008105E0);
            self[5] = 3;
            break;
        case 3:
            if (D_00810805 == 8) {
                func_001BA1A0(talk, D_overlay_AREA17_008270A0);
                self[5] = 4;
            }
            break;
        case 4:
            if (func_001BA1F0(self) != 0) {
                self[5] = 5;
            }
            break;
        case 5:
            break;
        }
        func_001C6380(self);
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
