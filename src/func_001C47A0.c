// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C40B0(int);
extern volatile char D_008106B0;
extern volatile char D_008106B1;

int func_001C47A0(int a0) {
    func_001C40B0(a0);
    D_008106B0 = 1;
    D_008106B1 = a0;
    return 0;
}
