// CFLAGS: -O4,p -sdatathreshold 0
extern int func_0019AB20(int, int, int, int);
extern void func_0019A310(int);
extern int D_700038A0;

int func_00179010(int a0) {
    int r;
    *(volatile int *)(0x700038A0) = 0;
    *(volatile int *)(0x700038A4) = 0xC0C00000;
    *(volatile int *)(0x700038A8) = 0;
    *(volatile int *)(0x700038AC) = 0x3F800000;
    r = func_0019AB20(a0, a0 + 0xB0, (int)&D_700038A0, 0x80000006);
    if (r) {
        unsigned char *s0 = (unsigned char *)a0;
        s0[0xA] = 1;
        s0[0x23B] = *(unsigned char *)(*(int *)(0x700031D0) + 0x1A);
        func_0019A310(a0 + 0x9C);
    } else {
        char *s0 = (char *)a0;
        s0[0x23B] = 0;
    }
    return *(unsigned char *)((char *)a0 + 0xA);
}
