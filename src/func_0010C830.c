// COMPILER: eegcc
// CFLAGS: -O2
extern int Deci2Call(int chan, void *arg);

int func_0010C830(int a0) {
    int local = a0;
    return Deci2Call(2, &local);
}
