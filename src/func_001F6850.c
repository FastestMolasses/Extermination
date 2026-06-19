// CFLAGS: -O4,p -sdatathreshold 0
extern void *func_001F6760(void);
extern void func_001F66F0(void *p);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char D_0025D2C0[];

void func_001F6850(void) {
    void *p = func_001F6760();
    if (!p) {
        return;
    }
    func_001F66F0(p);
    if ((D_00810700 << 8) + D_00810701 == 0x1301) {
        func_001F66F0(D_0025D2C0);
    }
}
