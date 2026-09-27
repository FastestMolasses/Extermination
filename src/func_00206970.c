// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SIF DMA send: validate count, build a one-entry transfer descriptor
// (arg1, arg0, n, 0), FlushCache(0), submit via sceSifSetDma, busy-poll
// sceSifDmaStat until it returns negative (transfer done), FlushCache(0) again,
// return the count.

extern void FlushCache(int);
extern int sceSifDmaStat(int);
extern int sceSifSetDma(void *, int);

int func_00206970(int arg0, int arg1, int n) {
    int buf[4];
    int id;
    if (n <= 0) return 0;
    buf[0] = arg1;
    buf[1] = arg0;
    buf[2] = n;
    buf[3] = 0;
    FlushCache(0);
    id = sceSifSetDma(buf, 1);
    while (sceSifDmaStat(id) >= 0)
        ;
    FlushCache(0);
    return n;
}
