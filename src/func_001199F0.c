// CFLAGS: -O4,p -sdatathreshold 0
extern char D_002819C0[0x100];

int func_001199F0(int a0, int a1) {
    int a2 = a1;
    int a3;
    char *v0;
    char *v1;
    int idx;
    if ((unsigned int)a0 >= 0x80U) { return -1; }
    if (a2 < 0) { return -1; }
    if (a2 >= 0x80) { return -1; }
    idx = a0 << 1;
    v0 = D_002819C0;
    v1 = v0 + idx;
    a3 = *v1;
    *v1 = (char)a2;
    *(v0 + idx + 1) = 1;
    return a3;
}
