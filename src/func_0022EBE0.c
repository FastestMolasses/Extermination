// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_008101E4;

int func_0022EBE0(void) {
    int v0;
    unsigned char v1 = D_008101E4;
    if (v1 == 3) {
        return 1;
    }
    v1 = *(volatile unsigned char *)(0x70003B8D);
    v0 = 0;
    if (v1) {
        if (v1 != 4) {
            return 1;
        }
        v0 = 0;
    }
    return v0;
}
