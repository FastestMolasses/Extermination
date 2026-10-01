// COMPILER: eegcc
// CFLAGS: -O2
// Movie library (SDK libmpeg, title path): resets a stream's state: the
// 64-bit word +0x48 is cleared and +4 set to 0x2000. The first argument is
// not used. Returns 0.
int func_0010BF18(void *unused, char *s) {
    *(long long *)(s + 0x48) = 0;
    *(int *)(s + 4) = 0x2000;
    return 0;
}
