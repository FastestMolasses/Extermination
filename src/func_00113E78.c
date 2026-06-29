// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241CF8;
extern int D_00241D40;
extern int D_0026BE60;
extern int D_00241D50;
extern int func_00122B58(void *);
extern int func_00113F68(int, int, int, int, void *);

void func_00113E78(void) {
    int n = D_00241CF8;
    D_00241D40 = 0;
    if (n > 0) {
        func_00122B58(&D_0026BE60);
    }
    func_00113F68(0, 0, 0, 7, &D_00241D50);
    __asm__ __volatile__("" ::: "memory");
}
