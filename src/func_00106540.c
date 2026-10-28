// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.90% NEARMISS).
// Fixes read from the original instructions:
//  - Both spin counters use the twins' post-increment test (`count++ >= 0x1389`,
//    as in func_00106490 / func_00106830); the second loop has its own counter,
//    initialised at its declaration, and its poll is the while condition. The
//    recorded "s1/s2 coloring + increment-scheduling wall" was these spellings.
//  - func_00109B70 returns int and D_002412F4 is a plain int (its definition and
//    the matched twins); D_00241170 is an int array.
//  - The tail reads 0x10002020 before 0x10002030, and the 64-bit word at
//    0x10002030 is a plain (non-volatile) load, which lets ee-gcc fold the low
//    half of its address into the load offset as the original does.
//
// IPU command issue with wait: busy-wait while the IPU is busy (IPU_CTRL
// 0x10002010 busy bit set, no error), calling func_00109B70(D_002412F4) every
// 5001 polls; write command (arg << 26) | 0x30000000 to IPU_CMD (0x10002000)
// and record its table entry D_00241170[cmd >> 28] in D_00241324; poll IPU_CMD
// until its busy (sign) bit clears, with the same watchdog; latch the low word
// of 0x10002030 in D_00241B98 and set the bit count D_00241B9C from the low 5
// bits of 0x10002020 (32 when 0x10002030 is non-negative); D_00241408 records
// whether the command's result word is zero. Returns its low 16 bits, signed.

extern int func_00109B70(int);
extern int D_002412F4;
extern int D_00241324;
extern int D_00241170[];
extern int D_00241B98;
extern int D_00241B9C;
extern int D_00241408;

int func_00106540(int arg) {
    int count;
    int c2 = 0;
    unsigned int v;
    long q;
    long t;
    int r;

    count = 0;
    while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
        if (count++ >= 0x1389) {
            func_00109B70(D_002412F4);
            count = 0;
        }
    }
    v = (arg << 26) | 0x30000000;
    *(volatile unsigned int *)0x10002000 = v;
    D_00241324 = D_00241170[(int)v >> 28];
    while ((q = *(volatile long *)0x10002000) < 0) {
        if (c2++ >= 0x1389) {
            func_00109B70(D_002412F4);
            c2 = 0;
        }
    }
    r = *(volatile int *)0x10002020;
    t = *(long *)0x10002030;
    D_00241B98 = (int)t;
    if (t < 0)
        D_00241B9C = (-(r & 0x1F)) & 0x1F;
    else
        D_00241B9C = 0x20;
    D_00241408 = ((int)q == 0);
    return (short)q;
}
