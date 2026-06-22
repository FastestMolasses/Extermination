// CFLAGS: -O4,p -sdatathreshold 0
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern unsigned char D_008102B0[];

int func_001B7F90(unsigned char *arg0, int unused, unsigned char *arg2) {
    unsigned char *base = D_008102B0;
    float *p = (float *)arg0;
    float f0;
    float f1;
    if (*(int *)(arg2 + 0x8) != 1) {
        return 0;
    }
    f0 = func_001B1240(base + 0xA0, p[0xB0 / 4], p[0xB8 / 4]);
    f1 = func_001B12B0(f0, *(float *)(base + 0xC4), *(float *)(arg2 + 0x24));
    *(float *)(base + 0xC4) = f1;
    if (f1 == f0) {
        return 1;
    }
    return 0;
}
