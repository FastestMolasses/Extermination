extern int *D_00275BF8;
extern int func_001C6120(int, short);

int func_001C61D0(int a0, int a1) {
    short a1_s = (short)a1;
    D_00275BF8 = (int *)func_001C6120(a0, a1_s);
    return *(unsigned short *)((char *)D_00275BF8 + 2);
}
