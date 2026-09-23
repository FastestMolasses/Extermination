// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 67.14% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. Source unchanged apart from the compiler line.

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
