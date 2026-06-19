// COMPILER: eegcc
// CFLAGS: -O2
// If a2 names one of the two known strings, bind it into a0 (f30/f34) and
// return the table base; if it matches neither, return 0.
extern int func_00123020(int a0, char *s);
extern char D_0026C800[];
extern char D_0026C7F0[];

struct rec {
    char pad30[0x30];
    int f30;    // 0x30
    int f34;    // 0x34
};

char *func_00120A48(struct rec *a0, int a1, int a2) {
    if (a2 != 0) {
        if (func_00123020(a2, D_0026C800) != 0 &&
            func_00123020(a2, D_0026C7F0) != 0) {
            return 0;
        }
        a0->f30 = a1;
        a0->f34 = a2;
    }
    return D_0026C800;
}
