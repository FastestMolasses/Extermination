// CFLAGS: -O4,p -sdatathreshold 4
extern char D_002821B0[0x100];
extern int D_0026EC10[2];
extern char D_00264D10[8];
extern char *D_002821D0;
extern int D_00275C50;
extern unsigned char D_00275C54;
extern unsigned char D_00275C55;
extern void func_00121A28(char *, int, int);

int func_001FC9B0(void) {
    func_00121A28(D_002821B0, 0, 0x9c);
    D_00275C50 = D_0026EC10[0];
    D_00275C54 = 0x80;
    D_002821D0 = D_00264D10;
    D_00275C55 = 0;
    return 1;
}
