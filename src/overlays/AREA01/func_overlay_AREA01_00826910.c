// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826950 (splat/link name 00826910; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: op09 callback of script 0x82B590 (record 0x82B650); moves the player
//  block D_008102B0 along the parameter path.
extern char D_008102B0[];
extern float D_70003600[4];
extern void func_00102948(void *dst, void *src);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00182F90(char *actor, void *target);

int func_overlay_AREA01_00826910(unsigned char *self, unsigned char *st, char *prm) {
    char *pl = D_008102B0;
    float goal;
    float r;
    float t;
    switch (st[4]) {
    case 0:
        func_00102948(prm + 0x20, pl + 0xA0);
        st[4]++;
        *(float *)(prm + 0x10) = 0.0f;
        *(short *)(pl + 0x1F2) = 2;
        pl[0x25C] = 2;
        *(float *)(pl + 0x1F8) = 4.0f;
        break;
    case 1:
        goal = func_001B1240(pl + 0xA0, *(float *)(prm + 0x30), *(float *)(prm + 0x38));
        r = func_001B12B0(goal, *(float *)(pl + 0xC4), 0.0698131695f);
        *(float *)(pl + 0xC4) = r;
        if (r != goal) {
            break;
        }
        st[4]++;
    case 2:
        if (!(*(float *)(prm + 0x10) < *(float *)(prm + 0xC))) {
            *(short *)(pl + 0x1F2) = 0;
            pl[0x25C] = 0;
            *(float *)(pl + 0x1F8) = 4.0f;
            return 1;
        }
        if (*(float *)(prm + 0x10) == 20.0f) {
            func_001FB9F0(0x4A, 0x1000, 0x1000, 0x1000);
        }
        if (*(float *)(prm + 0x10) == 40.0f) {
            func_001FB9F0(0x4A, 0x1000, 0x1000, 0x1000);
        }
        if (*(float *)(prm + 0x10) == 55.0f) {
            func_001FB9F0(0x4A, 0x1000, 0x1000, 0x1000);
        }
        *(float *)(prm + 0x10) += 1.0f;
        t = *(float *)(prm + 0x10) / *(float *)(prm + 0xC);
        func_001028D0(st + 0x10, prm + 0x30, prm + 0x20);
        *(float *)0x70003600 = *(float *)(prm + 0x20) + *(float *)(st + 0x10) * t;
        *(float *)0x70003604 = *(float *)(prm + 0x24) + *(float *)(st + 0x14) * t;
        *(float *)0x70003608 = *(float *)(prm + 0x28) + *(float *)(st + 0x18) * t;
        func_00182F90(pl, D_70003600);
        break;
    }
    return 0;
}
