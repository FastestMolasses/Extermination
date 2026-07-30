// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241C88;
extern int D_00279040;
extern unsigned char D_00279210[];

struct XferReq {
    int cmd;
    unsigned int addr;
    int size;
    int head_len;
    unsigned char head[16];
};
extern struct XferReq D_00278C00;

extern int *func_0010EFA8(void);
extern void func_0010DFD8(unsigned int addr, int size);
extern int func_0010E8A8(void *pkt, int a1, int a2, void *req, int a4, void *a5, int a6, int a7, int a8);

int func_0010F650(int unused, unsigned int addr, int size)
{
    struct XferReq *req;
    int *fh;
    int flags;
    unsigned int prev;
    int n;
    int i;
    int nowait;

    req = &D_00278C00;
    fh = func_0010EFA8();
    if (D_00241C88 == 0) {
        return -1;
    }
    if (fh == 0) {
        return -9;
    }
    if (fh[1] == 0) {
        return -9;
    }

    D_00278C00.cmd = fh[0];
    req->size = size;
    req->addr = addr;
    if ((addr & 0xF) == 0) {
        n = 0;
    } else {
        prev = addr - 0x10;
        n = ((addr >> 4) << 4) - prev;
    }
    if (size < n) {
        n = size;
    }
    flags = fh[1] & 0x8000;
    nowait = flags != 0;
    if ((fh[1] & 0x20000000) == 0) {
        func_0010DFD8(addr, size);
    }
    addr |= 0x20000000;
    req->head_len = n;
    for (i = 0; i < n; i++) {
        req->head[i] = *(unsigned char *)(addr + i);
    }
    if (func_0010E8A8(D_00279210, 3, nowait, &D_00278C00, 0x20, &D_00279040, 4, 0, 0) < 0) {
        return -1;
    }
    if ((fh[1] & 0x8000) != 0) {
        return 0;
    }
    return D_00279040;
}
