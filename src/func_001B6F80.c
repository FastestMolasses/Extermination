// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008102B0[];
extern float D_00810374;
extern void func_00182F90(char *, int);

void func_001B6F80(int a0, float f0) {
    int a1 = a0;
    D_00810374 = f0;
    func_00182F90(D_008102B0, a1);
}
