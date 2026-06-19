// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): build a local block then process it.
extern void func_00122D10(void *buf, int a0, int a1);
extern void func_0010A3A8(void *buf);

void func_0010A378(int a0, int a1) {
    unsigned char buf[0x100];
    func_00122D10(buf, a0, a1);
    func_0010A3A8(buf);
}
