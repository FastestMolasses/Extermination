// COMPILER: eegcc
// CFLAGS: -O2
extern void func_001063E8(void);
extern void func_00106830(int a0);
extern int func_001066F8(int a0);
void func_00106AB0(void) {
    int a0v;
    func_001063E8();
    a0v = (-(*(volatile int *)0x10002020 & 7)) & 7;
    if (a0v != 0) {
        func_00106830(a0v);
    }
    while (func_001066F8(0x18) != 1) {
        func_00106830(8);
    }
}
