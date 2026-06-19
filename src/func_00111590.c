// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00112D18(int a0);
extern int D_00241D44;

int func_00111590(int a0) {
    if (func_00112D18(1) != 0) {
        return 0;
    } else {
        int v0 = D_00241D44;
        D_00241D44 = a0;
        return v0;
    }
}
