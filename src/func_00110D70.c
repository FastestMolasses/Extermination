// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char *func_00110AB8(int a, int b, int c, int d);

int func_00110D70(int a, int b, int idx, int dir)
{
    unsigned char *p;
    p = func_00110AB8(a, b, idx, dir);
    if (p[0x72] != 1)
    {
        return 0;
    }
    if (p[0x64] < 2)
    {
        return 0;
    }
    if (idx == (-1))
    {
        return p[0x6B];
    }
    if (idx < p[0x6B])
    {
        do { switch (dir) { case -1: return (p + ((idx << 1) << 1))[0x40]; case 0: return (p + (idx << 2))[0x41]; case 1: return (p + (idx << 2))[0x42]; case 2: return (p + (idx << 2))[0x43]; } } while (0);
    }
    return 0;
}
