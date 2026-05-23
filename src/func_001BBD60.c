extern char D_0024DB80[];

void func_001BBD60(unsigned char *a0, int *a1) {
    short a2 = *(short *)(a0 + 0x56);
    unsigned short v1 = *(unsigned short *)(a0 + 0x2E);
    int row = ((a2 & 0xFF00) >> 8) * 4;
    unsigned short res = *(unsigned short *)(D_0024DB80 + row + v1 * 2);
    a1[6] = res;
}
