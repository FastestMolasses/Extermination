// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001AFA90(int);
extern void func_001F5040(void);

int func_001F4F40(int a0) {
    char *v0;
    v0 = (char *)func_001AFA90(0xC);
    if (v0) {
        v0[0xD] = a0;
        *(int *)(v0 + 0x10) = (int)func_001F5040;
        return (int)v0;
    }
    return 0;
}
