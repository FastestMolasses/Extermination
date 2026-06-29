// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027CCC0[];

int func_0011A470(int idx)
{
    unsigned char *t, *q;
    int off;

    if (idx < 0x30) {
        t = D_0027CCC0;
        q = t + 0x1A;
        off = idx * 0x6A;
        if (*(unsigned short *)(q + off) == 3) {
            goto ok;
        }
    }
    return -1;
ok:
    *(unsigned short *)(t + off) = 0;
    *(unsigned short *)(q + off) = 0;
    return 0;
}
