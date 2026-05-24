// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008102B0[];

void func_001831F0(int a0) {
    char *a2 = D_008102B0;
    if (a0 == 0) {
        a2[0x23F] = 0;
        *(int *)(a2 + 0x24C) = -1;
    } else if (a0 == 1) {
        a2[0x23F] = 2;
        *(int *)(a2 + 0x24C) = 0;
    } else {
        a2[0x23F] = 2;
        *(int *)(a2 + 0x24C) = 1;
    }
}
