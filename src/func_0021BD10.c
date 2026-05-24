// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_008102B0[8];
extern int func_0021BB00(unsigned char *);

int func_0021BD10(void) {
    int v1 = D_008102B0[0];
    int v0 = 1;
    if (v1 != v0) {
        return 0;
    }
    v0 = func_0021BB00(D_008102B0);
    if (v0 != 0) {
        return 0;
    }
    return 1;
}
