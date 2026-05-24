// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00121A28(int, int, int);
extern int D_0028B020;

void func_001AFE60(void) {
    int i;
    int *p;
    p = &D_0028B020;
    i = 0;
    do {
        func_00121A28((int)p, 0, 0x2F0);
        i++;
        p = (int *)((int)p + 0x2F0);
    } while (i < 0x18);
}
