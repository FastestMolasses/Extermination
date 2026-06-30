// COMPILER: eegcc
// CFLAGS: -O2

void func_00106070(int *ptr, int shift, int dir, int off, int flag)
{
    int lim = 0x10 << shift;
    int v = *ptr;
    int one = 1;

    if (flag)
        v = v >> one;
    if (dir > 0) {
        v = ((v + one) & 0xFFFFFFFFFFFFFFFF) + (((dir - one) << shift) + off);
        if (v >= lim)
            v = v - (lim << one);
    } else if (dir < 0) {
        v = (v - one) - (((~dir) << shift) + off);
        if (v < -lim)
            v = v + (lim << one);
    }
    *ptr = flag ? (v << one) : v;
}
