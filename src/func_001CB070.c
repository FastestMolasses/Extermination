// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_001C7420(int, int, int);
extern void func_001CAAC0(int, int);
extern void func_001D40E0(int, int, int);
extern void func_001D8C20(int);
extern char *D_00275670;
extern int D_00275B44;

void func_001CB070(int arg0, int arg1) {
    char *base;
    char *p;

    base = D_00275670;
    p = *(char **)(base + 0x1C);
    func_001D8C20(4);
    *(int *)(arg0 + 0x88) = 0x3F800000;
    *(int *)(arg0 + 0x84) = 0x3F800000;
    *(int *)(arg0 + 0x80) = 0x3F800000;
    func_001C7420(arg0, 0x3F5, 3);
    func_001D40E0(3, arg1, arg0 + 0x80);
    base = D_00275670;
    *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C)) = 0;
    *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
    func_001CAAC0(D_00275B44 + 0xB0, (int)p);
    func_001D8C20(0);
}
