// CFLAGS: -O4,p -sdatathreshold 0
extern char D_002817C0;

int func_0011A730(int a0) {
    int v1 = 0;
    if ((unsigned int)a0 < 0x30U) {
        char *base = &D_002817C0;
        v1 = *(int *)(base + a0 * 4 + 0xC0);
    }
    return v1;
}
