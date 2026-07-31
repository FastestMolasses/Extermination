// COMPILER: eegcc
// CFLAGS: -O2

extern int func_001217D8(const void *s1, const void *s2, unsigned int n);
extern unsigned char D_00279668[];
extern unsigned char D_00241C5C[];

int sub__9PsIIlibkernl2000_4(void)
{
    unsigned char *magic = D_00241C5C;
    unsigned char *buf = D_00279668;

    return func_001217D8(buf, magic, 4);
}
