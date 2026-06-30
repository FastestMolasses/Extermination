// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0011AFD0(void);
extern int func_0011B018(int);
extern int func_0011B028(void);

int func_0011AF00(void) {
    int result = 0;
    int v = func_0011AFD0();
    if (v < 4) {
        if (v > 0) {
            result = 1;
            func_0011B018(func_0011B028());
        }
    }
    return result;
}
