// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
//
// SDK VIF0 register snapshot: reads VIF0_CYCLE first, then unless
// func_0011B6E0() >= 2 (VIF0 not idle/stalled; returns 0 without writing) copies
// R0..R3, C0..C3, MASK, CODE, STAT (words), ITOP, ITOPS, MARK, NUM (halfwords),
// ERR, CYCLE.CL, CYCLE.WL and MODE (bytes) into *r and returns 1.

struct vif0_regs {
    unsigned int r0, r1, r2, r3;
    unsigned int c0, c1, c2, c3;
    unsigned int mask;
    unsigned int code;
    unsigned int stat;
    unsigned short itop;
    unsigned short itops;
    unsigned short mark;
    unsigned short num;
    unsigned char err;
    unsigned char cl;
    unsigned char wl;
    unsigned char mode;
};
#define REG(a) (*(volatile unsigned int *)(a))
extern int func_0011B6E0(void);
int func_0011B730(struct vif0_regs *r) {
    unsigned int cycle = REG(0x10003840);
    if ((unsigned int)func_0011B6E0() >= 2) {
        return 0;
    }
    r->r0 = REG(0x10003900);
    r->r1 = REG(0x10003910);
    r->r2 = REG(0x10003920);
    r->r3 = REG(0x10003930);
    r->c0 = REG(0x10003940);
    r->c1 = REG(0x10003950);
    r->c2 = REG(0x10003960);
    r->c3 = REG(0x10003970);
    r->mask = REG(0x10003870);
    r->code = REG(0x10003880);
    r->stat = REG(0x10003800);
    r->itop = REG(0x100038D0);
    r->itops = REG(0x10003890);
    r->mark = REG(0x10003830);
    r->num = REG(0x10003860);
    r->err = REG(0x10003820);
    r->cl = cycle;
    r->wl = cycle >> 8;
    r->mode = REG(0x10003850);
    return 1;
}
