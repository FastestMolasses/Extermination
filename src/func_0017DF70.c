// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int);

void func_0017DF70(int a0, int a1) {
    if (a1 == 0) {
        func_001749A0(a0, 0x7E, 0);
    } else {
        func_001749A0(a0, 0x7F, 0);
    }
}
