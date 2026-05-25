// CFLAGS: -O2 -sdatathreshold 0
extern volatile short D_0028A8D0[];
extern volatile short D_0028A8D2[];
extern volatile int D_0028A8D4[];

void func_001AEBA0(short a0) {
    short v1 = D_0028A8D0[0];
    if (v1 == 0) return;
    D_0028A8D0[0] = 2;
    D_0028A8D2[0] = a0;
    D_0028A8D4[0] = 0xFF;
}