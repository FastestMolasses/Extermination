// COMPILER: eegcc
// CFLAGS: -O2
// SDK: r = func_0011AB20(); if (r in 1..3) { func_0011AB60(func_0011AB70()); ret=1; } return ret.
extern int func_0011AB20(void);
extern int func_0011AB70(void);
extern void func_0011AB60(int a0);

int func_0011AA50(void) {
    int ret = 0;
    int r = func_0011AB20();
    if (r < 4) {
        if (r > 0) {
            ret = 1;
            func_0011AB60(func_0011AB70());
        }
    }
    return ret;
}
