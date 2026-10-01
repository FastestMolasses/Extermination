// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827E70 (splat/link name 00827E30; overlay code
//  is linked 0x40 below where it runs), 0x4D4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placements [59], [60]. As 0x827A30 with a 220-frame count,
//  y 0 for the three effects and sound 0x449, advancing +5; +5 3 copies its
//  pose from its placement record (+0xE, +0xC0..C8, +0xB0..B8; the AREA13
//  record idiom), func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 9)),
//  func_001C62C0, func_001C6380; every frame of state 1 ends with
//  func_001B1B70 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
#define REC ((unsigned char *)(self[0x9A] * 0x28 + (int)D_0024D7C0[D_00810700[0]][D_00810701[0]]))
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_0081080E[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char **D_0024D7C0[];
extern int func_001C6120(int a0, int a1);
extern void func_001CA6E0(unsigned char *self, int a1);
extern void func_001C62C0(unsigned char *self);
extern float D_700038A0[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001EFD90(int id, void *pos, void *dir);
extern void func_001EFD20(int id, void *pos);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001F02C0(void *pos, int id, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827E30(unsigned char *self) {
    int i;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[5] = 0;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081080E[0] == 0x10) {
                S16(0x28) = 0;
                self[5]++;
            }
            break;
        case 1:
            S16(0x28)++;
            if (S16(0x28) >= 0xDC) {
                self[5]++;
            }
            break;
        case 2:
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    D_700038A0[0] = -25.0f;
                    D_700038A0[1] = 0.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                case 1:
                    D_700038A0[0] = 0.0f;
                    D_700038A0[1] = 0.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                case 2:
                    D_700038A0[0] = 25.0f;
                    D_700038A0[1] = 0.0f;
                    D_700038A0[2] = 0.0f;
                    D_700038A0[3] = 1.0f;
                    break;
                }
                func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
                func_001EFD20(0x8000002F, D_700038A0);
            }
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 3.1415927f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001EFD90(0x8000005E, self + 0xB0, D_700038A0);
            self[5]++;
            func_001F02C0(self + 0xB0, 0x449, 800.0f);
            break;
        case 3:
            *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
            *(float *)(self + 0xC0) = *(float *)(REC + 0x40);
            *(float *)(self + 0xC4) = *(float *)(REC + 0x44);
            *(float *)(self + 0xC8) = *(float *)(REC + 0x48);
            *(float *)(self + 0xB0) = *(float *)(REC + 0x34);
            *(float *)(self + 0xB4) = *(float *)(REC + 0x38);
            *(float *)(self + 0xB8) = *(float *)(REC + 0x3C);
            func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 9));
            func_001C62C0(self);
            func_001C6380(self);
            self[5]++;
            break;
        case 4:
            break;
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
