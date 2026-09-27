// COMPILER: eegcc
// CFLAGS: -O2
extern int sceSifGetReg(int a0);
extern void sceSifSetReg(int a0, int a1);
extern void func_0010CE28(void);

int func_001104C0(void) {
    if (sceSifGetReg(4) & 0x40000) {
        sceSifSetReg(4, 0x40000);
        func_0010CE28();
        return 1;
    }
    return 0;
}
