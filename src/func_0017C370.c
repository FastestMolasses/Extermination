// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_0017B490(char *, int, int, int);
extern void func_001749A0(char *, int, int, float);
extern float func_001B1470(float);

void func_0017C370(char *e) {
    int clip;

    if (*(unsigned char *)(e + 0x1F0) == 6 || *(unsigned char *)(e + 0x1F0) == 7) {
        if (*(unsigned char *)(e + 0x1F1) == 3) {
            clip = func_0017B490(e, 2, *(unsigned char *)(e + 0x235), 0);
        } else {
            clip = func_0017B490(e, 4, *(unsigned char *)(e + 0x235), 0);
        }
        func_001749A0(e, clip, 1, 0.0f);
        *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
        *(float *)(e + 0x1FC) = 8.0f;
    }
    *(int *)(e + 0x38) = 0;
    *(int *)(e + 0x240) = 0;
    *(char *)(e + 0x25C) = 0;
    *(char *)(e + 0x25E) = 0;
    *(int *)(e + 0x260) = 0;
    *(int *)(e + 0x264) = 0;
    *(int *)(e + 0x268) = 0;
}
