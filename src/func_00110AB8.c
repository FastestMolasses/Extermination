// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_00279750[];
extern void func_0010C0C8(void *a, void *b);

void *func_00110AB8(int a0, int a1)
{
    unsigned char *e = (a1 * 0x60 + a0 * 0x180) + D_00279750;
    unsigned char *p = *(unsigned char **)(e + 0xC);
    int n;
    func_0010C0C8(p, p + 0x100);
    n = (*(int *)(p + 0x58) < *(int *)(p + 0xD8)) << 7;
    return p + n;
}
