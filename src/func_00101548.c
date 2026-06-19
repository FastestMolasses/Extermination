// COMPILER: eegcc
// CFLAGS: -O2
extern int *func_00100268(int a0);
extern int AddIntcHandler(int a0, int a1, int a2);
extern void func_0010C2F8(int a0);

int func_00101548(int a0) {
    int *s1 = func_00100268(a0);
    int s2 = s1[2];
    s1[2] = a0;
    s1[3] = AddIntcHandler(2, a0, -1);
    func_0010C2F8(2);
    return s2;
}
