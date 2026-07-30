// NEARMISS func_0010ED18  (vram 0x0010ED18, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 99.92% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SDK RPC/DMA submit helper (vram 0x0010ED18).
//
// Builds the outgoing packet for a request descriptor, flushes the caches for
// the payload, allocates a command packet under an interrupt-disabled section,
// and hands the whole thing to the IOP either through the "send command"
// path (func_0010DE38) or through a 2-entry SIF DMA transfer list, retrying
// with a busy-wait until the transfer is queued.
//
// Matching notes (ee-gcc 2.9-991111-01, -O2):
//   * `di`/`sync.p`/`mfc0` are emitted as three separate asm statements: the
//     MIPS backend pads the branch that consumes the asm result with nops, and
//     the pad count is decremented once per emitted insn -- three statements
//     give the single nop the target has between `and` and `bnez`.
//   * The transfer list is volatile: the target writes src/dest/size/attr in
//     source order, which only happens when the stores cannot be reordered.
//   * Pkt.f1C / Req.f1C are pointers so that the `req->f1C` load sits in a
//     different alias set from the `pkt->f20` store and can be hoisted above
//     it, exactly as the target does.
//   * `i` is pinned to $s2: ee-gcc ranks it and `src` equally in
//     allocno_compare and picks the opposite order. The annotation only
//     constrains register allocation; drop it for a non-MIPS build.

typedef struct DmaXfer {
    void *src;
    void *dest;
    int size;
    int attr;
} DmaXfer;

typedef struct Pkt {
    int f00;
    int f04;
    int f08;
    int f0C;
    int f10;
    int f14;
    int f18;
    void *f1C;
    unsigned int f20;
} Pkt;

typedef struct Req {
    int f00;
    void *(*f04)(int, void *, int);
    void *f08;
    int f0C;
    int f10;
    int f14;
    int f18;
    void *f1C;
    void *f20;
    int f24;
    void *f28;
    int f2C;
    int f30;
    unsigned int f34;
} Req;

extern char D_00278BC0[];

extern void func_0010DFD8(void *addr, int size);
extern void *func_0010E338(void *pool);
extern void *func_0010E368(void *pool, unsigned int idx);
extern int func_0010DE38(unsigned int cmd, void *pkt, int size, void *src, void *dest, int len);
extern int iGsPutIMR(volatile DmaXfer *xfer, int count);

int func_0010ED18(Req *req)
{
    volatile DmaXfer xfer[2];
    void *src;
    int len;
    Pkt *pkt;
    unsigned int st;
    register int i __asm__("$18");
    int id;
    int j;

    len = 0;
    src = req->f04(req->f24, req->f08, req->f0C);
    if (src != 0) {
        len = req->f2C;
    }
    if (req->f0C > 0) {
        func_0010DFD8(req->f08, req->f0C);
    }
    if (len > 0) {
        func_0010DFD8(src, len);
    }

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r" (st));
    } while (st & 0x10000);
    if (req->f34 & 4) {
        pkt = (Pkt *)func_0010E368(D_00278BC0, req->f34 >> 16);
    } else {
        pkt = (Pkt *)func_0010E338(D_00278BC0);
    }
    __asm__ __volatile__("ei");

    pkt->f20 = 0x8000000A;
    pkt->f1C = req->f1C;

    if (req->f30 != 0) {
        do {
            id = func_0010DE38(0x80000008, pkt, 0x40, src, req->f28, len);
        } while (id == 0);
        return id;
    }

    pkt->f18 = 0;
    pkt->f10 = 0;
    i = 0;
    if (len > 0) {
        xfer[0].src = src;
        xfer[0].dest = req->f28;
        xfer[0].size = len;
        xfer[0].attr = 0;
        i = 1;
    }
    xfer[i].src = pkt;
    xfer[i].dest = req->f20;
    xfer[i].size = 0x40;
    xfer[i].attr = 0;
    i++;

    do {
        id = iGsPutIMR(xfer, i);
        if (id != 0) {
            break;
        }
        j = 0x100000;
        do {
            j--;
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
        } while (j != -1);
    } while (id == 0);
    return id;
}
