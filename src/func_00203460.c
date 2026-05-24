// CFLAGS: -O4,p -sdatathreshold 0
extern char D_002DF740[8];
extern char D_0028F700[8];
extern int func_002034C0(void);
extern void func_00203C90(void);
extern void func_002036E0(char *, char *, int);

int func_00203460(void) {
    int a2;
    int v0;

    v0 = func_002034C0();
    a2 = v0;
    if (v0 == 0) {
        func_00203C90();
        return 1;
    }
    func_002036E0(D_002DF740, D_0028F700, a2);
    func_00203C90();
    return 1;
}
