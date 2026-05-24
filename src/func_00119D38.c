// CFLAGS: -O4,p -sdatathreshold 0
extern char D_0027E0C0[0x10000];

int func_00119D38(int a0) {
    char *v0;
    char *a1;
    unsigned short v1;
    int a2;

    v0 = D_0027E0C0;
    a2 = -1;
    if (a0 >= 48) {
        return a2;
    }
    a1 = v0 + a0 * 120;
    v1 = *(unsigned short *)(a1 + 46);
    if (v1 != 1) {
        return a2;
    }
    v1 = *(unsigned short *)(a1 + 64);
    a2 = *(unsigned short *)(a1 + 48);
    a2 = a2 | ((int)v1 << 1);
    return a2;
}
