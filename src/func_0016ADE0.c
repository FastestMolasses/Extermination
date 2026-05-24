// CFLAGS: -O4,p -sdatathreshold 4
extern unsigned short D_00810E70[4];
extern int D_00275B14;

int func_0016ADE0(char *a0) {
    int v1;
    int v0;

    v1 = D_00810E70[0];
    v0 = *(unsigned short *)0x70003B7c;
    v0 = v1 & v0;
    if (v0 == 0) {
        v0 = *(unsigned short *)0x70003B7e;
        v0 = v1 & v0;
    }
    if (v0 != 0) {
        a0[5] = 0x13;
        a0[6] = 0;
        a0[0x1f0] = 0x25;
        D_00275B14 = 0x1e;
        v0 = 1;
    }
    return v0;
}
