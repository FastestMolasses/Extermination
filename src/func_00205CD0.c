// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00205A50(int a0, int a1, unsigned long long a2);

void func_00205CD0(int a0, unsigned int a1, unsigned int a2, unsigned int a3) {
    func_00205A50(a0, 5, (unsigned long long)a1 |
                         ((unsigned long long)a2 << 16) |
                         ((unsigned long long)a3 << 32));
}
