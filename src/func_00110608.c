// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 60.70% NEARMISS).
// Fixes read from the original instructions:
//  - The slot address is formed twice with different association, so gcc does
//    not share it: the counter is reached through &D_00279750[port][slot]
//    (table base plus port*0x180, then plus slot*0x60) and the IOP address word
//    through the sibling func_00110AB8's spelling (slot*0x60 + port*0x180, then
//    plus the base). One shared byte pointer was the old "reg-alloc wall".
//  - The 0x110-byte frame is a 16-entry DMA descriptor array (0x100 bytes) of
//    typed {src, dest, size, attr} records, of which entry 0 is queued; the
//    debug print callee returns int and takes a variadic format.
//
// For the slot at port a0 / slot a1 of the 2x4 table D_00279750 (stride 0x60):
// increments the slot's counter word at +0x20, then queues one SIF DMA
// transfer of 0x14 bytes (attr 0) from that counter word to the IOP address
// held in the slot's +0x10 word. If sceSifSetDma returns 0 (not queued) it
// prints the D_0026B9E0 message.

typedef struct {
    void *src;
    void *dest;
    int size;
    int attr;
} SifDmaTransfer;

typedef struct {
    int a;
    int b;
    int c;
    void *buf;
    void *iop;
    int pad[3];
    int cnt;
    int d[15];
} Slot;

extern int sceSifSetDma(void *p, int n);
extern int func_00122B58(char *fmt, ...);
extern char D_0026B9E0[];
extern Slot D_00279750[2][4];

void func_00110608(int a0, int a1)
{
    SifDmaTransfer dma[16];
    unsigned char *e = (a1 * 0x60 + a0 * 0x180) + (unsigned char *)D_00279750;
    Slot *s = &D_00279750[a0][a1];

    dma[0].size = 0x14;
    dma[0].attr = 0;
    s->cnt++;
    dma[0].src = &s->cnt;
    dma[0].dest = *(void **)(e + 0x10);
    if (sceSifSetDma(dma, 1) == 0)
        func_00122B58(D_0026B9E0);
}
