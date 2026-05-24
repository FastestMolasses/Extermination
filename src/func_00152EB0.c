// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275B40;

void func_00152EB0(int a0) {
    int *v1;
    int a0b;
    a0b = *(unsigned char *)((char *)a0 + 5);
    if (a0b == 9 || a0b == 99) return;
    v1 = (int *)*D_00275B40;
    v1[0] = 0;
    v1 = (int *)*D_00275B40;
    v1[1] = 0;
    v1 = (int *)*D_00275B40;
    v1[2] = 0;
    v1 = (int *)*D_00275B40;
    v1[3] = 0;
    v1 = (int *)*D_00275B40;
    v1[4] = 0;
    v1 = (int *)*D_00275B40;
    v1[5] = 0;
}
