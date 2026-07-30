// NEARMISS func_0010DEB8  (vram 0x0010DEB8, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 95.00% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring + list-scheduler wall, confined to the prologue/qword-copy region. Object size is exact (288 == 288, symbol sizes identical) and the entire second half (isceSifSetDChain call, cid dispatch, both sys/usr handler-table paths, sync/ei/epilogue) is byte-identical. Residual = 19 ins...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS: 95.0% - eegcc GPR-coloring + list-scheduler wall in the prologue/copy
// region only (size exact; dispatch tail byte-identical). Body/logic fully recovered.

typedef unsigned int u128 __attribute__((mode(TI)));

typedef struct {
    void (*handler)(void *, void *);
    void *arg;
} SifCmdEntry;

extern void isceSifSetDChain(void);

extern char D_00277218[];

int func_0010DEB8(void)
{
    u128 buf[7];
    char *ctx;
    unsigned char *pkt;
    u128 *src;
    SifCmdEntry *tbl;
    int len;
    int nqw;
    int i;
    int cid;
    int idx;
    void (*handler)(void *, void *);

    ctx = D_00277218;
    __asm__ __volatile__("ei");

    pkt = *(unsigned char * volatile *)(D_00277218 + 0x0);
    len = *(volatile unsigned char *)pkt & 0xFF;
    if (len == 0)
        return 0;

    src = (u128 *)pkt;
    nqw = (len + 15) / 16;
    *(volatile unsigned char *)pkt = 0;
    for (i = 0; i < nqw; i++)
        buf[i] = *src++;

    isceSifSetDChain();

    cid = *(volatile int *)((char *)buf + 8);
    if (cid < 0) {
        idx = *(volatile int *)((char *)buf + 8) & 0x7FFFFFFF;
        if (idx < *(int *)(ctx + 0x10)) {
            tbl = *(SifCmdEntry **)(ctx + 0xC);
            handler = tbl[idx].handler;
            if (handler != 0)
                handler(buf, tbl[idx].arg);
        }
    } else {
        idx = *(volatile int *)((char *)buf + 8);
        if (idx < *(int *)(ctx + 0x18)) {
            tbl = *(SifCmdEntry **)(ctx + 0x14);
            handler = tbl[idx].handler;
            if (handler != 0)
                handler(buf, tbl[idx].arg);
        }
    }

    __asm__ __volatile__("sync");
    __asm__ __volatile__("ei");
    return 0;
}
