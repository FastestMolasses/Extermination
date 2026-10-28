// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 50.74% NEARMISS).
// Fixes read from the original instructions:
//  - func_00106490 takes no arguments and returns a 64-bit long (its definition in
//    src/); the result is narrowed to int before the store. The old `int f(int)`
//    extern faked the "saved-register wall".
//  - The bit count (32) is stored before the shifted result is read, and the result
//    is computed before the IPU command write (same spelling as the twins
//    func_001066F8 / func_00106830).
//  - Busy-wait written as the twins' while loop with a post-increment counter test.
//
// IPU bit reader: waits while the IPU is busy (IPU_CTRL 0x10002010 busy bit set, no
// error), calling func_00109B70 every 0x1389 polls. If the per-command flag
// (D_00241324, set from table D_00241170) is non-zero or fewer than n bits are buffered (D_00241B9C), issues a FDEC 0 at
// IPU_CMD 0x10002000 and refills D_00241B98 through func_00106490. Then marks 32 bits
// available, returns the top n bits of D_00241B98, issues FDEC n and refills
// D_00241B98 for the next read.

extern int func_00109B70(int);
extern long func_00106490(void);
extern int D_002412F4;
extern int D_00241324;
extern int D_00241170[];
extern int D_00241B98;
extern int D_00241B9C;

int func_00106948(int n) {
    int count;
    unsigned int v;
    int ret;

    count = 0;
    while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
        if (count++ >= 0x1389) {
            func_00109B70(D_002412F4);
            count = 0;
        }
    }
    if (D_00241324 != 0 || D_00241B9C < n) {
        *(volatile unsigned int *)0x10002000 = 0x40000000;
        D_00241324 = D_00241170[4];
        D_00241B98 = (int)func_00106490();
    }
    D_00241B9C = 32;
    ret = (unsigned int)D_00241B98 >> (32 - n);
    v = n | 0x40000000;
    *(volatile unsigned int *)0x10002000 = v;
    D_00241324 = D_00241170[v >> 28];
    D_00241B98 = (int)func_00106490();
    return ret;
}
