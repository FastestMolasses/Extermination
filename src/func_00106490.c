// NEARMISS func_00106490  (vram 0x00106490, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.89% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// gcc scheduling/regalloc: one redundant register copy. The original copies the loop counter (a1) into v0 before each `count < 5001` compare; ee-gcc -O2 drops that copy and compares the counter directly. Body and all other opcodes identical. Tried variants: docs/NEARMISS.md.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00109B70(int);
extern int D_002412F4;

long func_00106490(void) {
    long val;
    int count = 0;

    val = *(volatile long *)0x10002000;
    if (val < 0) {
        if ((*(volatile int *)0x10002010 & 0x4000) == 0) {
            do {
                if (count < 0x1389) {
                    count++;
                } else {
                    func_00109B70(D_002412F4);
                    count = 0;
                }
                val = *(volatile long *)0x10002000;
                if (val >= 0)
                    break;
            } while ((*(volatile int *)0x10002010 & 0x4000) == 0);
        }
    }
    return val;
}
