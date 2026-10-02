// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Actor sub-state handler (sub-step byte self+6). The wall / ledge probe
// func_001C2770(self, ctl, 2) runs first. Step 0 advances the step, sets the
// timer ctl+0xD0 = 60 + (rand & 0x30), ctl+0xD8 = 0.6 and starts
// func_001287F0(self, ctl, 6, 0.0). Step 1 counts the timer down; at zero it
// copies self+0xB0 to ctl+0x50, sets ctl+0xEC = 1.0, ctl+0xD8 = 0 and self+5 = 0.
// When the probe returned 0, func_001C3D60(self, ctl) runs last.
extern int func_001C2770(char *p, char *q, int a);
extern int func_00122BB8(void);
extern void func_001287F0(char *a, char *b, int c, float f);
extern void func_00102948(void *dst, void *src);
extern void func_001C3D60(char *a, char *b);

void func_0012D850(char *self, char *ctl) {
    int hung = func_001C2770(self, ctl, 2);
    unsigned char st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(short *)(ctl + 0xD0) = (func_00122BB8() & 0x30) + 0x3C;
        *(float *)(ctl + 0xD8) = 0.6f;
        func_001287F0(self, ctl, 6, 0.0f);
        break;
    case 1:
        if (--*(short *)(ctl + 0xD0) == 0) {
            func_00102948(ctl + 0x50, self + 0xB0);
            *(float *)(ctl + 0xEC) = 1.0f;
            *(float *)(ctl + 0xD8) = 0.0f;
            *(unsigned char *)(self + 5) = 0;
        }
        break;
    }
    if (hung == 0) {
        func_001C3D60(self, ctl);
    }
}
