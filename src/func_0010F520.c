// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK IOP-RPC style write/send wrapper: look up the device handle via
// func_0010EFA8, fill the request block at D_00278C00, flush the caches
// (func_0010DFD8) and issue the transfer (func_0010E8A8) with the completion
// callback func_0010F490.  Returns -1 (not initialised / send failed),
// -9 (bad handle), 0 (async) or the result word D_00279040.

struct dev {
    int fd;      /* 0x00 */
    int flags;   /* 0x04 */
};

struct req {
    int fd;      /* 0x00 */
    int buf;     /* 0x04 */
    int size;    /* 0x08 */
    void *sub;   /* 0x0C */
};

extern struct dev *func_0010EFA8(int);
extern void func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern void func_0010F490(void);
extern int D_00241C88;
extern struct req D_00278C00;
extern char D_00279080;
extern char D_00279210;
extern int D_00279040;

int func_0010F520(int a0, int a1, int a2) {
    struct req *p = &D_00278C00;
    struct dev *d;
    int isasync;
    int t;

    d = func_0010EFA8(a0);
    if (D_00241C88 == 0)
        return -1;
    if (d == 0)
        return -9;
    if (d->flags == 0)
        return -9;

    p->fd = d->fd;
    p->buf = a1;
    p->size = a2;
    p->sub = &D_00279080;

    t = d->flags & 0x8000;
    isasync = (t != 0);
    if ((d->flags & 0x20000000) == 0)
        func_0010DFD8((void *)a1, a2);
    func_0010DFD8(&D_00279080, 0x90);
    func_0010DFD8(p, 0x10);

    if (func_0010E8A8(&D_00279210, 2, isasync, p, 0x10, &D_00279040, 4,
                      (void *)func_0010F490, &D_00279080) < 0)
        return -1;
    if (d->flags & 0x8000)
        return 0;
    return D_00279040;
}
