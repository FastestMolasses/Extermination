// COMPILER: eegcc
// CFLAGS: -O2
// SDK: p = func_0011FD78(); *p = 5; return -1.
extern int *func_0011FD78(void);

int func_0010BF88(void) {
    int *p = func_0011FD78();
    *p = 5;
    return -1;
}
