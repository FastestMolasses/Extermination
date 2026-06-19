// COMPILER: eegcc
// CFLAGS: -O2
// SDK shutdown/flush: optionally log, then drain func_0010EA60 on D_0027AF60.
extern int D_00241CF8;
extern int D_0026BCB0;
extern int D_0027AF60;
extern void func_00122B58(int *fmt, ...);
extern int func_0010EA60(int *a0);

int func_00112DC0(int a0) {
    if (a0 == 0) {
        if (D_00241CF8 > 0) {
            func_00122B58(&D_0026BCB0);
        }
        while (func_0010EA60(&D_0027AF60) != 0) {
        }
        return 0;
    }
    return func_0010EA60(&D_0027AF60);
}
