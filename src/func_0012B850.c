// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Actor sub-state handler (sub-step byte self+6). The wall / ledge probe
// func_001C2770(self, ctl, 0) runs first. Step 0 sets ctl+0xD0 = 120 frames,
// ctl+0xD8 = 0.3, ctl+0xE8 = self+0xC4, starts func_001287F0(self, ctl, 6, 8.0)
// and falls into step 1. Step 1 counts ctl+0xD0 down; at zero the step advances
// and ctl+0xD8 = 0. Step 2 waits until func_00128640(self) returns 0, then copies
// self+0xB0 to ctl+0x50 and sets self+5 = 1, +6 = 0, +7 = 0. When the probe
// returned 0, func_001C3D60(self, ctl) runs last.
extern int func_001C2770(char *p, char *q, int a);
extern int func_00128640(char *p);
extern void func_001287F0(char *a, char *b, int c, float f);
extern void func_00102948(void *dst, void *src);
extern void func_001C3D60(char *a, char *b);

void func_0012B850(char *self, char *ctl) {
    int hung = func_001C2770(self, ctl, 0);
    switch (*(unsigned char *)(self + 6)) {
    case 0:
        *(short *)(ctl + 0xD0) = 0x78;
        *(unsigned char *)(self + 6) += 1;
        *(float *)(ctl + 0xD8) = 0.3f;
        *(float *)(ctl + 0xE8) = *(float *)(self + 0xC4);
        func_001287F0(self, ctl, 6, 8.0f);
    case 1:
        if (--*(short *)(ctl + 0xD0) == 0) {
            *(unsigned char *)(self + 6) += 1;
            *(float *)(ctl + 0xD8) = 0.0f;
        }
        break;
    case 2:
        if (func_00128640(self) == 0) {
            func_00102948(ctl + 0x50, self + 0xB0);
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
            *(unsigned char *)(self + 7) = 0;
        }
        break;
    }
    if (hung == 0) {
        func_001C3D60(self, ctl);
    }
}
