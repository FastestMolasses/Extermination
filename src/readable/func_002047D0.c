// NEARMISS func_002047D0  (vram 0x002047D0, 0x30C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 75.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register allocation and scheduling across the two wrap paths (the body follows the original's
// arithmetic; not iterated further).
//
// The function links from the asm body in src/func_002047D0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Movie decoder: resumes the IPU input transfer saved by func_00204700.
// The quadwords still buffered in the IPU (IPU_BP FP + IFC, saved in +0x38)
// are pushed back: the toIPU MADR moves back by that many quadwords and QWC
// grows by it. Under the semaphore (+0x40):
//  - when the rewound MADR falls before the ring base it wraps to the ring
//    end; QWC becomes the distance back to the base, TADR the ring's tag
//    address, the CHCR tag field is 3 unless the saved MADR sat exactly on
//    the base (or one ring past it), and when the sector before the read
//    sector is not within the filled count it becomes the read sector
//    (count + 1);
//  - otherwise, when the rewind crosses into another sector (func_002040A0
//    gives sector indices), QWC runs to the end of the saved sector, TADR
//    points at that sector's tag, the tag field is 3 unless the saved MADR is
//    the write position, and an out-of-range rewound sector becomes the read
//    sector (count + 1).
// The fromIPU channel is restarted when its MADR / QWC were saved
// (func_002040E0); with sectors queued the IPU gets a BCLR with the saved
// bit pointer (IPU_CMD, waiting on IPU_CTRL busy) and the toIPU channel is
// restarted (func_00204140). IPU_CTRL is restored, +0x44 = 1. Returns 1.
typedef struct IpuRing {
    unsigned int base;      /* 0x00 */
    int tagaddr;            /* 0x04 */
    int sectors;            /* 0x08 */
    int read_sector;        /* 0x0C */
    int count;              /* 0x10 */
    int byte_ofs;           /* 0x14 */
    int size;               /* 0x18 */
    unsigned int to_madr;   /* 0x1C */
    int to_tadr;            /* 0x20 */
    int to_qwc;             /* 0x24 */
    int to_chcr;            /* 0x28 */
    int from_madr;          /* 0x2C */
    int from_qwc;           /* 0x30 */
    int from_chcr;          /* 0x34 */
    unsigned int ipu_bp;    /* 0x38 */
    int ipu_ctrl;           /* 0x3C */
    int sema;               /* 0x40 */
    int busy;               /* 0x44 */
} IpuRing;

extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern int func_002040A0(IpuRing *r, unsigned int addr);
extern void func_002040E0(int chcr);
extern void func_00204140(int chcr);

int func_002047D0(IpuRing *r) {
    unsigned int bp = r->ipu_bp;
    unsigned int madr = r->to_madr;
    int tadr = r->to_tadr;
    int fifo = ((bp >> 16) & 3) + ((bp >> 8) & 0xF);
    unsigned int qwc = r->to_qwc + fifo;
    int chcr;
    int same;
    int k;
    int a;
    int b;

    madr -= fifo * 16;
    chcr = r->to_chcr | 0x100;
    WaitSema(r->sema);
    if (madr < r->base) {
        qwc = (r->base - madr) >> 4;
        same = 1;
        madr += r->sectors << 11;
        tadr = (unsigned int)(r->tagaddr << 4) >> 4;
        if (r->to_madr != r->base && r->to_madr != r->base + (r->sectors << 11)) {
            same = 0;
        }
        chcr = (unsigned int)(r->to_chcr << 4) >> 4 | (same ? 0 : 3) << 28 | 0x100;
        k = (r->sectors - r->read_sector) % r->sectors;
        if (k < 0 || k >= r->count) {
            r->read_sector = r->sectors - 1;
            r->count++;
        }
    } else {
        a = func_002040A0(r, r->to_madr);
        b = func_002040A0(r, madr);
        if (a != b) {
            qwc = (r->base + (a << 11) - madr) >> 4;
            k = (b + r->sectors - r->read_sector) % r->sectors;
            chcr = (unsigned int)(r->to_chcr << 4) >> 4
                 | ((r->base + (r->to_madr - r->base) % (unsigned int)(r->sectors << 11))
                    != (r->base + (((r->read_sector + r->count) % r->sectors) << 11)) ? 3 : 0) << 28
                 | 0x100;
            tadr = (unsigned int)((r->tagaddr + a * 16) << 4) >> 4;
            if (k < 0 || k >= r->count) {
                r->read_sector = b;
                r->count++;
            }
        }
    }
    if (r->from_madr != 0 && r->from_qwc != 0) {
        *(volatile int *)0x1000B010 = r->from_madr;
        *(volatile int *)0x1000B020 = r->from_qwc;
        func_002040E0(r->from_chcr | 0x100);
    }
    if (r->count != 0) {
        while (*(volatile int *)0x10002010 < 0) {
        }
        *(volatile int *)0x10002000 = bp & 0x7F;
        while (*(volatile int *)0x10002010 < 0) {
        }
    }
    *(volatile unsigned int *)0x1000B410 = madr;
    *(volatile int *)0x1000B430 = tadr;
    *(volatile unsigned int *)0x1000B420 = qwc;
    if (r->count != 0) {
        func_00204140(chcr);
    }
    *(volatile int *)0x10002010 = r->ipu_ctrl;
    r->busy = 1;
    SignalSema(r->sema);
    return 1;
}
