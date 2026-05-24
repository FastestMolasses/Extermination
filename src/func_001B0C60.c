// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001B0C00(int);
extern char D_008106B8;
extern char D_008106B5;
extern char D_008106B7;
extern char D_008106B6;

void func_001B0C60(int a0, int a1, int a2) {
    *(volatile char *)(0x70003B8D) = 3;
    func_001B0C00(4);
    D_008106B5 = a0;
    D_008106B7 = a2;
    D_008106B6 = a1;
    D_008106B8 = 1;
}
