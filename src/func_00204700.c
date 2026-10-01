// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Movie decoder: suspends the IPU transfer and saves its state. Under the
// context semaphore (+0x40) it clears +0x44, stops DMA channel 4 (toIPU) with
// func_00204140(5) and saves its MADR / TADR / QWC / CHCR to +0x1C..+0x28,
// waits until the IPU_CTRL FIFO count (bits 4..7) drains, stops channel 3
// (fromIPU) with func_002040E0(0) and saves its MADR / QWC / CHCR plus IPU_BP
// and IPU_CTRL to +0x2C..+0x3C. Returns 1.
typedef struct IpuSave {
    char pad[0x1C];
    int to_madr;    /* 0x1C */
    int to_tadr;    /* 0x20 */
    int to_qwc;     /* 0x24 */
    int to_chcr;    /* 0x28 */
    int from_madr;  /* 0x2C */
    int from_qwc;   /* 0x30 */
    int from_chcr;  /* 0x34 */
    int ipu_bp;     /* 0x38 */
    int ipu_ctrl;   /* 0x3C */
    int sema;       /* 0x40 */
    int busy;       /* 0x44 */
} IpuSave;

extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern void func_00204140(int mode);
extern void func_002040E0(int mode);

int func_00204700(IpuSave *s) {
    WaitSema(s->sema);
    s->busy = 0;
    func_00204140(5);
    s->to_madr = *(volatile int *)0x1000B410;
    s->to_tadr = *(volatile int *)0x1000B430;
    s->to_qwc = *(volatile int *)0x1000B420;
    s->to_chcr = *(volatile int *)0x1000B400;
    while (*(volatile int *)0x10002010 & 0xF0) {
    }
    func_002040E0(0);
    s->from_madr = *(volatile int *)0x1000B010;
    s->from_qwc = *(volatile int *)0x1000B020;
    s->from_chcr = *(volatile int *)0x1000B000;
    s->ipu_bp = *(volatile int *)0x10002020;
    s->ipu_ctrl = *(volatile int *)0x10002010;
    SignalSema(s->sema);
    return 1;
}
