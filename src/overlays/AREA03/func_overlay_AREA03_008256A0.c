// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x008256E0 (splat/link name 008256A0; overlay code
//  is linked 0x40 below where it runs), 0x298 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 0 placement [8]; 0x825430 without the 1.5 scale and with every
//  D_00275CA0 test inverted (the -5 / +5 position belongs to D_00275CA0 ==
//  0).
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

void func_overlay_AREA03_008256A0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        self[0] = 1;
        if (D_00275CA0 == 0) {
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
            if (D_00275CA0 != 0) {
                func_001B17A0(self);
            }
            if (W->f1F0 != 0 && D_00275CA0 == 0) {
                self[5] = 1;
                W->f1F4 = 0.0f;
            } else if (W->f1F0 == 0 && D_00275CA0 != 0) {
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
