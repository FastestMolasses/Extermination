// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FE070(int, int, int, int);
extern int *D_0028A49C;

void func_001FCF60(int a0, int a1, int a2) {
    int *p = D_0028A49C;
    int base = (int)p + p[0];
    func_001FE070(p[8] + base, a0, a1, a2);
}
