// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
/* arg2 is an int index (callers pass it as int); it is bounds-checked and
 * scaled as unsigned. */
int func_001FE4D0(int arg0, int arg1, int arg2) {
    int a3;
    int base;
    int e;
    int r;

    a3 = arg0 + 0x10;
    base = arg0 + *(int *)arg0;
    if (arg1 >= *(int *)(arg0 + 4))
        return 0;
    e = (arg1 << 4) + a3;
    if ((unsigned int)arg2 >= (unsigned int)(*(unsigned int *)(e + 0xC) >> 4))
        return 0;
    r = base + *(int *)e;
    r = r + ((unsigned int)arg2 << 4);
    return r;
}
