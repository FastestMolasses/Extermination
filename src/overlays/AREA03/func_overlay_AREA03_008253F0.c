// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00825430 (splat/link name 008253F0; overlay code
//  is linked 0x40 below where it runs), 0x2A8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 0 placement [7]. State 0: +0x80/84/88 = 1.5, +0 = 1; with
//  D_00275CA0 set the +0x7C of the objects at *(D_00275B40 + 4) / + 8 are
//  -5 / +5 and +0x1F0 = 0, otherwise +0x1F0 = 1; func_001C6380. State 1, +5
//  0: func_001B17A0 while D_00275CA0 is 0; +5 = 1 with +0x1F4 = 0 when
//  +0x1F0 and D_00275CA0 are both set, or with +0x1F4 = 60 when both are
//  clear. +5 1: +0x1F4 counts up (+0x1F0 set) or down by 1 a frame and the
//  two +0x7C values are -/+ +0x1F4 / 12; at 60 they are -5 / +5, at 0 they
//  are 0, then +5 = 0 and +0x1F0 flips. The +0x4C method every frame of
//  state 1. Other states: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
typedef struct {
    int f1F0;
    float f1F4;
} Work;
#define W ((Work *)(self + 0x1F0))
#define REC(o) (*(unsigned char **)(D_00275B40 + (o)))
extern char *D_00275B40;
extern int D_00275CA0;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_008253F0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        *(float *)(self + 0x80) = 1.5f;
        *(float *)(self + 0x84) = 1.5f;
        *(float *)(self + 0x88) = 1.5f;
        self[0] = 1;
        if (D_00275CA0 != 0) {
            *(float *)(REC(4) + 0x7C) = -5.0f;
            *(float *)(REC(8) + 0x7C) = 5.0f;
            W->f1F0 = 0;
        } else {
            W->f1F0 = 1;
        }
        func_001C6380(self);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00275CA0 == 0) {
                func_001B17A0(self);
            }
            if (W->f1F0 != 0 && D_00275CA0 != 0) {
                self[5] = 1;
                W->f1F4 = 0.0f;
            } else if (W->f1F0 == 0 && D_00275CA0 == 0) {
                self[5] = 1;
                W->f1F4 = 60.0f;
            }
            break;
        case 1:
            if (W->f1F0 != 0) {
                W->f1F4 += 1.0f;
                if (W->f1F4 < 60.0f) {
                    *(float *)(REC(4) + 0x7C) = -(0.083333336f * W->f1F4);
                    *(float *)(REC(8) + 0x7C) = 0.083333336f * W->f1F4;
                } else {
                    self[5] = 0;
                    W->f1F0 = !W->f1F0;
                    *(float *)(REC(4) + 0x7C) = -5.0f;
                    *(float *)(REC(8) + 0x7C) = 5.0f;
                }
            } else {
                W->f1F4 -= 1.0f;
                if (W->f1F4 > 0.0f) {
                    *(float *)(REC(4) + 0x7C) = -(0.083333336f * W->f1F4);
                    *(float *)(REC(8) + 0x7C) = 0.083333336f * W->f1F4;
                } else {
                    self[5] = 0;
                    W->f1F0 = !W->f1F0;
                    *(float *)(REC(4) + 0x7C) = 0.0f;
                    *(float *)(REC(8) + 0x7C) = 0.0f;
                }
            }
            func_001C6380(self);
            break;
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
