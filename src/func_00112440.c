// NEARMISS func_00112440  (vram 0x00112440, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.22% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc list-scheduler + delay-slot-fill wall. Body, control flow, register allocation and the switch decision tree all match exactly; the only residual is the two `jal CreateSema` (SignalSema) delay slots, which the original fills with `lw a0, %lo(D_00241D08)(v0)` and ours leaves as `nop` (.text ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// libcdvd: sceCdRead(lsn, sectors, buf, mode) — fills the RPC packet at
// D_00279F40 and fires an async SIF RPC (fno 1) to the cdvd server.

typedef struct {
    unsigned char trycount;
    unsigned char spindlctrl;
    unsigned char datapattern;
    unsigned char pad;
} CdRMode;

typedef struct {
    unsigned int lsn;      /* 0x00 */
    unsigned int sectors;  /* 0x04 */
    void *buf;             /* 0x08 */
    unsigned char trycount;    /* 0x0C */
    unsigned char spindlctrl;  /* 0x0D */
    unsigned char datapattern; /* 0x0E */
    unsigned char pad;         /* 0x0F */
    void *rbuf;            /* 0x10 */
    void *stat;            /* 0x14 */
} CdReadPkt;

extern CdReadPkt D_00279F40;
extern unsigned char D_00279E80[0x90];
extern int D_0027A2C0;
extern unsigned char D_00279E00[];

extern int D_00241CF8;
extern volatile int D_00241D48;
extern volatile int D_00241D14;
extern volatile int D_00241D08;
extern char D_0026BC88[];
extern char D_0026BC78[];

extern int func_00112088(void);
extern int func_00111F18(int cmd);
extern void func_0010DFD8(void *addr, int size);
extern void func_00111818(void *arg);
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void (*end_func)(void *),
                         void *end_para);
extern void func_00122B58(char *fmt);
extern void CreateSema(int semid);

int func_00112440(unsigned int lsn, unsigned int sectors, void *buf, CdRMode *mode)
{
    int size;

    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(4) == 0) {
        return 0;
    }

    D_00279F40.lsn = lsn;
    D_00279F40.sectors = sectors;
    D_00279F40.buf = buf;
    D_00279F40.trycount = mode->trycount;
    D_00279F40.spindlctrl = mode->spindlctrl;
    D_00279F40.datapattern = mode->datapattern;
    D_00279F40.rbuf = D_00279E80;
    D_00279F40.stat = &D_0027A2C0;

    switch (mode->datapattern) {
    case 1:
        size = sectors * 0x918;
        break;
    case 2:
        size = sectors * 0x924;
        break;
    case 0:
    default:
        size = sectors * 0x800;
        break;
    }

    D_0027A2C0 = 0;
    func_0010DFD8(buf, size);
    func_0010DFD8(D_00279E80, 0x90);
    func_0010DFD8(&D_00279F40, 0x18);
    func_0010DFD8(&D_0027A2C0, 4);

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC88);
    }

    D_00241D48 = 1;
    D_00241D14 = 1;
    if (func_0010E8A8(D_00279E00, 1, 1, &D_00279F40, 0x18, 0, 0,
                      func_00111818, D_00279E80) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC78);
    }
    CreateSema(D_00241D08);
    return 1;
}
