// CFLAGS: -O4,p -sdatathreshold 0
extern char D_00279110;

int func_0010EFA8(int a0) {
    if ((unsigned int)a0 >= 0x10U) {
        return 0;
    }
    return (int)(&D_00279110 + a0 * 0x10);
}
