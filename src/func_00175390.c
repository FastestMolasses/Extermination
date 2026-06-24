// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern float func_0011DE90(float);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern float D_008106A0;
extern unsigned char D_00810E57;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

unsigned char func_00175390(char *arg0) {
    float aa, s248, s24c;

    *(unsigned char *)(arg0 + 0x23F) = D_00810E57;
    if (*(unsigned char *)(arg0 + 0x23F) == 0) {
        *(float *)(arg0 + 0x24C) = 0.0f;
        *(float *)(arg0 + 0x218) = *(float *)(arg0 + 0xC4);
        return 0;
    }
    aa = (float)(unsigned int)D_00810E65 / 256.0f * 3.1415927f;
    *(float *)(arg0 + 0x244) = func_0011DE90((float)(unsigned int)D_00810E64 / 256.0f * 3.1415927f);
    s248 = func_0011DE90(aa);
    *(float *)(arg0 + 0x248) = s248;
    s24c = func_0011E620(-s248, *(float *)(arg0 + 0x244));
    *(float *)(arg0 + 0x24C) = s24c;
    *(float *)(arg0 + 0x218) = func_001B1470(3.1415927f + s24c + D_008106A0);
    return *(unsigned char *)(arg0 + 0x23F);
}
