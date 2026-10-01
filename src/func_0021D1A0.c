// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern float D_70003A20;
extern float D_70003A24;
extern float func_0011E620(float, float);   /* atan2f */
extern float func_001B1470(float);
extern float func_0011DF78(float);          /* fabsf */

int func_0021D1A0(char *e) {
    D_70003A20 = func_0011E620(-*(float *)(e + 0x78), *(float *)(e + 0x70));
    D_70003A20 = func_001B1470(1.5707964f + D_70003A20);
    D_70003A24 = func_001B1470(D_70003A20 - *(float *)(e + 0xC4));
    if (func_0011DF78(D_70003A24) <= 1.5707964f) return 0;
    return 1;
}
