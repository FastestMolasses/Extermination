// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241C70;
extern unsigned char D_002770C0[];
extern void func_0010C9A0(unsigned char *);

void func_0010CF20(int c) {
    int idx;
    idx = D_00241C70;
    if (idx >= 0x7E) {
        D_00241C70 = 0;
        D_002770C0[0x7F] = 0;
        func_0010C9A0(D_002770C0);
        idx = D_00241C70;
    }
    if (c == 0xA) {
        D_00241C70 = 0;
        D_002770C0[idx] = c;
        D_002770C0[idx + 1] = 0;
        func_0010C9A0(D_002770C0);
        return;
    }
    D_00241C70 = idx + 1;
    D_002770C0[idx] = c;
}
