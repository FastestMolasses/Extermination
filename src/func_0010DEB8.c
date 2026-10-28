// NEARMISS func_0010DEB8  (vram 0x0010DEB8, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 97.92% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful. Wave
// 2026-10-27: the packet pointer is copied into the copy cursor AFTER the header byte is
// cleared, and the rounded qword count reuses the length variable (was 95.00%).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS: 97.92% - Remaining differences: register colouring in the prologue/copy region
// only. The original keeps the loop down-counter in its own register (a copy of the qword
// count), so the packet pointer, length and copy cursor each sit one argument register higher,
// and the post-call command-id test loads into the second return register. Size exact.

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

    len = (len + 15) / 16;
    *(volatile unsigned char *)pkt = 0;
    src = (u128 *)pkt;
    for (i = 0; i < len; i++)
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
