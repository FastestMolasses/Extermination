// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Initializes a sub-structure at arg0+0x2D0. Sets float field +4 to
// 40.0f + (float)(func_00122BB8() % 120) (a randomized timer/phase in [40,160)),
// zeroes the int at +0, stores arg1 as a short at +8, zeroes short +0xA and
// int +0xC. func_00122BB8 receives the original a0/a1 unchanged (declared void).
extern int func_00122BB8(void);

void func_001F1110(char *arg0, int arg1) {
    char *p;

    p = arg0 + 0x2D0;
    *(float *)(p + 4) = 40.0f + (float)(func_00122BB8() % 120);
    *(int *)(p + 0) = 0;
    *(short *)(p + 8) = (short)arg1;
    *(short *)(p + 0xA) = 0;
    *(int *)(p + 0xC) = 0;
}
