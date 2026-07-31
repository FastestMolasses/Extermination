// NEARMISS gs_vram_readback  (vram 0xam_readback, 0x68C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.39% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table (external-table dispatch) wall + eegcc coloring: the PSM switch dispatches through the splat data symbol jtbl_0026B130, but ee-gcc emits its own local .rodata table, so the lui/addiu %hi/%lo pair can never match a symbol reloc (hard 2-instruction floor, the confirmed jr-table class). On ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef int qword __attribute__((mode(TI)));

extern int func_00122B58(char *);
extern int SetCPUTimer();
extern int SetCPUTimerHandler();

extern char D_0026B050[];
extern char D_0026B088[];
extern char D_0026B0B8[];
extern char D_0026B0F8[];
extern qword D_00241040;

int gs_vram_readback(unsigned char *packet, unsigned char *dst)
{
    unsigned char tmp[16];
    unsigned char *p;
    unsigned long trxreg;
    unsigned long bitbltbuf;
    unsigned int timer;
    int w, h, psm, ih;
    int bytes, q;
    int nq, rq, rb, pad;
    int oldtimer;
    int i;

    pad = 0;
    rb = 0;
    nq = 0;
    rq = 0;
    timer = 0;

    trxreg = *(unsigned long *)(packet + 0x40);
    bitbltbuf = *(unsigned long *)(packet + 0x20);
    w = (int)(trxreg & 0xFFF);
    h = (int)((trxreg >> 32) & 0xFFF);
    psm = (int)((bitbltbuf >> 24) & 0x3F);
    ih = 0;

    switch (psm) {
    case 0:
    case 48:
        bytes = w * h * 4;
        q = bytes >> 4;
        rb = bytes & 0xF;
        rq = q & 7;
        nq = q & 0xFFFFFFF8;
        if (rb != 0) {
            ih = (h + 3) & 0x1FFC;
            pad = ((w * ih) >> 2) - nq - rq - 1;
        } else {
            ih = h;
            pad = 0;
        }
        break;
    case 1:
    case 49:
        bytes = w * h * 3;
        q = bytes >> 4;
        rb = bytes & 0xF;
        rq = q & 7;
        nq = q & 0xFFFFFFF8;
        if (rb != 0) {
            ih = (h + 15) & 0x1FF0;
            pad = ((w * ih * 3) >> 4) - nq - rq - 1;
        } else {
            ih = h;
            pad = 0;
        }
        break;
    case 2:
    case 10:
    case 50:
    case 58:
        bytes = w * h * 2;
        q = bytes >> 4;
        rb = bytes & 0xF;
        rq = q & 7;
        nq = q & 0xFFFFFFF8;
        if (rb != 0) {
            ih = (h + 7) & 0xFFFFFFF8;
            pad = ((w * ih) >> 3) - nq - rq - 1;
        } else {
            ih = h;
            pad = 0;
        }
        break;
    case 19:
    case 27:
        bytes = w * h;
        q = bytes >> 4;
        rb = bytes & 0xF;
        rq = q & 7;
        nq = q & 0xFFFFFFF8;
        if (rb != 0) {
            ih = (h + 7) & 0xFFFFFFF8;
            pad = ((w * ih) >> 4) - nq - rq - 1;
        } else {
            ih = h;
            pad = 0;
        }
        break;
    case 20:
    case 36:
    case 44:
        bytes = (w * h) >> 1;
        q = bytes >> 4;
        rb = bytes & 0xF;
        rq = q & 7;
        nq = q & 0xFFFFFFF8;
        if (rb != 0) {
            ih = (h + 7) & 0xFFFFFFF8;
            pad = ((w * ih) >> 5) - nq - rq - 1;
        } else {
            ih = h;
            pad = 0;
        }
        break;
    default:
        break;
    }

    if (rb != 0) {
        *(unsigned long *)((unsigned int)(packet + 0x40) | 0x20000000) =
            ((unsigned long)ih << 32) | (unsigned long)w;
    }

    while (*(volatile unsigned int *)0x10009000 & 0x100) {
        if (timer++ > 0x1000000) {
            func_00122B58(D_0026B050);
            return -1;
        }
    }

    oldtimer = SetCPUTimer(SetCPUTimerHandler() | 0x200);

    *(volatile unsigned long *)0x12001000 = 2;
    *(volatile unsigned int *)0x10009020 = 7;
    if (((unsigned int)packet & 0x70000000) == 0x70000000) {
        *(volatile unsigned int *)0x10009010 = ((unsigned int)packet & 0xFFFFFFF) | 0x80000000;
    } else {
        *(volatile unsigned int *)0x10009010 = (unsigned int)packet & 0xFFFFFFF;
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
            *(qword *)0x10005000 = D_00241040;
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

    if (rq != 0) {
        p = nq * 16 + dst;
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
            *(qword *)p = *(volatile qword *)0x10005000;
            p += 16;
        }
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
            (dst + (nq + rq) * 16)[i] = tmp[i];
        }
    }

    if (pad > 0) {
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
    }

    *(volatile unsigned int *)0x10003C00 = 0;
    *(volatile unsigned long *)0x12001040 = 0;
    SetCPUTimer(oldtimer);
    *(volatile unsigned long *)0x12001000 = 2;
    *(qword *)0x10005000 = D_00241040;
    return 0;
}
