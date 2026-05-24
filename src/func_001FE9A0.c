extern int D_00275C64;
extern int func_00114848(int, int, int);

void func_001FE9A0(int a0, int a1, int a2) {
    int *s0 = (int *)a2;
    int v0 = func_00114848(a0, a1, a2);
    if (v0 == 1) {
        D_00275C64 = *s0;
    }
}
