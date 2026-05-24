// CFLAGS: -O4,p -sdatathreshold 0
extern char D_002410E8[20];

char *func_00101EA8(char *a0) {
    char *t0 = D_002410E8;
    long a1 = *(long *)t0;
    long a2 = *(long *)(t0 + 8);
    int a3 = *(int *)(t0 + 16);
    *(long *)a0 = a1;
    *(long *)(a0 + 8) = a2;
    *(int *)(a0 + 16) = a3;
    return a0;
}
