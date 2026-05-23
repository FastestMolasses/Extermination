extern volatile short D_0028A9A0[];
extern volatile unsigned char D_0028A9A3[];
extern volatile unsigned char D_0028A9A2[];
extern volatile short D_0028A9A4[];

void func_001AEDB0(unsigned char a0) {
    D_0028A9A0[0] = 2;
    D_0028A9A3[0] = 1;
    D_0028A9A2[0] = a0;
    D_0028A9A4[0] = 0xFF;
}
