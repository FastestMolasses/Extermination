// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001AFA90(int);
extern void func_00102948(int, int);
extern void func_0014C220(void);

void func_0014D1E0(int a0, int a1, int a2) {
    char *s0;
    if (!(s0 = (char *)func_001AFA90(2))) return;
    s0[3] = 8;
    *(int *)(s0 + 0x10) = (int)func_0014C220;
    s0[0xD] = a2;
    func_00102948((int)(s0 + 0xB0), a0);
    func_00102948((int)(s0 + 0xC0), a1);
}
