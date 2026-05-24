// CFLAGS: -O4,p -sdatathreshold 0
extern char D_0027E0C0;

int func_00119D08(int a0) {
    int v1 = 0x78;
    char *v0 = &D_0027E0C0;
    int a1 = -1;
    v1 = a0 * v1;
    v1 += (int)v0;
    if ((unsigned int)a0 >= 0x30U) {
        return a1;
    }
    a1 = *(unsigned short *)(v1 + 0x54);
    return a1;
}
