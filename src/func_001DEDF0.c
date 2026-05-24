// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001DEDB0(int);
extern void func_001DEE80(int, int);
extern void func_001DEEC0(int, int);
extern int D_0026E850;

void func_001DEDF0(void) {
    char *p;
    p = func_001DEDB0(2);
    p[0] = 0;
    p[3] = 0;
    p[2] = 0;
    p[1] = 0;
    *(int *)(p + 8) = 2;
    p = func_001DEDB0(9);
    p[0] = 0;
    p[3] = 0;
    p[2] = 0;
    p[1] = 0;
    *(int *)(p + 8) = 9;
    func_001DEE80(2, (int)&D_0026E850);
    func_001DEEC0(2, 0x60);
    func_001DEE80(9, (int)&D_0026E850);
    func_001DEEC0(9, 0x60);
}
