// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_00810CC3[];
extern volatile char D_008106B0;
extern volatile char D_008106B1;

int func_001C4760(int a0, int a1) {
    int v0;
    unsigned char *p;
    p = &D_00810CC3[a0];
    *p += a1;
    v0 = (a0 < 0x20);
    if (!v0) {
        D_008106B0 = 3;
        D_008106B1 = a0;
    }
    return v0;
}
