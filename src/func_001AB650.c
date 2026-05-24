// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00121A28(int, int, int);
extern int D_0028A750;

void func_001AB650(void) {
    int i;
    int *p;
    p = &D_0028A750;
    i = 0;
    do {
        func_00121A28((int)p, 0, 0x20);
        i++;
        p = (int *)((int)p + 0x20);
    } while (i < 3);
}
