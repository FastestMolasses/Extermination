// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_0081070A;

int func_00128390(int a0, int a1) {
    if (D_0081070A == 0) return a1 == 0 ? 15 : 30;
    else return a1 == 0 ? 30 : 50;
}
