// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
int func_001FE4D0(int arg0, int arg1, unsigned int arg2) {
    int a3;
    int base;
    int e;
    int r;

    a3 = arg0 + 0x10;
    base = arg0 + *(int *)arg0;
    if (arg1 >= *(int *)(arg0 + 4))
        return 0;
    e = (arg1 << 4) + a3;
    if (arg2 >= (unsigned int)(*(unsigned int *)(e + 0xC) >> 4))
        return 0;
    r = base + *(int *)e;
    r = r + (arg2 << 4);
    return r;
}
