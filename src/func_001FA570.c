// CFLAGS: -O4,p -sdatathreshold 4
extern char D_00281CF0[0x40];
extern unsigned char D_00275B34;
extern unsigned char D_00275B30;
extern void func_00121A28(char *, int, int);

void func_001FA570(void) {
    char *a0 = D_00281CF0;
    int a1 = 0xff;
    func_00121A28(a0, a1, 0x40);
    D_00275B34 = 0;
    D_00275B30 = 0;
}
