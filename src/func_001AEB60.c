// CFLAGS: -O2 -sdatathreshold 0
extern volatile short D_0028A8D0[];
extern volatile short D_0028A8D2[];
extern volatile int D_0028A8D4[];

void func_001AEB60(short a0) {
    short a1 = D_0028A8D0[0];
    if (a1 == 1) return;
    D_0028A8D0[0] = 3;
    D_0028A8D2[0] = a0;
    D_0028A8D4[0] = 0;
}