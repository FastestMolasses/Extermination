// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C5930(void);
extern char *func_001AFA90(int);

void func_001C5C50(void) {
    char *v0 = func_001AFA90(8);
    if (v0 == 0) return;
    v0[3] = 3;
    v0[0xD] = 0;
    *(void **)(v0 + 0x10) = (void *)func_001C5930;
}
