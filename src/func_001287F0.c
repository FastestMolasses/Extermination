// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C67E0(int, int);

void func_001287F0(int a0, short *a1, short a2) {
    if (a1[0xF8 / 2] == a2) return;
    a1[0xF8 / 2] = a2;
    func_001C67E0(a0, a2);
}
