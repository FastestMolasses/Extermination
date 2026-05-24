// CFLAGS: -O4,p -sdatathreshold 0
extern int D_008106C8;
extern unsigned char D_008106BE;

void func_001B0B50(void) {
    int a0 = D_008106C8;
    if (a0 & 1) {
        D_008106BE = 1;
    } else if (a0 & 2) {
        D_008106BE = 0x81;
    } else {
        D_008106BE = 0;
    }
}
