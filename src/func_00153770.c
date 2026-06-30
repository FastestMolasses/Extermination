// NEARMISS func_00153770  (vram 0x00153770, 0x1DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// scheduling + block-ordering near-miss (87.1% mwcc233): target schedules `subu t34-step` into the branch delay slot so the timer compare emits `slt $at` (mwcc emits `slt v0`); target orders the block_22 reset block before the flag-class else-chain, mwcc reverses. Not the clean-store nop -> permuter.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 87.1% (mwcc 2.3.3; 991202=79.6%). Per-frame state/timer step. Early
// out if arg0+0x36 (flags) == 0. If arg1+0x50 (a delay byte) is nonzero, set
// arg0[0]=1, clear flags, return 0. Otherwise set arg0[0]=2, arg1+0x50=-1, and
// compute the advance `step` = sign-extended low 12 bits of flags, times 5 when
// flags & 0x8000. If flags & 0x2000, accumulate (when arg1+0x51 set) or set
// (else) arg1+0x40 by step, then latch arg1+0x51 = D_00275438. If arg1+0x52 == 0
// and flags & 0x4000, arm arg1+0x52 = 0x3C and signal func_001EFE00(0x80000027,
// self). If step >= arg0+0x34 (the remaining timer): clear it, set arg0[4]=2,
// [5]=0, [6]=0, call func_001B4CF0(self), return 1. Else decrement the timer by
// step; if arg0[5]!=1 and arg1+0x40 >= D_0027543A, or flags & 0x8000, take the
// `block_22` reset (arg0[4]=2,[5]=0,[6]=0; arg1+0x40=0; return 1); otherwise pick
// arg1+0x50 from the flag class (0x5000->0x1E, 0x2000->0x3C, else 0), set
// arg0[0]=1, clear flags, return 0.
//
// Body + logic fully recovered; the residual is CW-vs-mwcc instruction
// scheduling/layout, NOT the clean-store nop: (1) the timer compare emits `slt
// at,...; bnez at` in the target (CW schedules `subu v1,s0` = t34-step into the
// branch delay slot, freeing v0 so the slt lands in $at) where mwcc materializes
// the compare in v0 (`slt v0`); (2) the target lays out the block_22 reset block
// before the flag-class else-chain, mwcc orders them the other way. Both are
// scheduling/basic-block-ordering artifacts -> permuter territory.
extern signed char D_00275438;
extern short D_0027543A;
extern void func_001B4CF0(void *);
extern void func_001EFE00(int, void *);

int func_00153770(unsigned char *arg0, unsigned char *arg1) {
    short flags;
    short t34;
    short step;

    if (*(short *)(arg0 + 0x36) == 0) {
        return 0;
    }
    if (*(signed char *)(arg1 + 0x50) != 0) {
        *(signed char *)(arg0 + 0) = 1;
        *(short *)(arg0 + 0x36) = 0;
        return 0;
    }
    *(signed char *)(arg0 + 0) = 2;
    *(signed char *)(arg1 + 0x50) = -1;
    flags = *(short *)(arg0 + 0x36);
    step = (short)(flags & 0xFFF);
    if (flags & 0x8000) {
        step = (short)(step * 5);
    }
    if (flags & 0x2000) {
        if (*(signed char *)(arg1 + 0x51) != 0) {
            *(short *)(arg1 + 0x40) = (short)(*(short *)(arg1 + 0x40) + step);
        } else {
            *(short *)(arg1 + 0x40) = step;
        }
        *(signed char *)(arg1 + 0x51) = D_00275438;
    }
    if (*(signed char *)(arg1 + 0x52) == 0 && (*(short *)(arg0 + 0x36) & 0x4000)) {
        *(signed char *)(arg1 + 0x52) = 0x3C;
        func_001EFE00(0x80000027, arg0);
    }
    t34 = *(short *)(arg0 + 0x34);
    if (!(step < t34)) {
        *(short *)(arg0 + 0x34) = 0;
        *(signed char *)(arg0 + 4) = 2;
        *(unsigned char *)(arg0 + 5) = 0;
        *(signed char *)(arg0 + 6) = 0;
        func_001B4CF0(arg0);
        return 1;
    }
    *(short *)(arg0 + 0x34) = (short)(t34 - step);
    if (*(unsigned char *)(arg0 + 5) != 1 && *(short *)(arg1 + 0x40) >= D_0027543A) {
        goto block_22;
    }
    flags = *(short *)(arg0 + 0x36);
    if (!(flags & 0x8000)) {
        if (flags & 0x5000) {
            *(signed char *)(arg1 + 0x50) = 0x1E;
        } else if (flags & 0x2000) {
            *(signed char *)(arg1 + 0x50) = 0x3C;
        } else {
            *(signed char *)(arg1 + 0x50) = 0;
        }
        *(signed char *)(arg0 + 0) = 1;
        *(short *)(arg0 + 0x36) = 0;
        return 0;
    }
block_22:
    *(signed char *)(arg0 + 4) = 2;
    *(unsigned char *)(arg0 + 5) = 0;
    *(signed char *)(arg0 + 6) = 0;
    *(short *)(arg1 + 0x40) = 0;
    return 1;
}
