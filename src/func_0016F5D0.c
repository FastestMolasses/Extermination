// CFLAGS: -O2 -sdatathreshold 0
// Reset object substates (0x1F1/0x318 -> 2, 0x2F2 -> 0) and consume the
// global one-shot flag D_008106C7 if it was set.
extern unsigned char D_008106C7;

void func_0016F5D0(unsigned char *obj) {
    obj[0x1F1] = 2;
    obj[0x318] = 2;
    obj[0x2F2] = 0;
    if (D_008106C7) {
        D_008106C7 = 0;
    }
}
