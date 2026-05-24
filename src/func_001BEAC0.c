// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001AFA90(int);
extern void func_00102948(int, int);
extern void func_001BE6C0(void);

int func_001BEAC0(int a0, int a1, int a2, int a3) {
    char *s0;
    s0 = (char *)func_001AFA90(2);
    if (s0) {
        func_00102948((int)(s0 + 0xB0), a0 + 0xB0);
        func_00102948((int)(s0 + 0xC0), a0 + 0xC0);
        func_00102948((int)(s0 + 0xA0), a1);
        s0[3] = 0x13;
        s0[0xD] = a3;
        *(short *)(s0 + 0x34) = a2;
        *(int *)(s0 + 0x10) = (int)func_001BE6C0;
        *(int *)(s0 + 0x20) = a0;
    }
    return (int)s0;
}
