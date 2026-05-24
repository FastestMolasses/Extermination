// CFLAGS: -O4,p -sdatathreshold 0
extern void func_0018A6B0(void);
extern char *func_001AFA90(int);

char *func_0018A880(int a0, int a1) {
    char *v0;
    int s0;
    int s1;
    int v1;

    s1 = a0;
    s0 = a1;
    v0 = func_001AFA90(1);
    if (v0 != (char *)0) {
        v1 = (int)func_0018A6B0;
        v0[3] = s1;
        *(int *)(v0 + 0x10) = v1;
        v0[0xd] = s0;
    }
    return v0;
}
