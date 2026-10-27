// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.25% NEARMISS).
// Fixes read from the original instructions:
//  - func_00106490 takes no arguments (its own definition in src/ is
//    `long func_00106490(void)`). Declaring it with a pointer parameter made
//    ee-gcc treat &D_00241170 as an outgoing argument, which forced $a0 for it
//    and pushed the IPU command constant into a different register. The
//    "GPR-coloring wall" was this wrong prototype.
//
// IPU bit read: if D_00241324 (the last command's table entry) is nonzero or fewer than n bits
// remain, busy-wait while the IPU is busy (calling func_00109B70 every 5001
// polls), issue IPU command 4 (0x40000000) to IPU_CMD, record the command's
// table entry (D_00241170[cmd >> 28] = D_00241170[4], as func_001063B8 does),
// read the next 32-bit word via func_00106490 and reset the bit count to 32.
// Returns the top n bits of the buffered word.

extern int func_00109B70(int);
extern long func_00106490(void);
extern int D_002412F4;
extern int D_00241324;
extern int D_00241170[];
extern int D_00241B98;
extern int D_00241B9C;

int func_001066F8(int n) {
    int count;

    if (D_00241324 != 0 || D_00241B9C < n) {
        count = 0;
        while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
            if (count++ >= 0x1389) {
                func_00109B70(D_002412F4);
                count = 0;
            }
        }
        *(volatile unsigned int *)0x10002000 = 0x40000000;
        D_00241324 = D_00241170[4];
        D_00241B98 = (int)func_00106490();
        D_00241B9C = 32;
    }
    return (unsigned int)D_00241B98 >> (32 - n);
}
