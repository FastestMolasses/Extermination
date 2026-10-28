// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK (libdma/libvif-style helper): snapshot of the VIF1 registers. Reads VIF1_CYCLE
// first, then asks func_0011BA10 for the VIF1 state class; a class of 2 or more
// (stalled / error) returns 0 without touching the record. Otherwise copies the row
// (R0-R3) and column (C0-C3) registers, MASK, CODE, STAT, the 16-bit ITOP, ITOPS,
// BASE, OFST, TOP, TOPS, MARK and NUM, the ERR and MODE bytes and CYCLE's CL/WL
// bytes into the 64-byte record and returns 1. The state class is compared
// unsigned; assembled by the 2.96 gas (the CYCLE read sits in the call's slot).

#define VIF1_REG(off) (*(volatile unsigned int *)(0x10003C00 + (off)))
#define VIF1_STAT  VIF1_REG(0x000)
#define VIF1_ERR   VIF1_REG(0x020)
#define VIF1_MARK  VIF1_REG(0x030)
#define VIF1_CYCLE VIF1_REG(0x040)
#define VIF1_MODE  VIF1_REG(0x050)
#define VIF1_NUM   VIF1_REG(0x060)
#define VIF1_MASK  VIF1_REG(0x070)
#define VIF1_CODE  VIF1_REG(0x080)
#define VIF1_ITOPS VIF1_REG(0x090)
#define VIF1_BASE  VIF1_REG(0x0A0)
#define VIF1_OFST  VIF1_REG(0x0B0)
#define VIF1_TOPS  VIF1_REG(0x0C0)
#define VIF1_ITOP  VIF1_REG(0x0D0)
#define VIF1_TOP   VIF1_REG(0x0E0)
#define VIF1_R0    VIF1_REG(0x100)
#define VIF1_R1    VIF1_REG(0x110)
#define VIF1_R2    VIF1_REG(0x120)
#define VIF1_R3    VIF1_REG(0x130)
#define VIF1_C0    VIF1_REG(0x140)
#define VIF1_C1    VIF1_REG(0x150)
#define VIF1_C2    VIF1_REG(0x160)
#define VIF1_C3    VIF1_REG(0x170)

typedef struct {
    unsigned int r[4];          /* 0x00 row registers R0..R3 */
    unsigned int c[4];          /* 0x10 column registers C0..C3 */
    unsigned int mask;          /* 0x20 */
    unsigned int code;          /* 0x24 */
    unsigned int stat;          /* 0x28 */
    unsigned short itop;        /* 0x2C */
    unsigned short itops;       /* 0x2E */
    unsigned short base;        /* 0x30 */
    unsigned short ofst;        /* 0x32 */
    unsigned short top;         /* 0x34 */
    unsigned short tops;        /* 0x36 */
    unsigned short mark;        /* 0x38 */
    unsigned short num;         /* 0x3A */
    unsigned char err;          /* 0x3C */
    unsigned char cl;           /* 0x3D */
    unsigned char wl;           /* 0x3E */
    unsigned char mode;         /* 0x3F */
} Vif1Regs;

extern int func_0011BA10(void);   /* VIF1 state class from VIF1_STAT */

int func_0011BA60(Vif1Regs *regs)
{
    unsigned int cycle = VIF1_CYCLE;

    if ((unsigned int)func_0011BA10() >= 2)
        return 0;

    regs->r[0] = VIF1_R0;
    regs->r[1] = VIF1_R1;
    regs->r[2] = VIF1_R2;
    regs->r[3] = VIF1_R3;
    regs->c[0] = VIF1_C0;
    regs->c[1] = VIF1_C1;
    regs->c[2] = VIF1_C2;
    regs->c[3] = VIF1_C3;
    regs->mask = VIF1_MASK;
    regs->code = VIF1_CODE;
    regs->stat = VIF1_STAT;
    regs->itop = VIF1_ITOP;
    regs->itops = VIF1_ITOPS;
    regs->base = VIF1_BASE;
    regs->ofst = VIF1_OFST;
    regs->top = VIF1_TOP;
    regs->tops = VIF1_TOPS;
    regs->mark = VIF1_MARK;
    regs->num = VIF1_NUM;
    regs->err = VIF1_ERR;
    regs->cl = cycle;
    regs->wl = cycle >> 8;
    regs->mode = VIF1_MODE;
    return 1;
}
