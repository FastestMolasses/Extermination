// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241C68;
extern int D_00276C98;
struct C788 { int flag; int value; };
extern struct C788 D_00276C90;
extern void _iSignalSema(int sema);

int func_0010C788(unsigned int a0) {
    if (a0 >= 0x80) {
        return -1;
    }
    if (D_00241C68 == 0) {
        return -1;
    }
    D_00276C90.value = a0;
    D_00276C90.flag = 1;
    _iSignalSema(D_00276C98);
    return a0;
}
