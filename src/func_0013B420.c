// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame state reset + 12-bit signed rapid-fire counter step; advances counter by n (x5 when sign bit set) and compares against the [0x34] threshold.
float func_0011E620(float, float);
float func_001B1470(float);
void func_001B4CF0(char *);
void func_001EFE00(int, char *);
void func_0021C040(void *, char *);
extern char D_008102B0;
extern unsigned char D_008106BD;

void func_0013B420(char *arg0, char *arg1) {
    float temp_f0;
    short temp_a0;
    short n;
    char *temp_a0_2;

    *((char *) (arg0 + 0)) = 2;
    *((char *) (arg1 + 0x82)) = 0;
    *((char *) (arg1 + 0x84)) = 0;
    *((int *) (arg1 + 0x4C)) = 0x3ECCCCCD;
    *((int *) (arg1 + 0x48)) = 0;
    *((int *) (arg1 + 0x44)) = 0;
    *((int *) (arg1 + 0x5C)) = 0;
    *((int *) (arg1 + 0x50)) = 0;
    *((int *) (arg0 + 0xC8)) = 0;
    temp_f0 = func_001B1470(3.1415927f + func_0011E620(*((float *) (arg0 + 0x70)), *((float *) (arg0 + 0x78))));
    *((float *) (arg0 + 0xC4)) = temp_f0;
    *((float *) (arg1 + 0x58)) = temp_f0;
    temp_a0 = *((short *) (arg0 + 0x36));
    n = (short) (temp_a0 & 0xFFF);
    if (temp_a0 & 0x8000) {
        n = (short) (n * 5);
    }
    temp_a0_2 = *((char **) (arg0 + 0x20));
    if (temp_a0_2 != 0) {
        *((char *) (temp_a0_2 + 4)) = 3;
        *((char **) (arg0 + 0x20)) = 0;
    }
    if ((*((char *) (arg1 + 0x83))) != 0) {
        *((char *) (arg1 + 0x83)) = 0;
        func_0021C040(&D_008102B0, arg0);
        if (D_008106BD == 1) {
            D_008106BD = 0;
        }
    }
    if (((*((char *) (arg1 + 0x85))) == 0) && ((*((short *) (arg0 + 0x36))) & 0x4000)) {
        *((char *) (arg1 + 0x85)) = 0x3C;
        func_001EFE00(0x80000027, arg0);
    }
    if ((*((short *) (arg0 + 0x34))) <= n) {
        *((unsigned char *) (arg0 + 5)) = 2;
        *((short *) (arg0 + 0x34)) = 0;
        func_001B4CF0(arg0);
        return;
    }
    *((unsigned char *) (arg0 + 5)) = (unsigned char) ((*((unsigned char *) (arg0 + 5))) + 1);
    *((short *) (arg0 + 0x34)) = (short) ((*((short *) (arg0 + 0x34))) - n);
}
