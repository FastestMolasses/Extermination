// CFLAGS: -O4,p -sdatathreshold 0
extern char D_002817C0;

int func_0011A938(int a0) {
    int v1 = 0;
    if (a0 < 0x10) {
        char *base = &D_002817C0;
        v1 = *(int *)(base + a0 * 4 + 0x180);
    }
    return v1;
}
