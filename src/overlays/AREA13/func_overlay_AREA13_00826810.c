// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00826850 (splat/link name 00826810; overlay code
//  is linked 0x40 below where it runs), 0x760 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [62] / [63]. State 0: +0 = 1, +8 = 1, +2 = 0x84
//  while D_00810C8B != 0 (else 4); z (+0xB8) > 1000 uses the descriptor
//  0x82CDD0 and D_00810839 bit 0, else 0x82CDF0 and bit 1. With the bit set:
//  +0xD = 0xD, +2 = 4, func_001B0FD0, func_001C6380, state 2; without it
//  func_001B0F60(self, 9) and func_001C68C0. Then +0x2EC = +0xB4 + 0.152 and
//  four transformed corner points go to +0x2CC..+0x2E8. State 1: +5 0 on +0xB
//  bit 2 writes the points 0x82CAB0 / 0x82CAC0 / 0x82CB00 (two sets, by z)
//  and starts script 0x82CA50; +5 1 stores func_001C64F0(self, 1.0) in the
//  talk block's +0xE, counts +0x28 and, when the script ends, sets the model
//  func_001C6120(D_0028A59C, 0xD), state 2, +2 = 4, ORs bit 0 or 1 into
//  D_00810839 and returns; at count 4 it writes 0x82CA00..0x82CA14. Then
//  func_001C68C0, the +0x4C method when func_001B17A0 is set, and four
//  func_001F4BF0 lights (colour 0x80, 0, 0, 0x80) at the corners. State 2: +1
//  = func_001B1630(+0xB0..B8); the +0x4C method while +1 != 0. Other states
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810C8B;
extern unsigned char D_00810839;
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_overlay_AREA13_0082CA00[];
extern float D_overlay_AREA13_0082CAB0[];
extern float D_overlay_AREA13_0082CAC0[];
extern float D_overlay_AREA13_0082CB00[];
extern char D_overlay_AREA13_0082CA50[];
extern char D_overlay_AREA13_0082CDD0[];
extern char D_overlay_AREA13_0082CDF0[];
extern int func_001B0FD0(unsigned char *self);
extern int func_001B0F60(unsigned char *self, int a1);
extern void func_001C6380(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern short func_001C64F0(unsigned char *self, float step);
extern int func_001BA1F0(unsigned char *self);
extern int func_001C6120(int a, int b);
extern void func_001CA6E0(unsigned char *self, int v);
extern void func_001C62C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *col);
extern int func_001B1630(float x, float y, float z);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00826810(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        self[0] = 1;
        self[8] = 1;
        if (*(unsigned char *)0x810C8B != 0) {
            self[2] = 0x84;
        } else {
            self[2] = 4;
        }
        if (!(*(float *)(self + 0xB8) <= 1000.0f)) {
            *(void **)(self + 0x30) = D_overlay_AREA13_0082CDD0;
            if (*(unsigned char *)0x810839 & 1) {
                self[0xD] = 0xD;
                self[2] = 4;
                if (func_001B0FD0(self) != 0) {
                    break;
                }
                func_001C6380(self);
                self[4] = 2;
            } else {
                if (func_001B0F60(self, 9) != 0) {
                    break;
                }
                func_001C68C0(self);
            }
        } else {
            *(void **)(self + 0x30) = D_overlay_AREA13_0082CDF0;
            if (*(unsigned char *)0x810839 & 2) {
                self[0xD] = 0xD;
                self[2] = 4;
                if (func_001B0FD0(self) != 0) {
                    break;
                }
                func_001C6380(self);
                self[4] = 2;
            } else {
                if (func_001B0F60(self, 9) != 0) {
                    break;
                }
                func_001C68C0(self);
            }
        }
        *(float *)(self + 0x2EC) = 0.152f + *(float *)(self + 0xB4);
        *(float *)0x700038A0 = -5.95f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = -5.688f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
        *(float *)(self + 0x2E8) = *(float *)0x700038B0;
        *(float *)(self + 0x2E4) = *(float *)0x700038B8;
        *(float *)0x700038A0 = 5.832f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
        *(float *)(self + 0x2E0) = *(float *)0x700038B0;
        *(float *)(self + 0x2DC) = *(float *)0x700038B8;
        *(float *)0x700038A8 = 5.536f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
        *(float *)(self + 0x2D8) = *(float *)0x700038B0;
        *(float *)(self + 0x2D4) = *(float *)0x700038B8;
        *(float *)0x700038A0 = -5.95f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
        *(float *)(self + 0x2D0) = *(float *)0x700038B0;
        *(float *)(self + 0x2CC) = *(float *)0x700038B8;
        break;
    case 1:
        if (D_00810C8B != 0) {
            self[2] = 0x84;
        } else {
            self[2] = 4;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (!(*(float *)(self + 0xB8) <= 1000.0f)) {
                    D_overlay_AREA13_0082CAB0[0] = 730.2f;
                    D_overlay_AREA13_0082CAB0[1] = 192.9f;
                    D_overlay_AREA13_0082CAB0[2] = 1276.4f;
                    D_overlay_AREA13_0082CAC0[0] = 725.7f;
                    D_overlay_AREA13_0082CAC0[1] = 174.6f;
                    D_overlay_AREA13_0082CAC0[2] = 1262.6f;
                    D_overlay_AREA13_0082CB00[0] = 719.8f;
                    D_overlay_AREA13_0082CB00[1] = 160.5f;
                    D_overlay_AREA13_0082CB00[2] = 1252.3f;
                } else {
                    D_overlay_AREA13_0082CAB0[0] = 1098.5f;
                    D_overlay_AREA13_0082CAB0[1] = 188.9f;
                    D_overlay_AREA13_0082CAB0[2] = 854.0f;
                    D_overlay_AREA13_0082CAC0[0] = 1081.5f;
                    D_overlay_AREA13_0082CAC0[1] = 173.2f;
                    D_overlay_AREA13_0082CAC0[2] = 850.9f;
                    D_overlay_AREA13_0082CB00[0] = 1071.5f;
                    D_overlay_AREA13_0082CB00[1] = 160.5f;
                    D_overlay_AREA13_0082CB00[2] = 845.2f;
                }
                *(short *)(self + 0x28) = 0;
                func_001BA1A0(blk, D_overlay_AREA13_0082CA50);
                self[5]++;
            }
            break;
        case 1:
            *(short *)(blk + 0xE) = func_001C64F0(self, 1.0f);
            *(short *)(self + 0x28) += 1;
            if (func_001BA1F0(self) != 0) {
                func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 0xD));
                func_001C62C0(self);
                func_001C6380(self);
                self[4] = 2;
                self[2] = 4;
                if (!(*(float *)(self + 0xB8) <= 1000.0f)) {
                    D_00810839 |= 1;
                } else {
                    D_00810839 |= 2;
                }
                return;
            }
            if (*(short *)(self + 0x28) == 4) {
                D_overlay_AREA13_0082CA00[5] = 0.0f;
                D_overlay_AREA13_0082CA00[3] = 0.0f;
                D_overlay_AREA13_0082CA00[1] = 160.5f;
                if (!(*(float *)(self + 0xB8) <= 1000.0f)) {
                    D_overlay_AREA13_0082CA00[0] = 719.8f;
                    D_overlay_AREA13_0082CA00[2] = 1252.3f;
                    D_overlay_AREA13_0082CA00[4] = 0.031415924f;
                } else {
                    D_overlay_AREA13_0082CA00[0] = 1071.5f;
                    D_overlay_AREA13_0082CA00[2] = 845.2f;
                    D_overlay_AREA13_0082CA00[4] = 1.5865042f;
                }
            }
            break;
        }
        func_001C68C0(self);
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        *(int *)0x700038B0 = 0x80;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x80;
        *(float *)0x700038A4 = *(float *)(self + 0x2EC);
        *(float *)0x700038AC = 1.0f;
        *(float *)0x700038A0 = *(float *)(self + 0x2E8);
        *(float *)0x700038A8 = *(float *)(self + 0x2E4);
        func_001F4BF0(D_700038A0, D_700038B0);
        *(float *)0x700038A0 = *(float *)(self + 0x2E0);
        *(float *)0x700038A8 = *(float *)(self + 0x2DC);
        func_001F4BF0(D_700038A0, D_700038B0);
        *(float *)0x700038A0 = *(float *)(self + 0x2D8);
        *(float *)0x700038A8 = *(float *)(self + 0x2D4);
        func_001F4BF0(D_700038A0, D_700038B0);
        *(float *)0x700038A0 = *(float *)(self + 0x2D0);
        *(float *)0x700038A8 = *(float *)(self + 0x2CC);
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 2:
        self[1] = func_001B1630(*(float *)(self + 0xB0), *(float *)(self + 0xB4), *(float *)(self + 0xB8));
        if (self[1] != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
