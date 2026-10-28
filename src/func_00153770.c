// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.10% NEARMISS).
// Fixes read from the original instructions:
// - The reset tail is one `||` condition ((arg0[5] != 1 && arg1+0x40 >= D_0027543A)
//   || flags & 0x8000) placed before the flag-class chain, not a goto.
// - The flags word is re-read once inside that condition into an int local and
//   reused by the flag-class tests (no second load / sign extension).
// - The timer test is `timer <= step` read directly from arg0+0x34 and the
//   decrement is a compound `-=` on the field (puts the compare result in $at).
//
// Per-frame state/timer step. Early out if arg0+0x36 (flags) == 0. If arg1+0x50
// (a delay byte) is nonzero, set arg0[0]=1, clear flags, return 0. Otherwise set
// arg0[0]=2, arg1+0x50=-1, and compute the advance `step` = sign-extended low 12
// bits of flags, times 5 when flags & 0x8000. If flags & 0x2000, accumulate (when
// arg1+0x51 set) or set (else) arg1+0x40 by step, then latch arg1+0x51 =
// D_00275438. If arg1+0x52 == 0 and flags & 0x4000, arm arg1+0x52 = 0x3C and
// signal func_001EFE00(0x80000027, self). If the remaining timer arg0+0x34 <= step:
// clear it, set arg0[4]=2, [5]=0, [6]=0, call func_001B4CF0(self), return 1. Else
// decrement the timer by step; if (arg0[5]!=1 and arg1+0x40 >= D_0027543A) or
// flags & 0x8000, reset (arg0[4]=2,[5]=0,[6]=0; arg1+0x40=0; return 1); otherwise
// pick arg1+0x50 from the flag class (0x5000->0x1E, 0x2000->0x3C, else 0), set
// arg0[0]=1, clear flags, return 0.
extern signed char D_00275438;
extern short D_0027543A;
extern void func_001B4CF0(void *);
extern void func_001EFE00(int, void *);

int func_00153770(unsigned char *arg0, unsigned char *arg1) {
    short flags;
    short step;
    int f2;

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
    if (*(short *)(arg0 + 0x34) <= step) {
        *(short *)(arg0 + 0x34) = 0;
        *(signed char *)(arg0 + 4) = 2;
        *(unsigned char *)(arg0 + 5) = 0;
        *(signed char *)(arg0 + 6) = 0;
        func_001B4CF0(arg0);
        return 1;
    }
    *(short *)(arg0 + 0x34) -= step;
    if ((*(unsigned char *)(arg0 + 5) != 1 && *(short *)(arg1 + 0x40) >= D_0027543A)
        || ((f2 = *(short *)(arg0 + 0x36)) & 0x8000)) {
        *(signed char *)(arg0 + 4) = 2;
        *(unsigned char *)(arg0 + 5) = 0;
        *(signed char *)(arg0 + 6) = 0;
        *(short *)(arg1 + 0x40) = 0;
        return 1;
    }
    if (f2 & 0x5000) {
        *(signed char *)(arg1 + 0x50) = 0x1E;
    } else if (f2 & 0x2000) {
        *(signed char *)(arg1 + 0x50) = 0x3C;
    } else {
        *(signed char *)(arg1 + 0x50) = 0;
    }
    *(signed char *)(arg0 + 0) = 1;
    *(short *)(arg0 + 0x36) = 0;
    return 0;
}
