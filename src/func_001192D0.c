// COMPILER: eegcc
// CFLAGS: -O2
extern int D_0027F740[80];
extern unsigned char D_00281B00[];

extern void *func_0010EA60(void *);

void *func_001192D0(void)
{
    volatile int *p = D_0027F740;
    void *r = 0;
    if (p[0x11] != 0) {
        r = func_0010EA60(D_00281B00);
    }
    return r;
}
