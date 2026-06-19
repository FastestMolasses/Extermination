// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): set *errno = 5, return -1.
extern int *func_0011FD78(void);

int func_0010BE28(void) {
    int *p = func_0011FD78();
    *p = 5;
    return -1;
}
