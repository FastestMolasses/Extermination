// NEARMISS func_00112150  (vram 0x00112150, 0x2F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.51% via ee-gcc 2.9-991111-01 (-O2, 2.96 gas). Logic, structure, instruction
// selection and scheduling all match. Remaining differences: one saved-register swap only —
// the original gives the mode pointer and the per-sector size (they share one register) $s2
// and the rounded entry count $s3; this C gets them the other way round (global-alloc
// priority). Matching wave 2026-10-27 fixes (was 88.85%): packet as a struct global, the
// RPC/semaphore globals volatile as in the byte-matched siblings, callee prototypes from
// those siblings, and the copy loop written as a for loop whose condition tests the
// 0xC0 limit before the terminator (gcc's loop rotation then matches).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// libcdvd: sceCdReadChain-style request (RPC fno 0xF): copies a -1-terminated list of
// (lsn, sectors, buf) triples into the packet at D_00279F80, rounds it to whole triples,
// flushes each destination buffer (size = sectors * the mode's sector size) and the
// packet, then fires the async SIF RPC.
//
// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

typedef struct {
    unsigned int list[0xC3];      /* 0x000 */
    unsigned char trycount;       /* 0x30C */
    unsigned char spindlctrl;     /* 0x30D */
    unsigned char datapattern;    /* 0x30E */
    unsigned char pad;            /* 0x30F */
    int *stat;                    /* 0x310 */
} CdRPkt;

typedef struct {
    unsigned char trycount;
    unsigned char spindlctrl;
    unsigned char datapattern;
    unsigned char pad;
} CdRMode_t;

extern int D_00241CF8;
extern volatile int D_00241D08;
extern volatile int D_00241D14;
extern volatile int D_00241D48;
extern unsigned char D_00279E00[];
extern CdRPkt D_00279F80;
extern int D_0027A2C0;
extern char D_0026BC00[];
extern char D_0026BC18[];
extern char D_0026BC30[];
extern char D_0026BC60[];
extern char D_0026BC78[];

extern int func_00112088(void);
extern int func_00111F18(int cmd);
extern void func_00122B58(char *fmt, ...);
extern void func_0010DFD8(void *addr, int size);
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void (*end_func)(void *),
                         void *end_para);
extern void SignalSema(int semid);
extern void func_001115D0(void *arg);

int func_00112150(unsigned int *list, CdRMode_t *mode)
{
    int i;
    int n;
    int blk;

    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(3) == 0) {
        return 0;
    }

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC00);
    }

    for (i = 0; i < 0xC0 && *list != 0xFFFFFFFFu; i++, list++) {
        D_00279F80.list[i] = *list;
    }

    n = (i / 3) * 3;
    D_00279F80.list[n] = 0xFFFFFFFFu;
    D_00279F80.list[n + 1] = 0xFFFFFFFFu;
    D_00279F80.list[n + 2] = 0xFFFFFFFFu;

    D_00279F80.trycount = mode->trycount;
    D_00279F80.spindlctrl = mode->spindlctrl;
    D_00279F80.datapattern = mode->datapattern;
    D_00279F80.stat = &D_0027A2C0;

    switch (mode->datapattern) {
    case 1:
        blk = 0x918;
        break;
    case 2:
        blk = 0x924;
        break;
    case 0:
    default:
        blk = 0x800;
        break;
    }

    D_0027A2C0 = 0;
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC18);
    }

    for (i = 1; i < n; i += 3) {
        if ((D_00279F80.list[i + 1] & 1) == 0) {
            if (D_00241CF8 > 0) {
                func_00122B58(D_0026BC30, D_00279F80.list[i + 1], D_00279F80.list[i] * blk);
            }
            func_0010DFD8((void *)D_00279F80.list[i + 1], D_00279F80.list[i] * blk);
        }
    }

    func_0010DFD8(&D_00279F80, 0x18);
    func_0010DFD8(&D_0027A2C0, 4);

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC60);
    }

    D_00241D48 = 1;
    D_00241D14 = 1;
    if (func_0010E8A8(D_00279E00, 0xF, 1, &D_00279F80, 0x314, 0, 0,
                      func_001115D0, &D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        SignalSema(D_00241D08);
        return 0;
    }

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC78);
    }
    SignalSema(D_00241D08);
    return 1;
}
