// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Camera step: func_001916C0(cam, target, 1), then the angle cam+0x44 =
// func_001B12B0(cam+0x48, cam+0x44, 0.0034906587) and the position is cam+0x10 = cam+0x20 - cam+0x4C * sin(angle), cam+0x18 = cam+0x28 -
// cam+0x4C * cos(angle) (func_0011E2A8 / func_0011DE90). cam+1 = 1 and cam+3 = 0 when
// the angle equals cam+0x48, when target+0x230 is neither 1 nor 2, or when cam+7
// has any of bits 0x0D (cam+3 == 0) or 0x0B (cam+3 != 0).
extern void func_001916C0(unsigned char *cam, unsigned char *target, int mode);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);

void func_00193D90(unsigned char *cam, unsigned char *target) {
    int m;
    func_001916C0(cam, target, 1);
    *(float *)(cam + 0x44) = func_001B12B0(*(float *)(cam + 0x48), *(float *)(cam + 0x44), 0.0034906587f);
    *(float *)(cam + 0x10) = *(float *)(cam + 0x20) - *(float *)(cam + 0x4C) * func_0011E2A8(*(float *)(cam + 0x44));
    *(float *)(cam + 0x18) = *(float *)(cam + 0x28) - *(float *)(cam + 0x4C) * func_0011DE90(*(float *)(cam + 0x44));
    if (*(float *)(cam + 0x44) == *(float *)(cam + 0x48)) {
        cam[1] = 1;
        cam[3] = 0;
    }
    m = *(int *)(target + 0x230);
    if (m != 1 && m != 2) {
        cam[1] = 1;
        cam[3] = 0;
    }
    if (cam[3] == 0) {
        if (cam[7] & 0xD) {
            cam[1] = 1;
            cam[3] = 0;
        }
    } else if (cam[7] & 0xB) {
        cam[1] = 1;
        cam[3] = 0;
    }
}
