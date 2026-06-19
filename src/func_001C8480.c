// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275BF8;
extern int *D_00275BF4;
extern int *D_00275BF0;
extern int *D_00275BEC;
extern int *func_001C6120(int *base, short index);

void func_001C8480(int *base, int index) {
    D_00275BF8 = func_001C6120(base, (short)index);
    D_00275BF4 = (int *)((char *)D_00275BF8 + D_00275BF8[2]);
    D_00275BF0 = (int *)((char *)D_00275BF8 + D_00275BF8[3]);
    D_00275BEC = (int *)((char *)D_00275BF8 + D_00275BF8[4]);
}
