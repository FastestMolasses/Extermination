// COMPILER: eegcc
// CFLAGS: -O2
// SDK: r = func_00108640(a0, 1); func_00108660(a0, 1); return r.
extern int func_00108640(int a0, int a1);
extern int func_00108660(int a0, int a1);

int func_00108748(int a0) {
    int r = func_00108640(a0, 1);
    func_00108660(a0, 1);
    return r;
}
