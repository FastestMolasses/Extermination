// CFLAGS: -O4,p -sdatathreshold 0
extern char D_700038A0[0x100];
extern void func_001026A0(char *, char *, char *);

void func_00180420(int a0) {
    *(int *)0x700038a0 = 0;
    *(int *)0x700038a4 = 0;
    *(int *)0x700038a8 = 0xc0400000;
    *(int *)0x700038ac = 0x3f800000;
    func_001026A0((char *)a0 + 0x290, (char *)a0 + 0xd0, D_700038A0);
}
