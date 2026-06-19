// COMPILER: eegcc
// CFLAGS: -O2
// SDK: if a live handler chain exists, enqueue via func_00109B20({0, a0});
// else dispatch directly via func_0010A368(a0).
extern int *D_002412F4;
extern int func_00109B20(int a0, int *p);
extern void func_0010A368(int a0);

void func_0010A3A8(int a0) {
    int *p = D_002412F4;
    if (p != 0 && p[0x10] != 0 && ((int *)p[0x10])[3] != 0) {
        int local[2];
        local[1] = a0;
        local[0] = 0;
        func_00109B20((int)p, local);
    } else {
        func_0010A368(a0);
    }
}
