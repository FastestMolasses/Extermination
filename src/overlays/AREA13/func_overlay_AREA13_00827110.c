// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00827150 (splat/link name 00827110; overlay code
//  is linked 0x40 below where it runs), 0xADC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placements [45] / [46]. D_00275CA8 = self + 0x1F0 every frame
//  (the block: [0] step, [1] count, [2] count limit, [3] markers left, [4] /
//  [5] step-4 state, [6] blink bit, [7] latch). State 0: func_001B0FD0,
//  clears the block ([3] = 4, [6] = -1); when D_00810774 == 0xFF: +0xD =
//  0x12, pose from the placement record, model func_001C6120(D_0028A59C,
//  0x12), state 1. State 1 for +0xD 0x11: +5 = 3 when D_008107F4 == 0xFF; +5
//  0 sets +0x40 = D_0028A6EC and func_001C63E0(self, 0); +5 1 waits for
//  D_008107F4 bit 1 (then ORs 0x10); +5 2 ORs 0x40 into D_008107F4 and calls
//  the step table 0x82D190[step] (0x827C30, 0x827DD0, 0x827E00, 0x827F20,
//  0x827F90), advancing the step when the count reaches the limit; +5 3
//  reloads the record pose and model; +5 4 func_001C6380. Then func_001B1B70,
//  the +0x4C method and one model-0x3F5 marker plus a func_001F4E20 sprite
//  for each of the last [3] of four points (marker 0: size 5 while [6] == -1,
//  size 7 while bit [6] of D_70003B68 is set, with func_001F02C0(.., 0x8D2,
//  500) once); while the step is below 4, func_001EFD20(5, ..) every 16
//  frames and twelve func_001F4E20 sprites around +0xB0. +0xD 0x12: while
//  D_00810774 == 0xFF func_001C6380, func_001B1B70 and the +0x4C method.
//  States 2/3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
typedef struct { float x, y, z, w; } Vec4;
#define REC ((unsigned char *)(self[0x9A] * 0x28 + (int)D_0024D7C0[D_00810700[0]][D_00810701[0]]))
extern int *D_00275CA8;
extern unsigned char D_008107F4[];
extern unsigned char D_00810774[];
extern int D_70003B68[];
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern float D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];
extern ActorFn D_overlay_AREA13_0082D190[];
extern Vec4 D_overlay_AREA13_0082D1B0[];
extern Vec4 D_overlay_AREA13_0082D1F0[];
extern Vec4 D_overlay_AREA13_0082D2F0[];
extern void func_001B0FD0(unsigned char *self);
extern int func_001C6120(int a, int b);
extern void func_001CA6E0(unsigned char *self, int v);
extern void func_001C62C0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001C68C0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102A60(void *dst, void *src, float a);
extern void func_00102918(void *a, void *b, void *c);
extern int func_001CA7B0(void *m, float r);
extern void func_001C7900(void *m, void *c, int id, int a3);
extern void func_001CA940(int h, int v);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F4E20(void *pos, void *col, float size);
extern void func_001F02C0(void *pos, int id, float vol);
extern void func_001EFD20(unsigned int msg, void *pos);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00827110(unsigned char *self) {
    int i;
    int h;
    unsigned char f;
    D_00275CA8 = (int *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        *(short *)(self + 0x28) = 0;
        *(short *)(self + 0x2A) = 0;
        D_00275CA8[0] = 0;
        D_00275CA8[1] = 0;
        D_00275CA8[3] = 4;
        D_00275CA8[4] = 0;
        D_00275CA8[5] = 0;
        D_00275CA8[6] = -1;
        D_00275CA8[7] = 0;
        if (*(unsigned char *)0x810774 == 0xFF) {
            self[0xD] = 0x12;
            *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
            *(float *)(self + 0xC0) = *(float *)(REC + 0x40);
            *(float *)(self + 0xC4) = *(float *)(REC + 0x44);
            *(float *)(self + 0xC8) = *(float *)(REC + 0x48);
            *(float *)(self + 0xB0) = *(float *)(REC + 0x34);
            *(float *)(self + 0xB4) = *(float *)(REC + 0x38);
            *(float *)(self + 0xB8) = *(float *)(REC + 0x3C);
            func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 0x12));
            func_001C62C0(self);
            func_001C6380(self);
            self[4] = 1;
        }
        break;
    case 1:
        switch (self[0xD]) {
        case 0x11:
            if (D_008107F4[0] == 0xFF) {
                self[5] = 3;
            }
            switch (self[5]) {
            case 0:
                self[5]++;
                *(int *)(self + 0x40) = *(int *)0x28A6EC;
                func_001C63E0(self, 0);
                func_001C68C0(self);
                break;
            case 1:
                f = D_008107F4[0];
                if (f & 2) {
                    *(unsigned char *)0x8107F4 = f | 0x10;
                    self[5]++;
                }
                func_001C68C0(self);
                break;
            case 2:
                D_008107F4[0] |= 0x40;
                D_overlay_AREA13_0082D190[D_00275CA8[0]](self);
                D_00275CA8[1]++;
                if (D_00275CA8[1] >= D_00275CA8[2]) {
                    D_00275CA8[0]++;
                    D_00275CA8[1] = 0;
                }
                func_001C68C0(self);
                break;
            case 3:
                self[5]++;
                *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
                *(float *)(self + 0xC0) = *(float *)(REC + 0x40);
                *(float *)(self + 0xC4) = *(float *)(REC + 0x44);
                *(float *)(self + 0xC8) = *(float *)(REC + 0x48);
                *(float *)(self + 0xB0) = *(float *)(REC + 0x34);
                *(float *)(self + 0xB4) = *(float *)(REC + 0x38);
                *(float *)(self + 0xB8) = *(float *)(REC + 0x3C);
                func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 0x12));
                func_001C62C0(self);
                func_001C6380(self);
                break;
            case 4:
                func_001C6380(self);
                break;
            }
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            *(float *)0x700038A0 = 1.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 1.0f;
            *(float *)0x700038AC = 1.0f;
            for (i = 4 - D_00275CA8[3]; i < 4; i++) {
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, D_overlay_AREA13_0082D2F0[i].x);
                func_00102BB0(D_700036A0, D_700036A0, D_overlay_AREA13_0082D2F0[i].y);
                func_00102A60(D_700036A0, D_700036A0, D_overlay_AREA13_0082D2F0[i].z);
                func_00102918(D_700036A0, D_700036A0, &D_overlay_AREA13_0082D1B0[i]);
                h = func_001CA7B0(D_700036D0, 10.0f);
                if (h >= 0) {
                    func_001C7900(D_700036A0, D_700038A0, 0x3F5, 0);
                    func_001CA940(h, func_001C6120(*(int *)0x28A59C, 0x1B));
                }
                D_700038C0[0] = 0.0f;
                *(int *)0x700038B0 = 0xFF;
                *(int *)0x700038B4 = 0;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
                *(float *)0x700038C4 = 3.5f;
                *(float *)0x700038C8 = 0.9015f;
                *(float *)0x700038CC = 1.0f;
                func_001026A0(D_700038C0, D_700036A0, D_700038C0);
                switch (i) {
                case 0:
                    if (D_00275CA8[6] == -1) {
                        func_001F4E20(D_700038C0, D_700038B0, 5.0f);
                    } else if ((D_70003B68[0] >> D_00275CA8[6]) & 1) {
                        func_001F4E20(D_700038C0, D_700038B0, 7.0f);
                        if (D_00275CA8[7] == 0) {
                            D_00275CA8[7] = 1;
                            func_001F02C0(D_700038C0, 0x8D2, 500.0f);
                        }
                    } else {
                        D_00275CA8[7] = 0;
                    }
                    break;
                default:
                    func_001F4E20(D_700038C0, D_700038B0, 5.0f);
                    break;
                }
            }
            if (D_00275CA8[0] < 4) {
                for (i = 0; i < 4 - D_00275CA8[3]; i++) {
                    if (!(D_70003B68[0] & 0xF)) {
                        func_001EFD20(5, &D_overlay_AREA13_0082D1B0[i]);
                    }
                }
                *(int *)0x700038B0 = 0xFF;
                *(int *)0x700038B4 = 0;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x80;
                for (i = 0; i < 12; i++) {
                    func_001028B8(D_700038A0, self + 0xB0, &D_overlay_AREA13_0082D1F0[i]);
                    func_001F4E20(D_700038A0, D_700038B0, 7.0f);
                }
            }
            break;
        case 0x12:
            if (D_00810774[0] == 0xFF) {
                func_001C6380(self);
                func_001B1B70(self);
                (*(ActorFn *)(self + 0x4C))(self);
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
