// NEARMISS gs_vram_readback  (vram 0x00100EB8, 0x68C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.89% via ee-gcc 2.9-991111-01 (-O2) (matching wave 2026-10-27, was 88.39%).
// This is the libgraph StoreImage (GS-to-host VRAM readback) routine: the packet is a
// StoreImage-shaped struct whose TRXREG/BITBLTBUF are read through bitfields (int-promoted,
// hence the sign extensions), IMR get/put take and return 64-bit values, each PSM case
// tests `rb == 0` first, the padded product lives in one shared variable `n`, the read
// loops are plain for-loops without guard ifs, and the 0x10005000 FIFO stores are volatile.
// Remaining differences: (1) the PSM switch dispatches through the splat data symbol
// jtbl_0026B130 while ee-gcc emits its own local .rodata table (2-instruction %hi/%lo
// reloc floor, the jr-table class); (2) epilogue: the original puts the final
// 0x12001040 = 0 store in the GsPutIMR call delay slot and loads D_00241040 before the
// CSR = 2 store; ours leaves the slot empty (store order variants and non-volatile
// spellings tried, none reproduce it).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef int qword __attribute__((mode(TI)));

typedef struct {
    unsigned long SBP:14, p0:2, SBW:6, p1:2, SPSM:6, p2:2;
    unsigned long DBP:14, p3:2, DBW:6, p4:2, DPSM:6, p5:2;
} GsBitbltbuf;

typedef struct {
    unsigned long RRW:12, p0:20, RRH:12, p1:20;
} GsTrxreg;

typedef struct {
    qword vifcode;
    qword giftag;
    GsBitbltbuf bitbltbuf;
    unsigned long bitbltbufaddr;
    unsigned long trxpos;
    unsigned long trxposaddr;
    GsTrxreg trxreg;
    unsigned long trxregaddr;
} GsStoreImage;

extern int func_00122B58(char *);
extern unsigned long GsPutIMR(unsigned long);
extern unsigned long GsGetIMR(void);

extern char D_0026B050[];
extern char D_0026B088[];
extern char D_0026B0B8[];
extern char D_0026B0F8[];
extern qword D_00241040;

