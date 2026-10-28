// NEARMISS func_00101CD0  (vram 0x00101CD0, 0x1D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.34% via ee-gcc 2.9-991111-01 (-O2). This is the libdma routine sceDmaPutEnv:
// it validates the env bytes (stall source, stall drain, MFIFO drain < 10, release cycle < 7),
// packs D_CTRL from three lookup tables, writes D_PCR, D_SQWC, D_RBOR and D_RBSR, and saves
// the 20-byte env to D_002410E8 (read back by func_00101EA8, sceDmaGetEnv).
// Remaining differences: register choice and scheduling around the four discarded volatile reads
// of D_PCR/D_SQWC/D_RBOR/D_RBSR at entry. Reading them into locals that are later reassigned
// from env (the SDK's read-then-overwrite shape) byte-matches 100% (same 0x1D8 size), but those
// initial values are never read, which the honesty rule forbids without approval.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef struct {
    unsigned char sts;
    unsigned char std;
    unsigned char mfd;
    unsigned char rcycle;
    unsigned short express;
    unsigned short notify;
    unsigned short sqwc;
    unsigned short tqwc;
    void *rbadr;
    unsigned int rbmsk;
} sceDmaEnv;

extern unsigned char D_002410B8[];
extern unsigned char D_002410C8[];
extern unsigned char D_002410D8[];
extern sceDmaEnv D_002410E8;

int func_00101CD0(sceDmaEnv *env)
{
    unsigned int ctrl, pcr, sqwc, rbor, rbsr;

    ctrl = *(volatile unsigned int *)0x1000E000;
    *(volatile unsigned int *)0x1000E020;
    *(volatile unsigned int *)0x1000E030;
    *(volatile unsigned int *)0x1000E050;
    *(volatile unsigned int *)0x1000E040;

    if (env->sts >= 10) return -1;
    if (env->std >= 10) return -2;
    if (env->mfd >= 10) return -3;
    if (env->rcycle >= 7) return -4;

    ctrl = (ctrl & ~0x30) | (D_002410B8[env->sts] << 4);
    ctrl = (ctrl & ~0xC0) | (D_002410C8[env->std] << 6);
    ctrl = (ctrl & ~0x0C) | (D_002410D8[env->mfd] << 2);
    if (env->rcycle) {
        ctrl |= 2;
        ctrl = (ctrl & ~0x300) | ((env->rcycle - 1) << 8);
    } else {
        ctrl &= ~2;
    }
    pcr = (env->express << 16) | env->notify;
    sqwc = (env->tqwc << 16) | env->sqwc;
    rbor = (unsigned int)env->rbadr;
    rbsr = env->rbmsk;
    *(volatile unsigned int *)0x1000E000 = ctrl;
    *(volatile unsigned int *)0x1000E020 = pcr;
    *(volatile unsigned int *)0x1000E030 = sqwc;
    *(volatile unsigned int *)0x1000E050 = rbor;
    *(volatile unsigned int *)0x1000E040 = rbsr;
    D_002410E8 = *env;
    return 0;
}
