// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001B1190(int);
extern void func_001AFC10(char *);

void func_00131E80(char *a0) {
    char *s0 = a0;
    char *v1 = *(char **)(a0 + 0x20);
    if (v1 != 0) {
        v1[4] = 3;
        *(int *)(s0 + 0x20) = 0;
    }
    func_001B1190(*(unsigned char *)(s0 + 0x9A));
    func_001AFC10(s0);
}
