// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
int func_00203A80(int *arg0, int *arg1) {
    int v1;

    v1 = arg0[0x14001];
    if (v1 != 0) {
        int a2 = arg0[0x14002];
        *arg1 = (int)arg0 + ((a2 + (arg0[0x14000] - v1)) % a2);
    }
    return arg0[0x14001];
}
