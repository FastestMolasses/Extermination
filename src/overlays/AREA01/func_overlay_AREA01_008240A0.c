// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008240E0 (splat/link name 008240A0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x823CD0 (O3 owner); per-state turn/wait machine driven
//  by +6, uses 0x823A90 as its range check.
// Covers the splat pieces 008240A0, 008240E0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern int func_001C2770(unsigned char *act, unsigned char *st, int flags);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern void func_001287F0(void *a0, void *a1, short a2, float f12);
extern float func_001B12B0(float goal, float cur, float rate);
extern int func_overlay_AREA01_00823A90(unsigned char *self, void *a1, void *a2);
extern void func_001C3D60(unsigned char *a, unsigned char *b);

void func_overlay_AREA01_008240A0(unsigned char *self, unsigned char *ev) {
    int res = func_001C2770(self, ev, 1);
    float r;
    short n;
    switch (self[6]) {
    case 0:
        *(short *)(self + 0x28) = 0;
        *(float *)(ev + 0xD8) = 0.0f;
        if (func_00122BB8() & 1) {
            *(short *)(ev + 0xD0) = 0x1E;
        } else {
            *(short *)(ev + 0xD0) = 0x3C;
        }
        *(float *)(ev + 0xE8) = func_001B1470(6.2831855f * (float)(func_00122BB8() & 0xF0) / 256.0f);
        self[6]++;
        func_001287F0(self, ev, 5, 8.0f);
        break;
    case 1:
        r = func_001B12B0(*(float *)(ev + 0xE8), *(float *)(self + 0xC4), 0.0698131695f);
        *(float *)(self + 0xC4) = r;
        if (r == *(float *)(ev + 0xE8)) {
            self[6]++;
            *(float *)(ev + 0xD8) = 0.4f;
        }
        break;
    case 2:
        if (func_overlay_AREA01_00823A90(self, self + 0xB0, ev + 0x50) != 0) {
            self[6] = 3;
            *(float *)(ev + 0xD8) = 0.0f;
        } else {
            n = *(short *)(ev + 0xD0) - 1;
            *(short *)(ev + 0xD0) = n;
            if (n == 0) {
                self[6] = 0;
                *(float *)(ev + 0xD8) = 0.0f;
            }
        }
        break;
    case 3:
        r = func_001B12B0(*(float *)(ev + 0xE8), *(float *)(self + 0xC4), 0.0698131695f);
        *(float *)(self + 0xC4) = r;
        if (r == *(float *)(ev + 0xE8)) {
            self[6]++;
            *(float *)(ev + 0xD8) = 0.4f;
            *(short *)(ev + 0xD0) = 0x14;
        }
        break;
    case 4:
        n = *(short *)(ev + 0xD0) - 1;
        *(short *)(ev + 0xD0) = n;
        if (n == 0) {
            self[6] = 2;
        }
        break;
    }
    if (res == 0 || res == 8) {
        func_001C3D60(self, ev);
    }
}
