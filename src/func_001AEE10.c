extern volatile short D_0028A9A0[];
extern volatile short D_0028A9A6[];
extern volatile unsigned char D_0028A9A3[];
extern volatile unsigned char D_0028A9A2[];

void func_001AEE10(short a0, unsigned char a1) {
    D_0028A9A0[0] = 1;
    D_0028A9A6[0] = a0;
    D_0028A9A3[0] = 2;
    D_0028A9A2[0] = a1;
}
