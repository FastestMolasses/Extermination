// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 68.70% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. Source unchanged apart from the compiler line.

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
