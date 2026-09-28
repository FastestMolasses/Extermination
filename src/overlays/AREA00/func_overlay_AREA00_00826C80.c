// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00826CC0 (splat/link name 00826C80; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: +0x64 starts at 1.0 or 0.0 (D_00810803); state 1: +5 0 waits for
//  D_00810803 == 2 (+0x28 = 30), +5 1 counts down then func_001EFD20(4, ...),
//  +5 2 raises +0x64 by 0.02/0.008 to 1.0, plays sound 0x9F every 10 counts
//  and jitters +0xB0/+0xB8; state 3 when D_00810803 is 0xFF.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810803;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001EFD20(int a, void *b);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826C80(unsigned char *self) {
    int buf[4];
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        if (D_00810803 != 0) {
            *(float *)(self + 0x64) = 1.0f;
        } else {
            *(float *)(self + 0x64) = 0.0f;
        }
        func_001C6380(self);
        break;
    case 1:
        if (D_00810803 == 0xFF) {
            self[4] = 3;
        }
        switch (self[5]) {
        case 0:
            if (D_00810803 == 2) {
                *(short *)(self + 0x28) = 30;
                *(short *)(self + 0x2A) = 0;
                self[5] = 1;
            }
            break;
        case 1:
            if (*(short *)(self + 0x28) == 0) {
                func_001EFD20(4, buf);
                self[5] = 2;
            }
            (*(short *)(self + 0x28))--;
            break;
        case 2:
            (*(short *)(self + 0x2A))++;
            if (*(float *)(self + 0x64) <= 1.0f) {
                if (*(short *)(self + 0x2A) & 8) {
                    *(float *)(self + 0x64) += 0.02f;
                } else {
                    *(float *)(self + 0x64) += 0.008f;
                }
            } else {
                self[5] = 3;
            }
            if (*(short *)(self + 0x2A) % 10 == 0) {
                func_001FBD50(self, 0x9F, 0, 300.0f);
            }
            if (*(short *)(self + 0x2A) & 2) {
                *(float *)(self + 0xB0) -= 0.5f;
                *(float *)(self + 0xB8) += 0.8f;
            } else {
                *(float *)(self + 0xB0) += 0.5f;
                *(float *)(self + 0xB8) -= 0.8f;
            }
            break;
        case 3:
            break;
        }
        func_001C6380(self);
        if (D_00810803 != 0) {
            func_001B1B70(self);
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
