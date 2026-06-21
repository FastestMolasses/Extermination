// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// func_002034C0() returns a status; on 0 just run func_00203C90() and return 1,
// otherwise call func_002036E0(D_002DF740, D_0028F700, <status>) then func_00203C90().
// The third arg (the status value) is what makes the compiler materialize the
// `paddub a2,v0,zero` (v0->a2 via parallel-add) idiom in BOTH branch delay slots,
// matching the target. mwcc 2.3.3 reproduces this; the pinned 991202 build does not.
extern char D_002DF740[];
extern char D_0028F700[];
extern int func_002034C0(void);
extern void func_00203C90(void);
extern void func_002036E0(char *, char *, int);

int func_00203460(void) {
    int r = func_002034C0();
    if (r == 0) {
        func_00203C90();
        return 1;
    }
    func_002036E0(D_002DF740, D_0028F700, r);
    func_00203C90();
    return 1;
}
