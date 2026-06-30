// NEARMISS func_00106540  (vram 0x00106540, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.90% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring/list-scheduler wall. Control flow, frame size 0x80, both poll loops, and the entire tail match byte-for-byte. Residual is pure GPR coloring (s1<->s2 swap on the two loop counters and the D_00241170/D_002412F4 base regs) plus a loop-counter increment-scheduling diff (expected keeps a redundant `daddu v...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241324;
extern int D_00241170;
extern int D_00241B98;
extern int D_00241B9C;
extern int D_00241408;
extern void *D_002412F4;
extern void func_00109B70(int chan);

int func_00106540(unsigned int a0) {
    int i = 0;
    int j = 0;
    long q;
    int v1;
    int v0;

    if ((*(volatile int *)0x10002010 & 0x80004000) == 0x80000000) {
        do {
            if (i >= 0x1389) {
                func_00109B70((int)D_002412F4);
                i = 0;
            }
            i++;
        } while ((*(volatile int *)0x10002010 & 0x80004000) == 0x80000000);
    }

    v1 = (a0 << 26) | 0x30000000;
    *(volatile int *)0x10002000 = v1;
    (&D_00241324)[0] = *(int *)((char *)&D_00241170 + ((v1 >> 28) << 2));

    q = *(volatile long *)0x10002000;
    if (q < 0) {
        do {
            if (j >= 0x1389) {
                func_00109B70((int)D_002412F4);
                j = 0;
            }
            j++;
            q = *(volatile long *)0x10002000;
        } while (q < 0);
    }

    {
        long t = *(volatile long *)0x10002030;
        v0 = *(volatile int *)0x10002020;
        D_00241B98 = (int)t;
        if (t < 0)
            D_00241B9C = (-(v0 & 0x1F)) & 0x1F;
        else
            D_00241B9C = 0x20;
    }
    D_00241408 = ((int)q == 0);
    return (short)(int)q;
}
