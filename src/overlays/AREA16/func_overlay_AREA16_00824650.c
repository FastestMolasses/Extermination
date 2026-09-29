// NEARMISS func_overlay_AREA16_00824650 (99.04%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00824690 (splat/link name 00824650; overlay code is
// linked 0x40 below where it runs), 0x778 bytes.
// Role (read from the instructions): state 0: +0 = +8 = 1, D_00810809 = 0xFF
//  when D_00810789 == 0xFF; by D_00810809 (1/other: state 1; 2: +0xD = 0x2C,
//  +0xE from the D_0024D7C0 record table, state 1; 0xFF: state 2, each after
//  func_001B0FD0 / func_001C6380); makes func_001C5570 attachments 0x31 / 0x33
//  (and 0x32 in state 1, then 0x8256E0(self, 0)) through func_001C5570, else
//  +0x2E8 = 0 and 0x825730. State 1: +5 0 when D_008104C4 is this object and
//  D_00810809 < 2 starts script 0x829970, zeroes the colour of the three
//  attachments, 0x825730, +5 = 1 and gives the fifteen +0x1C chain members
//  +0x2E4 = 0x2D0, +0x52 = index; otherwise draws a marker (func_001F4E20 at
//  (345, 113.28, 258.50)). When +5 is 1 and the script ends with D_00810809 1
//  or 2: func_001FAE70(0), restores the attachment colours, retires 0x32,
//  spawns a class 9 object (0x825260 behaviour) and a class 4 object
//  (func_001C4820 behaviour), +0x2E = 0xFFFF, +0xD = 0x30, +0xE from the
//  record table, +5 = 0, func_001C4760(0x1E, 1), D_00810809 = D_00810789 =
//  0xFF, func_001AF800, func_001CB5B0(+9), state 2 after func_001B0FD0. Then
//  func_001B1B70 / +0x4C unless D_00810789 == 1 with D_70003B92 set. State 2
//  animates and draws the marker. State 3/other: 0x825730, func_001AFC10.
// Divergence: 0x10 bytes short: the original keeps an explicit compare for
//  D_00810809 == 1 whose target is the default body (mwcc 2.3.3 merges case 1
//  into default for every label order tried), and pads the D_70003B88 loop
//  body with one nop before the test. The multiset audit is clean.
/* 0x28-byte records; the halfword reads are at +0x9A * 0x28 + 0x2E and + 6 */
typedef struct { char pad[6]; unsigned short id; char pad2[0x20]; } Ent;
extern Ent **D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810789;
extern unsigned char D_00810809;
extern unsigned char *D_008104C4;
extern unsigned char D_70003B92;
extern short D_70003B88;
extern float D_700038A0[];
extern int D_700038B0[];
extern char D_overlay_AREA16_00829970[];
extern char D_overlay_AREA16_00825260[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001C5570(unsigned char *self, float *pos, int kind, int a3);
extern void func_overlay_AREA16_008256E0(unsigned char *self, int idx);
extern void func_overlay_AREA16_00825730(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001F4E20(void *pos, void *col, float f12);
extern void func_001FAE70(int a);
extern unsigned char *func_001AFA90(int kind);
extern void func_001C4820(unsigned char *self);
extern void func_001C4760(int a0, int a1);
extern void func_001AF800(unsigned char *self);
extern void func_001CB5B0(int a0);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00824650(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    unsigned char *c;
    unsigned char **p;
    unsigned char *o;
    int d;
    switch (self[4]) {
    case 0:
        self[0] = 1;
        self[8] = 1;
        if (D_00810789 == 0xFF) {
            D_00810809 = 0xFF;
        }
        switch (D_00810809) {
        case 1:
        default:
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 1;
            }
            break;
        case 2:
            self[0xD] = 0x2C;
            *(unsigned short *)(self + 0xE) = D_0024D7C0[D_00810700][D_00810701][self[0x9A] + 1].id;
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 1;
            }
            break;
        case 0xFF:
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 2;
            }
            break;
        }
        D_700038A0[0] = 1.0f;
        D_700038A0[1] = 1.0f;
        D_700038A0[2] = 1.0f;
        D_700038A0[3] = 0.25f;
        *(unsigned char **)(self + 0x2EC) = func_001C5570(self, D_700038A0, 0x31, 0);
        *(unsigned char **)(self + 0x2E4) = func_001C5570(self, D_700038A0, 0x33, 0);
        if (self[4] == 1) {
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 1.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 0.25f;
            *(unsigned char **)(self + 0x2E8) = func_001C5570(self, D_700038A0, 0x32, 0);
            func_overlay_AREA16_008256E0(self, 0);
        } else {
            *(int *)(self + 0x2E8) = 0;
            func_overlay_AREA16_00825730(self);
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_008104C4 == self && D_00810809 < 2) {
                D_00810809 = 1;
                D_00810789 = 1;
                func_001BA1A0(blk, D_overlay_AREA16_00829970);
                c = *(unsigned char **)(self + 0x2EC);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3F);
                if (c != 0) {
                    *(float *)(c + 0xA0) = 0.0f;
                    *(float *)(*p + 0xA4) = 0.0f;
                    *(float *)(*p + 0xA8) = 0.0f;
                    *(float *)(*p + 0xAC) = 0.0f;
                }
                c = *(unsigned char **)(self + 0x2E8);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3E);
                if (c != 0) {
                    *(float *)(c + 0xA0) = 0.0f;
                    *(float *)(*p + 0xA4) = 0.0f;
                    *(float *)(*p + 0xA8) = 0.0f;
                    *(float *)(*p + 0xAC) = 0.0f;
                }
                c = *(unsigned char **)(self + 0x2E4);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3D);
                if (c != 0) {
                    *(float *)(c + 0xA0) = 0.0f;
                    *(float *)(*p + 0xA4) = 0.0f;
                    *(float *)(*p + 0xA8) = 0.0f;
                    *(float *)(*p + 0xAC) = 0.0f;
                }
                func_overlay_AREA16_00825730(self);
                self[5]++;
                o = *(unsigned char **)(self + 0x1C);
                for (D_70003B88 = 0; D_70003B88 < 15; D_70003B88++) {
                    *(int *)(o + 0x2E4) = 0x2D0;
                    *(unsigned short *)(o + 0x52) = D_70003B88;
                    o = *(unsigned char **)(o + 0x1C);
                }
                goto tail;
            }
            D_700038A0[0] = 344.9997f;
            D_700038A0[1] = 113.2757f;
            D_700038A0[2] = 258.5011f;
            D_700038A0[3] = 1.0f;
            D_700038B0[0] = 0x80;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0x66;
            D_700038B0[3] = 0x80;
            func_001F4E20(D_700038A0, D_700038B0, 8.0f);
            break;
        case 1:
            break;
        }
        if (self[5] == 1 && func_001BA1F0(self) != 0) {
            if (D_00810809 == 1 || D_00810809 == 2) {
                func_001FAE70(0);
                c = *(unsigned char **)(self + 0x2EC);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3F);
                if (c != 0) {
                    *(float *)(c + 0xA0) = 1.0f;
                    *(float *)(*p + 0xA4) = 1.0f;
                    *(float *)(*p + 0xA8) = 1.0f;
                    *(float *)(*p + 0xAC) = 0.0f;
                }
                c = *(unsigned char **)(self + 0x2E8);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3E);
                if (c != 0) {
                    c[4] = 3;
                    *p = 0;
                }
                c = *(unsigned char **)(self + 0x2E4);
                p = (unsigned char **)((int *)(self + 0x1F0) + 0x3D);
                if (c != 0) {
                    *(float *)(c + 0xA0) = 1.0f;
                    *(float *)(*p + 0xA4) = 1.0f;
                    *(float *)(*p + 0xA8) = 1.0f;
                    *(float *)(*p + 0xAC) = 0.0f;
                }
                o = func_001AFA90(9);
                if (o != 0) {
                    o[0x9A] = 0;
                    o[3] = 0;
                    *(short *)(o + 0x2E) = 0;
                    o[0xD] = 0x68;
                    *(unsigned short *)(o + 0xE) = 0xFFFF;
                    *(short *)(o + 0x54) = 0;
                    *(short *)(o + 0x56) = 0;
                    *(float *)(o + 0xB0) = 371.6428f;
                    *(float *)(o + 0xB4) = 101.7f;
                    *(float *)(o + 0xB8) = 250.0218f;
                    *(float *)(o + 0xC0) = 0.0f;
                    *(float *)(o + 0xC4) = -1.5358899f;
                    *(float *)(o + 0xC8) = 0.0f;
                    *(char **)(o + 0x10) = D_overlay_AREA16_00825260;
                }
                o = func_001AFA90(4);
                if (o != 0) {
                    o[0x9A] = 0;
                    o[3] = 0;
                    *(short *)(o + 0x2E) = 0;
                    o[0xD] = 0xE;
                    *(unsigned short *)(o + 0xE) = 0xFFFF;
                    *(short *)(o + 0x54) = 0;
                    *(short *)(o + 0x56) = 0;
                    *(float *)(o + 0xB0) = 371.41f;
                    *(float *)(o + 0xB4) = 101.3f;
                    *(float *)(o + 0xB8) = 256.84f;
                    *(float *)(o + 0xC0) = 1.5725416f;
                    *(float *)(o + 0xC4) = 0.30543265f;
                    *(float *)(o + 0xC8) = -0.034906585f;
                    *(void (**)(unsigned char *))(o + 0x10) = func_001C4820;
                }
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[0xD] = 0x30;
                *(unsigned short *)(self + 0xE) = D_0024D7C0[D_00810700][D_00810701][self[0x9A]].id;
                self[5] = 0;
                func_001C4760(0x1E, 1);
                D_00810809 = 0xFF;
                D_00810789 = 0xFF;
                func_001AF800(self);
                func_001CB5B0(self[9]);
                if (func_001B0FD0(self) == 0) {
                    func_001C6380(self);
                    self[4] = 2;
                }
            }
        }
    tail:
        d = D_00810789;
        if (d != 1 || D_70003B92 == 0 || d == 0xFF) {
            func_001B1B70(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        D_700038A0[0] = 344.9997f;
        D_700038A0[1] = 113.2757f;
        D_700038A0[2] = 258.5011f;
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 0x80;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0x66;
        D_700038B0[3] = 0x80;
        func_001F4E20(D_700038A0, D_700038B0, 8.0f);
        break;
    case 3:
    default:
        func_overlay_AREA16_00825730(self);
        func_001AFC10(self);
        break;
    }
}
