// COMPILER: eegcc
// CFLAGS: -O2
// SDK polling loop (ee-gcc 2.96): while func_00106948(1) is nonzero, call
// func_00106830(8).
extern int func_00106948(int);
extern void func_00106830(int);

void func_00107060(void) {
    while (func_00106948(1)) {
        func_00106830(8);
    }
}
