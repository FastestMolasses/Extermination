// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Entity state-machine tick: dispatches on the mode byte at self+6 (states 0-7, 0xA-0xD, 0x14)
// driving spawn/anim/timer/movement sub-steps, then re-checks mode<3 with the 0x5D override at
// self+0x23A. D_00275B40 points at a global clock struct (+4/+8 = two running clocks, used for
// per-state delta-time accumulation).
extern void func_001B61C0(int a, int b, int c, int d);
extern int func_001FBD50(char *e, int a, int b, float f);
extern int func_001749A0(char *e, int clip, int flags, float blend);
extern int func_00178B90(char *e, int a);
extern int func_00175900(char *e, int a);
extern int func_00179880(char *e, char *p);
extern int func_0021C190(char *e);
extern void func_0021C120(char *e);
extern void func_0021C200(char *e);
extern void func_0017C540(char *e);
extern void func_0021D2E0(char *e, int a, int b);
extern void func_0021D490(char *e);
extern void func_0021D250(char *e, int a);
extern int *D_00275B40;
extern unsigned char D_008106F1[16];

void func_00221060(char *self) {
    unsigned char mode;

    mode = *(unsigned char *)(self + 6);
    switch (mode) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(self, 0x150, 0, 300.0f);
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        func_001749A0(self, 0x26, 0, 1.0f);
        *(int *)(self + 0x21C) = 0;
        *(int *)(self + 0x2E4) = 0;
        break;
    case 1:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        } else {
            *(float *)(self + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(*D_00275B40 + 8);
            *(float *)(self + 0x38) *= 0.75f;
            func_00178B90(self, 1);
            *(float *)(self + 0x2EC) = *(float *)(*D_00275B40 + 4) - *(float *)(self + 0x2E4);
            *(float *)(self + 0x2E4) = *(float *)(*D_00275B40 + 4);
            *(float *)(self + 0xB4) += *(float *)(self + 0x2EC);
            func_00175900(self, 1);
        }
        break;
    case 2:
        func_00178B90(self, 1);
        func_00179880(self, self + 0x2EC);
        if (func_00175900(self, 1) != 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            func_001FBD50(self, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
            func_001749A0(self, 0x27, 0, 0.0f);
            *(int *)(self + 0x21C) = 0;
            *(int *)(self + 0x2E4) = 0;
        }
        break;
    case 3:
        if (*(int *)(self + 0x200) & 0x1000) {
            if (*(float *)(self + 0x220) <= 0.0f) {
                if ((*(unsigned char *)(self + 0xF) == 0x63) || (*(unsigned char *)(self + 0x234) == 1)) {
                    func_0021C200(self);
                    *(unsigned char *)(self + 6) = 0x14;
                    func_001749A0(self, 0x1C4, 0, 16.0f);
                } else {
                    *(unsigned char *)(self + 6) = 0xA;
                    func_001749A0(self, 0x29, 0, 4.0f);
                }
            } else if (!(*(float *)(self + 0x228) < 100.0f) && (D_008106F1[0] != 0)) {
                *(unsigned char *)(self + 6) = mode + 1;
                func_0021C120(self);
                *(unsigned char *)(self + 0x302) = 1;
            } else {
                *(unsigned char *)(self + 6) = *(volatile unsigned char *)(self + 6) + 2;
                *(unsigned char *)(self + 0x302) = 0;
            }
        } else {
            *(float *)(self + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(self, 1);
            func_00175900(self, 1);
        }
        break;
    case 4:
        if (func_0021C190(self) != 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        }
        break;
    case 5:
        *(unsigned char *)(self + 6) = mode + 1;
        func_001749A0(self, 0x28, 0, 8.0f);
        *(int *)(self + 0x38) = 0;
        *(int *)(self + 0x21C) = 0;
        break;
    case 6:
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(unsigned char *)(self + 6) = mode + 1;
            *(int *)(self + 0x2EC) = 0;
            *(char *)(self + 0x1F1) = 0;
        }
        break;
    case 7:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 0xF) = 0;
            *(int *)(self + 0x224) = 0;
            *(int *)(self + 0x22C) = 0;
            *(short *)(self + 0x20E) = 0x3C;
            *(char *)(self + 0x25C) = 0;
            func_0017C540(self);
        } else {
            *(float *)(self + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(self, 1);
            if (*(unsigned char *)(self + 0x302) != 0) {
                *(int *)(self + 0x204) = 0x3ECCCCCD;
            }
        }
        func_00179880(self, self + 0x2EC);
        func_00175900(self, 1);
        break;
    case 0xA:
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(unsigned char *)(self + 6) = mode + 1;
            func_001FBD50(self, 0x156, 0, 300.0f);
        }
        break;
    case 0xB:
        if (*(float *)(self + 0x3C) <= 18.0f) {
            *(unsigned char *)(self + 6) = mode + 1;
            func_0021D490(self);
        }
        break;
    case 0xC:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = mode + 1;
            *(char *)(self + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 0xD:
        func_0021D2E0(self, 0x78, 0);
        break;
    case 0x14:
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(char *)(self + 4) = 2;
            *(char *)(self + 5) = 3;
            *(unsigned char *)(self + 6) = 0;
            *(char *)(self + 0x1F0) = 0x3F;
        } else {
            *(float *)(self + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(self, 1);
        }
        break;
    }
    if ((*(unsigned char *)(self + 6) < 3) && (*(unsigned char *)(self + 0x23A) == 0x5D)) {
        func_0021D250(self, 1);
    }
}
