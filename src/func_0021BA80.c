// CFLAGS: -O4,p -sdatathreshold 0
extern void func_0021BA70(long long);

void func_0021BA80(int a0, int a1, int a2) {
    func_0021BA70((long long)a0 | ((long long)a1 << 8) | ((long long)a2 << 16));
}
