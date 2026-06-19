// CFLAGS: -O4,p -sdatathreshold 0
char *func_001C6120(char *a0, int a1) {
    int v;
    char *p;
    a1 = a1 & 0xFFFF;
    a1 = a1 & 0xFFFF7FFF;
    p = (char *)(a1 << 2) + (int)a0;
    v = *(int *)(p + 4) >> 2;
    a0 = a0 + (v << 2);
    return a0;
}
