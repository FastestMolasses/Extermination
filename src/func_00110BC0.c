// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_00241CB8;
extern void func_00123168(int, int);

void func_00110BC0(int a0, int a1) {
    int v1;
    int v0;

    if (a0 >= 8) {
        *((char *)a1) = *(char *)D_00241CB8;
        return;
    }
    v0 = (int)D_00241CB8 + a0 * 4;
    func_00123168(a1, *(int *)v0);
}
