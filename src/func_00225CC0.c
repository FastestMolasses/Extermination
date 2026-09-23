// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 66.67% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. Source unchanged apart from the compiler line.

extern char D_00810040[];
extern void func_00121A28(char *, int, int);
extern void func_001FE910(void);

void func_00225CC0(void) {
    char *a0 = D_00810040;
    int a2 = 0xD4;
    func_00121A28(a0, 0, a2);
    func_001FE910();
}