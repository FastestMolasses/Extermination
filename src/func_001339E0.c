// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008102B0[];
extern void func_0021C040(char *, int);

void func_001339E0(int a0, unsigned char *a1) {
    if (a1[0x65] == 0) return;
    a1[0x65] = 0;
    func_0021C040(D_008102B0, a0);
}
