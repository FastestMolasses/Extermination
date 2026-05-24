// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C84;
extern char D_007A55A0[8];
extern void func_00206010(char *);

int func_00206170(void) {
    int v0 = D_00275C84;
    if (v0 != 0) {
        func_00206010(D_007A55A0);
        D_00275C84 = 0;
        asm { sync }
        asm { ei }
    }
    return 0;
}
