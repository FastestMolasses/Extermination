// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Matched via mwcc 2.3.3 (idiom-13-family delay-slot/dead-const wall that 991202 fills).
extern void func_0015C1F0(void);
extern int func_001EFE00(int, void *);
extern void func_001FAFD0(void);
extern unsigned char D_008106F0;
extern unsigned char D_008106F1;

int func_0021C190(unsigned char *arg0) {
    unsigned char v1 = arg0[0x31F];
    arg0[0x31F] = v1 - 1;
    if (v1 == 0) {
        func_0015C1F0();
        *(int *)(arg0 + 0x1C) = func_001EFE00(0x80000048, arg0);
        D_008106F0 = 0;
        func_001FAFD0();
        D_008106F1 = 0;
        return 1;
    }
    return 0;
}
