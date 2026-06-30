// NEARMISS func_001063E8  (vram 0x001063E8, 0xA4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.83% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc regalloc/rematerialization wall. Structure (guard + do-while watchdog poll loop on hw reg 0x10002010, slti 0x1389 watchdog calling func_00109B70(D_002412F4)) is byte-correct. Remaining ~6.5% gap: this ee-gcc 2.9-991111-01 rematerializes `lui 0x1000;ori 0x2010` for the loop poll each iteration instead of cachi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00109B70(int);
extern int D_002412F4;

int func_001063E8(void) {
    volatile int *reg = (volatile int *)0x10002010;
    int i = 0;

    if ((*reg & 0x80004000) != 0x80000000) {
        return 0x80000000;
    }
    do {
        if (i >= 0x1389) {
            func_00109B70(D_002412F4);
            i = 0;
        } else {
            i++;
        }
    } while ((*reg & 0x80004000) == 0x80000000);
    return i;
}
