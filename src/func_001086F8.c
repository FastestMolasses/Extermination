// COMPILER: eegcc
// CFLAGS: -O2
// Save result of func_00108640(a0, a1), then call func_00108660(a0, a1); return saved.
extern int func_00108640(int a0, int a1);
extern void func_00108660(int a0, int a1);

int func_001086F8(int a0, int a1) {
    int r = func_00108640(a0, a1);
    func_00108660(a0, a1);
    return r;
}
