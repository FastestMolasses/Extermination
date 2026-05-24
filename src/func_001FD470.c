// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FBC50(int);
extern void func_001FABB0(void);

void func_001FD470(int a0) {
    int s0 = a0;
    if (a0 & 0x1) {
        func_001FBC50(a0);
    }
    if (s0 & 0x2) {
        func_001FABB0();
    }
}
