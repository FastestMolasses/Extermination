// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00119528(int a0, int a1);
extern void func_001193A8(int a0, int a1, int a2);

int func_001194B8(int a0, int *a1, int a2) {
    int r = func_00119528((int)a1, a2);
    if (r != -1) {
        func_001193A8(a0, a2, a1[1]);
    }
    return r;
}
