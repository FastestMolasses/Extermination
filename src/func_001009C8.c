// COMPILER: eegcc
// CFLAGS: -O2
extern void *func_00100268(void);
extern void func_0010BC90(void);
extern long func_0010BCD0(void);

int func_001009C8(void) {
    void *p = func_00100268();

    if (*(int *)((char *)p + 8) == 0) {
        func_0010BC90();
        if (*(short *)p == 1) {
            return (int)((*(volatile unsigned long *)0x12001000 >> 13) & 1);
        }
        return 1;
    } else {
        long v = (func_0010BCD0() >> 13) & 1;
        if (*(short *)p == 1) {
            return (int)v;
        }
        return 1;
    }
}
