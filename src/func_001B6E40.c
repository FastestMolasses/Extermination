// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_008102B0[8];
extern int func_00182BF0(unsigned char *);

int func_001B6E40(void) {
    int v0 = *(unsigned char *)0x70003B8D;
    if (v0 != 0) {
        return 0;
    }
    v0 = func_00182BF0(D_008102B0);
    if (v0 != 0) {
        return 0;
    }
    *(unsigned char *)0x70003B8D = 3;
    return 1;
}
