extern int *D_00275BF8;
extern int func_001C6120(int, short);

int func_001C6160(int *a0) {
    short a1 = *(short *)((char *)a0 + 0x2C);
    D_00275BF8 = (int *)func_001C6120(*(int *)((char *)a0 + 0x40), a1);
    return *(unsigned short *)((char *)D_00275BF8 + 2);
}
