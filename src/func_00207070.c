// CFLAGS: -O4,p -sdatathreshold 0
extern int D_002DF788;
extern void func_00203980(void);
extern void func_002044F0(int *);

int func_00207070(void) {
    func_00203980();
    func_002044F0(&D_002DF788);
    return 1;
}