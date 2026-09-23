// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 23.33% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. Source unchanged apart from the compiler line.

extern char D_008102B0[];
extern float D_00810374;
extern void func_00182F90(char *, int);

void func_001B6F80(int a0, float f0) {
    int a1 = a0;
    D_00810374 = f0;
    func_00182F90(D_008102B0, a1);
}
