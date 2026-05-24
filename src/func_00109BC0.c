// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_002412F4;

void func_00109BC0(int a0) {
    int *v1 = D_002412F4;
    int *a1 = (int *)*(int *)((char *)v1 + 0x40);
    *(int *)((char *)a1 + 0xD8) = a0;
}
