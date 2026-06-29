// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Angle-tracking step: clamp the signed delta between a target angle
// (field 0xC4) and the current angle (field 0x60) to +/-0.034906585 rad
// (~2 deg), choose a step coefficient by sign quadrant, advance via
// func_001B12B0, and write the new target angle back to field 0x60.
extern float func_001B1470(float);
extern float func_0011DF78(float);
extern float func_001B12B0(float, float, float);

void func_0013BA20(unsigned char *arg0, unsigned char *arg1) {
    int flag;
    float v20;
    float f14;

    flag = *(short *)(arg0 + 0x2C) & 0xFFFF7FFF;
    if (flag == 0 || flag == 4) {
        *(float *)(arg1 + 0x60) = *(float *)(arg0 + 0xC4);
    }
    if (!(func_0011DF78(v20 = func_001B1470(*(float *)(arg0 + 0xC4) - *(float *)(arg1 + 0x60))) <= 0.034906585f)) {
        if (v20 < 0.0f) {
            v20 = -0.034906585f;
        } else {
            v20 = 0.034906585f;
        }
    }
    if (v20 < 0.0f) {
        if (!(*(float *)(arg0 + 0xC8) < 0.0f)) {
            f14 = 0.03316125646233559f;
        } else {
            f14 = 0.06108652800321579f;
        }
    } else {
        if (*(float *)(arg0 + 0xC8) <= 0.0f) {
            f14 = 0.03316125646233559f;
        } else {
            f14 = 0.06108652800321579f;
        }
    }
    *(float *)(arg0 + 0xC8) = func_001B12B0(-(1.5533430576324463f * (v20 / 0.034906585f)), *(float *)(arg0 + 0xC8), f14);
    *(float *)(arg1 + 0x60) = *(float *)(arg0 + 0xC4);
}
