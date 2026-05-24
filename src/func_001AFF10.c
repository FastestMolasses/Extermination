// CFLAGS: -O4,p -sdatathreshold 0
extern char D_0028B020[24][0x2f0];

char *func_001AFF10(void) {
    int a0;
    char *v0;
    unsigned char v1;
    v0 = D_0028B020[0];
    for (a0 = 0; a0 < 24; a0++, v0 += 0x2f0) {
        v1 = *(unsigned char *)v0;
        if (v1 != 0) continue;
        *(int *)(v0 + 0x14) = (int)v0;
        v0[0] = 2;
        *(int *)(v0 + 0x60) = 0x3f800000;
        *(int *)(v0 + 0x64) = 0x3f800000;
        *(int *)(v0 + 0x68) = 0x3f800000;
        *(int *)(v0 + 0x6c) = 0x3f800000;
        *(int *)(v0 + 0x80) = 0x3f800000;
        *(int *)(v0 + 0x84) = 0x3f800000;
        *(int *)(v0 + 0x88) = 0x3f800000;
        *(int *)(v0 + 0x8c) = 0x3f800000;
        v0[0x9a] = 0;
        v0[0x99] = 0;
        *(short *)(v0 + 0x94) = -1;
        return v0;
    }
    return (char *)0;
}
