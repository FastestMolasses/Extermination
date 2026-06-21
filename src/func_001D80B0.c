// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int *func_001D8060(void);

void func_001D80B0(void) {
    int *p = func_001D8060();
    if (p != 0) {
        *(int *)((char *)p + 0x2C) = 0;
        *(int *)((char *)p + 0xC) = -1;
    }
}
