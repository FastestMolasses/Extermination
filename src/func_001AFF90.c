// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AF800(int);

void func_001AFF90(int a0) {
    int s0 = *(int *)(a0 + 0x14);
    int i;
    int *p;

    *(int *)(a0 + 0x14) = 0;
    func_001AF800(s0);
    *(int *)(s0 + 0x0) = 0;
    *(int *)(s0 + 0x4) = 0;
    *(int *)(s0 + 0x8) = 0;
    *(int *)(s0 + 0xC) = 0;
    *(short *)(s0 + 0x36) = 0;
    *(char *)(s0 + 0x98) = 0;
    *(int *)(s0 + 0x90) = 0;
    i = 0;
    p = (int *)(s0 + 0x1F0);
    do {
        i++;
        *p = 0;
        p++;
    } while (i < 0x40);
}
