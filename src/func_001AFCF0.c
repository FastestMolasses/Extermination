// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008106B0[0x80];
extern void func_00121A28(char *, int, int);
extern void func_001FC9B0(void);

void func_001AFCF0(void) {
    char *a0 = D_008106B0;
    *(short *)0x70003B84 = 0;
    *(unsigned char *)0x70003B93 = 0;
    *(unsigned char *)0x70003B8C = 0;
    *(unsigned char *)0x70003B8D = 0;
    *(unsigned char *)0x70003B8E = 0;
    *(unsigned char *)0x70003B8F = 0;
    *(unsigned char *)0x70003B91 = 0;
    *(unsigned char *)0x70003B92 = 0;
    *(int *)0x70003258 = 0;
    func_00121A28(a0, 0, 0x48);
    func_001FC9B0();
}
