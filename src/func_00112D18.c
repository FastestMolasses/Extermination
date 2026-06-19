// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241CF8;
extern volatile int D_00241D14;
extern int D_00279E00;
extern int D_0026BCA0;
extern void func_00122B58(int *a0);
extern int func_0010EA60(int *a0);
int func_00112D18(int a0) {
    if (a0 == 0) {
        if (D_00241CF8 > 0) {
            func_00122B58(&D_0026BCA0);
        }
        do {
            while (D_00241D14 != 0) {
                ;
            }
        } while (func_0010EA60(&D_00279E00) != 0);
        return 0;
    }
    if (D_00241D14 != 0 || func_0010EA60(&D_00279E00) != 0) {
        return 1;
    }
    return 0;
}