int gs_vram_readback(GsStoreImage *sp, qword *dst)
{
    unsigned char tmp[16];
    unsigned int timer;
    int w, h, psm, ih;
    int bytes, q;
    int nq, rq, rb, pad;
    unsigned long oldimr;
    int i;
    int n;

    pad = 0;
    rb = 0;
    nq = 0;
    rq = 0;
    timer = 0;

    w = sp->trxreg.RRW;
    h = sp->trxreg.RRH;
    psm = sp->bitbltbuf.SPSM;
    ih = 0;

    switch (psm) {
    case 0:
    case 48:
        bytes = w * h * 4;
        rb = bytes & 0xF;
        rq = (bytes >> 4) & 7;
        nq = (bytes >> 4) & 0xFFFFFFF8;
        if (rb == 0) {
            ih = h;
            pad = 0;
        } else {
            ih = (h + 3) & 0x1FFC;
            n = w * ih;
            pad = (n >> 2) - nq - rq - 1;
        }
        break;
    case 1:
    case 49:
        bytes = w * h * 3;
        rb = bytes & 0xF;
        rq = (bytes >> 4) & 7;
        nq = (bytes >> 4) & 0xFFFFFFF8;
        if (rb == 0) {
            ih = h;
            pad = 0;
        } else {
            ih = (h + 15) & 0x1FF0;
            n = w * ih;
            n = ((n * 3) >> 4) - nq;
            pad = n - rq - 1;
        }
        break;
    case 2:
    case 10:
    case 50:
    case 58:
        bytes = w * h * 2;
        rb = bytes & 0xF;
        rq = (bytes >> 4) & 7;
        nq = (bytes >> 4) & 0xFFFFFFF8;
        if (rb == 0) {
            ih = h;
            pad = 0;
        } else {
            ih = (h + 7) & 0xFFFFFFF8;
            n = w * ih;
            pad = (n >> 3) - nq - rq - 1;
        }
        break;
    case 19:
    case 27:
        bytes = w * h;
        rb = bytes & 0xF;
        rq = (bytes >> 4) & 7;
        nq = (bytes >> 4) & 0xFFFFFFF8;
        if (rb == 0) {
            ih = h;
            pad = 0;
        } else {
            ih = (h + 7) & 0xFFFFFFF8;
            n = w * ih;
            pad = (n >> 4) - nq - rq - 1;
        }
        break;
    case 20:
    case 36:
    case 44:
        bytes = (w * h) >> 1;
        rb = bytes & 0xF;
        rq = (bytes >> 4) & 7;
        nq = (bytes >> 4) & 0xFFFFFFF8;
        if (rb == 0) {
            ih = h;
            pad = 0;
        } else {
            ih = (h + 7) & 0xFFFFFFF8;
            n = w * ih;
            pad = (n >> 5) - nq - rq - 1;
        }
        break;
    default:
        break;
    }

    if (rb != 0) {
        *(unsigned long *)((unsigned int)&sp->trxreg | 0x20000000) =
            (unsigned long)sp->trxreg.RRW | ((unsigned long)ih << 32);
    }

    while (*(volatile unsigned int *)0x10009000 & 0x100) {
        if (timer++ > 0x1000000) {
            func_00122B58(D_0026B050);
            return -1;
        }
    }

    oldimr = GsPutIMR(GsGetIMR() | 0x200);

    *(volatile unsigned long *)0x12001000 = 2;
    *(volatile unsigned int *)0x10009020 = 7;
    if (((unsigned int)sp & 0x70000000) == 0x70000000) {
        *(volatile unsigned int *)0x10009010 = ((unsigned int)sp & 0xFFFFFFF) | 0x80000000;
    } else {
        *(volatile unsigned int *)0x10009010 = (unsigned int)sp & 0xFFFFFFF;
    }
    *(volatile unsigned int *)0x10009000 = 0x101;

    while (*(volatile unsigned int *)0x10009000 & 0x100) {
        if (timer++ > 0x1000000) {
            func_00122B58(D_0026B050);
            return -1;
        }
    }

    while ((*(volatile unsigned long *)0x12001000 & 2) == 0) {
        if (timer++ > 0x1000000) {
            func_00122B58(D_0026B088);
            *(volatile qword *)0x10005000 = D_00241040;
            return -1;
        }
    }

    *(volatile unsigned int *)0x10003C00 = 0x800000;
    *(volatile unsigned long *)0x12001040 = 1;

    if (nq != 0) {
        *(volatile unsigned int *)0x10009020 = nq;
        if (((unsigned int)dst & 0x70000000) == 0x70000000) {
            *(volatile unsigned int *)0x10009010 = ((unsigned int)dst & 0xFFFFFFF) | 0x80000000;
        } else {
            *(volatile unsigned int *)0x10009010 = (unsigned int)dst & 0xFFFFFFF;
        }
        *(volatile unsigned int *)0x10009000 = 0x100;
        while (*(volatile unsigned int *)0x10009000 & 0x100) {
            if (timer++ > 0x1000000) {
                func_00122B58(D_0026B0B8);
                *(volatile unsigned long *)0x12001000 = 0x100;
                *(volatile unsigned long *)0x12001040 = 0;
                *(volatile unsigned int *)0x10003000 = 1;
                *(volatile unsigned int *)0x10003C10 = 1;
                return -1;
            }
        }
    }

    for (i = 0; i < rq; i++) {
        while ((*(volatile unsigned int *)0x10003C00 & 0x1F000000) == 0) {
            if (timer++ > 0x1000000) {
                func_00122B58(D_0026B0F8);
                *(volatile unsigned long *)0x12001000 = 0x100;
                *(volatile unsigned long *)0x12001040 = 0;
                *(volatile unsigned int *)0x10003000 = 1;
                *(volatile unsigned int *)0x10003C10 = 1;
                return -1;
            }
        }
        dst[nq + i] = *(volatile qword *)0x10005000;
    }

    if (rb != 0) {
        while ((*(volatile unsigned int *)0x10003C00 & 0x1F000000) == 0) {
            if (timer++ > 0x1000000) {
                func_00122B58(D_0026B0F8);
                *(volatile unsigned long *)0x12001000 = 0x100;
                *(volatile unsigned long *)0x12001040 = 0;
                *(volatile unsigned int *)0x10003000 = 1;
                *(volatile unsigned int *)0x10003C10 = 1;
                return -1;
            }
        }
        *(qword *)tmp = *(volatile qword *)0x10005000;
        for (i = 0; i < rb; i++) {
            ((unsigned char *)&dst[nq + rq])[i] = tmp[i];
        }
    }

    for (i = 0; i < pad; i++) {
        while ((*(volatile unsigned int *)0x10003C00 & 0x1F000000) == 0) {
            if (timer++ > 0x1000000) {
                func_00122B58(D_0026B0F8);
                *(volatile unsigned long *)0x12001000 = 0x100;
                *(volatile unsigned long *)0x12001040 = 0;
                *(volatile unsigned int *)0x10003000 = 1;
                *(volatile unsigned int *)0x10003C10 = 1;
                return -1;
            }
        }
        *(qword *)tmp = *(volatile qword *)0x10005000;
    }

    *(volatile unsigned int *)0x10003C00 = 0;
    *(volatile unsigned long *)0x12001040 = 0;
    GsPutIMR(oldimr);
    *(volatile unsigned long *)0x12001000 = 2;
    *(volatile qword *)0x10005000 = D_00241040;
    return 0;
}
