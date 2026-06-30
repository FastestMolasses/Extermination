// COMPILER: eegcc
// CFLAGS: -O2
int func_0010E338(int *a0) {
    int r = a0[9] % a0[6];
    int ret = a0[5] + (r << 6);
    a0[9] = r + 1;
    return ret;
}
