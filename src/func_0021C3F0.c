// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810770;

int func_0021C3F0(void) {
    int v0;
    v0 = 1;
    if (D_00810700 != 8) {
        return v0;
    }
    if (D_00810701 != 2) {
        return v0;
    }
    if (D_00810770 != 0xff) {
        return v0;
    }
    v0 = 0;
    return v0;
}
