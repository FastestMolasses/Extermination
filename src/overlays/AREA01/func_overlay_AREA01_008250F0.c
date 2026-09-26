// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825130 (splat/link name 008250F0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: op09 callback of script 0x829FA0 (records 0x82A020/0x82A420); picks
//  clip 5/6 by func_001B1380, then turns toward D_00810350.
extern int func_001B1380(void *a, void *b, float c);
extern void func_001C67E0(char *self, int clip, float blend, float frame);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float D_00810350[8];

int func_overlay_AREA01_008250F0(char *self, unsigned char *st, char *prm) {
    float goal;
    float r;
    switch (st[4]) {
    case 0:
        if (func_001B1380(D_00810350, self + 0xB0, *(float *)(self + 0xC4)) != 0) {
            func_001C67E0(self, 6, 20.0f, 0.0f);
        } else {
            func_001C67E0(self, 5, 20.0f, 0.0f);
        }
        st[4] = 1;
        break;
    case 1:
        goal = func_001B1240(self + 0xB0, D_00810350[4], D_00810350[6]);
        r = func_001B12B0(goal, *(float *)(self + 0xC4), *(float *)(prm + 0xC));
        *(float *)(self + 0xC4) = r;
        if (r == goal) {
            return 1;
        }
        break;
    }
    return 0;
}
