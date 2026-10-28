// NEARMISS func_0010B160  (vram 0x0010B160, 0x294 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.89% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff needs instructions that plain C cannot express:
// SDK IPU-init code (ee-gcc, was mislabelled mwcc). The original inlines the privileged
// interrupt-disable / pipeline-sync / Status-register read / interrupt-enable instructions
// (SDK DI/EI-style macros); C-only, they are extern calls here, which add a stack frame and
// shift the registers of the first block. Everything from the IPU reset onward lines up
// apart from small scheduling differences that follow from the frame. Only inline asm closes it
// (not allowed).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;

/* 16-byte aligned qword, moved with 128-bit loads/stores. */
typedef unsigned int qword __attribute__((mode(TI)));

#define IPU_CMD    (*(volatile u32 *)0x10002000)
#define IPU_CTRL   (*(volatile u32 *)0x10002010)
#define IPU_IN_FIFO (*(volatile qword *)0x10007010)
#define D4_CHCR    (*(volatile u32 *)0x1000B400)
#define D_ENABLER  (*(volatile u32 *)0x1000F520)
#define D_ENABLEW  (*(volatile u32 *)0x1000F590)

#define IPU_BUSY(x)  (((int)(x)) < 0)      /* bit 31 */

/*
 * COP0 / privileged ops. Not expressible in mwcc C; the original emits them
 * literally. Left as declarations so the body below reads correctly.
 */
extern void cop0_di(void);        /* disable interrupts            */
extern void cop0_ei(void);        /* enable interrupts             */
extern void cop0_sync_p(void);    /* pipeline sync                 */
extern u32  cop0_status(void);    /* read the COP0 Status register */

/* Quantiser-matrix / VQ-CLUT source tables (.data, disc-independent). */
extern qword D_00241BE0[];        /* [0..3] intra Q matrix, [4] flat non-intra */
extern qword D_00241C30[];        /* [0..1] 32-byte VQ CLUT                    */

static inline void ipu_wait_idle(void)
{
    while (IPU_BUSY(IPU_CTRL)) {
        /* five nops of read latency in the original */
    }
}

static inline void ipu_cmd(u32 cmd)
{
    IPU_CMD = cmd;
    ipu_wait_idle();
}

void func_0010B160(void)
{
    u32 hold;

    /* --- 1. stop the toIPU DMA channel inside a verified critical section --- */
    do {
        cop0_di();
        cop0_sync_p();
    } while (cop0_status() & 0x00010000);   /* spin until EIE really cleared */

    hold      = D_ENABLER;
    D_ENABLEW = hold | 0x10000;             /* suspend DMA */
    D4_CHCR   = 1;                          /* ch4 (toIPU): STR = 0 */
    D_ENABLEW = D_ENABLER & 0xFFFEFFFF;     /* resume DMA */
    cop0_ei();

    /* --- 2. reset and program the decoder --- */
    IPU_CTRL = 0x40000000;                  /* RST */
    ipu_wait_idle();
    ipu_cmd(0x00000000);                    /* BCLR */

    IPU_IN_FIFO = D_00241BE0[0];            /* intra quantiser matrix, 64 B */
    IPU_IN_FIFO = D_00241BE0[1];
    IPU_IN_FIFO = D_00241BE0[2];
    IPU_IN_FIFO = D_00241BE0[3];
    IPU_IN_FIFO = D_00241BE0[4];            /* non-intra matrix: same qword x4 */
    IPU_IN_FIFO = D_00241BE0[4];
    IPU_IN_FIFO = D_00241BE0[4];
    IPU_IN_FIFO = D_00241BE0[4];
    ipu_cmd(0x50000000);                    /* SETIQ intra     */
    ipu_cmd(0x58000000);                    /* SETIQ non-intra */

    IPU_IN_FIFO = D_00241C30[0];            /* VQ CLUT, 32 B */
    IPU_IN_FIFO = D_00241C30[1];
    ipu_cmd(0x60000000);                    /* SETVQ */

    ipu_cmd(0x90000000);                    /* SETTH, thresholds 0 */

    /* --- 3. leave the IPU idle with an empty bit-stream buffer --- */
    IPU_CTRL = 0x40000000;                  /* RST */
    ipu_wait_idle();
    ipu_cmd(0x00000000);                    /* BCLR */
}
