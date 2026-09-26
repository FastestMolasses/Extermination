// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825040 (splat/link name 00825000; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x824340; two-state turn toward the player with a
//  120-frame timeout.
// Covers the splat pieces 00825000, 00825040 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern char *D_008106C0;
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);

int func_overlay_AREA01_00825000(unsigned char *self, char *prm) {
    char *pl = D_008106C0;
    short n;
    switch (self[7]) {
    case 0:
        pl = *(char **)(pl + 0x118);
        *(float *)(prm + 0xE8) = func_001B1240(self + 0xB0, *(float *)(pl + 0xC0), *(float *)(pl + 0xC8));
        self[7]++;
        *(short *)(prm + 0xD2) = 0x78;
        break;
    case 1:
        *(float *)(self + 0xC4) = func_001B12B0(*(float *)(prm + 0xE8), *(float *)(self + 0xC4), 0.139626339f);
        n = *(short *)(prm + 0xD2) - 1;
        *(short *)(prm + 0xD2) = n;
        if (n == 0) {
            return 1;
        }
        if (*(float *)(self + 0xC4) == *(float *)(prm + 0xE8)) {
            return 1;
        }
        break;
    }
    return 0;
}
