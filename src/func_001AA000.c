// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern short D_70003B86;
extern float func_0011E748(float);          /* sqrtf */
extern float func_0011E620(float, float);   /* atan2f */
extern float func_001B1470(float);
extern float func_0011DE90(float);          /* cosf */
extern float func_0011E2A8(float);          /* sinf */

void func_001AA000(char *a, char *b, char *ga, char *gb) {
    float dx, dz, dy, d, r, ang, oz;

    if (*(int *)(ga + 0xE4) != *(int *)(gb + 0xE4)) return;
    if (*(int *)(ga + 0xE4) == 0x200) {
        D_70003B86 = 0;
        return;
    }
    dx = *(float *)(a + 0xB0) - *(float *)(b + 0xB0);
    dz = *(float *)(a + 0xB8) - *(float *)(b + 0xB8);
    d = func_0011E748(dx * dx + dz * dz);
    r = (*(float **)(a + 0x30))[0] + (*(float **)(b + 0x30))[0];
    if (!(d <= r)) return;
    dy = *(float *)(a + 0xB4) - *(float *)(b + 0xB4);
    d = func_0011E748(dy * dy);
    if (!(d <= (*(float **)(a + 0x30))[1] + (*(float **)(b + 0x30))[1])) return;
    ang = func_001B1470(func_0011E620(dx, dz));
    oz = r * func_0011DE90(ang);
    *(float *)(a + 0xB0) = *(float *)(b + 0xB0) + r * func_0011E2A8(ang);
    *(float *)(a + 0xB8) = *(float *)(b + 0xB8) + oz;
    D_70003B86 = 0;
}
