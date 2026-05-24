extern int *D_00275BF8;
extern int *D_00275BF4;
extern int *D_00275BF0;
extern int *D_00275BEC;
extern int func_001C6120(int, short);

void func_001C8480(int a0, int a1) {
    int *v0;
    int *v1;
    D_00275BF8 = (int *)func_001C6120(a0, (short)a1);
    v0 = D_00275BF8;
    v1 = (int *)((char *)v0 + *(int *)((char *)v0 + 8));
    D_00275BF4 = v1;
    v1 = (int *)((char *)v0 + *(int *)((char *)v0 + 0xC));
    D_00275BF0 = v1;
    v1 = (int *)((char *)v0 + *(int *)((char *)v0 + 0x10));
    D_00275BEC = v1;
}
