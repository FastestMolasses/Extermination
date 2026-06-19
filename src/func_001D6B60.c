// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001D6930(int a0, int a1, int a2, int a3, int a4);
extern void func_001D1F20(int a0);

int func_001D6B60(int a0, int a1, int a2, int a3, int a4) {
    int r = func_001D6930(a0, a1, a2, a3, a4);
    func_001D1F20(a0);
    return r;
}
