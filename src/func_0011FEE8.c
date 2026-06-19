// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): allocate a node block, init it, clear its payload.
extern void *func_00120F40(int a0, int a1);
extern void func_00121A28(void *p, int a1, int a2);

void *func_0011FEE8(int a0, int a1) {
    int sz = a1 * 0x58;
    char *p = func_00120F40(a0, sz + 0xC);
    if (p == 0) {
        return 0;
    }
    *(int *)(p + 0x4) = a1;
    *(int *)(p + 0x0) = 0;
    *(void **)(p + 0x8) = p + 0xC;
    func_00121A28(p + 0xC, 0, sz);
    return p;
}
