// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00122BB8(int);

void func_001C1500(int a0, int a1, float f12, float f13, float f14) {
    int s0 = a0 + 0x1F0;
    int r;
    *(int *)(a0 + 0x22C) = a1;
    *(int *)(a0 + 0x210) = 0;
    *(int *)(a0 + 0x214) = 0x3F800000;
    r = func_00122BB8(a0);
    *(int *)(s0 + 0x34) = r;
    *(int *)(s0 + 0x38) = 0;
    *(float *)(s0 + 0x28) = f12;
    *(float *)(s0 + 0x2C) = f13;
    *(float *)(s0 + 0x30) = f14;
}
