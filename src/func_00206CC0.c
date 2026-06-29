// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00206BE0(int);
extern int func_00109A40(int);

int func_00206CC0(int a0) {
    int r = 0;
    if (func_00206BE0(a0) == 0) {
        if (func_00109A40(a0) != 0) {
            r = 1;
        }
    }
    return r;
}
