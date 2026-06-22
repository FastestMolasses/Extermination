// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern float func_001B1240(int, float, float);
extern float func_001B1470(float);
extern float func_0011DF78(float);

int func_001B1560(int a0, int a1, float f12) {
    float f0;
    f0 = func_001B1240(a0 + 0xB0, *(float *)(a1 + 0x0), *(float *)(a1 + 0x8));
    f0 = func_001B1470(f0 - *(float *)(a0 + 0xC4));
    f0 = func_0011DF78(f0);
    if (f0 <= f12) {
        return 1;
    }
    return 0;
}
