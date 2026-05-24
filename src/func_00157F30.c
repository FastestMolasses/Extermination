// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008106B0;
extern int D_008106D0;

int func_00157F30(char *a0) {
    D_008106B0 = 4;
    D_008106D0 = *(int *)(a0 + 0x14);
    a0[0xA] = 0;
    a0[0xB] = 0;
    a0[0] = 1;
    return 1;
}
