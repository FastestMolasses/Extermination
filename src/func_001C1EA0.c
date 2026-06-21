// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Matched via mwcc 2.3.3 (idiom-13-family delay-slot/dead-const wall that 991202 fills).
extern int func_001B0070(void);
extern void func_001EFD20(int, void *);
extern char D_00250F00;
extern char D_00250F10;
extern char D_00250F20;

void func_001C1EA0(void) {
    int v0 = func_001B0070();
    if (v0 & 0x02000010) {
        func_001EFD20(0x80000017, &D_00250F00);
        return;
    }
    if (v0 & 0x04000020) {
        func_001EFD20(0x80000017, &D_00250F10);
        return;
    }
    if (v0 & 0x08000040) {
        func_001EFD20(0x80000017, &D_00250F20);
    }
}
