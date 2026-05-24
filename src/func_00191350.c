// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001B1EA0(int, int, int *, int);
extern int D_0024A4F0;

int func_00191350(int a0) {
    int r;
    int v1;
    r = func_001B1EA0(0, a0 + 0xA0, &D_0024A4F0, 4);
    v1 = 1;
    v1 = r ? v1 : 0;
    return v1;
}
