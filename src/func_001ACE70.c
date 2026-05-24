// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_008106CE;
extern unsigned char D_008106C5;
extern unsigned char D_008106B0;

int func_001ACE70(void) {
    int v0;
    v0 = D_008106CE;
    if (v0 == 0) {
        v0 = D_008106C5;
        if (v0 != 0) {
            return 2;
        }
        v0 = D_008106B0;
        v0 = (v0 == 0) ? 0 : 2;
        return v0;
    }
    return 3;
}
