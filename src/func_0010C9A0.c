// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: Deci2Call(0x10, &local) where local = a0.
extern int Deci2Call(int a0, int *p);

int func_0010C9A0(int a0) {
    int local = a0;
    return Deci2Call(0x10, &local);
}
