// NEARMISS func_00106830  (vram 0x00106830, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 60.94% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-hoist / frame-stride wall. Logic recovered but our ee-gcc 2.9-991111-01 allocates MORE callee-saved registers (frame 0xa0, saves s0-s8) than the original (frame 0x60, saves s0-s4) because it hoists the loop-invariant volatile-poll address constants (0x10002010, 0x80004000) into callee-saved regs, wher...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241170[16];
extern int D_002412F4;
extern int D_00241324;
extern int D_00241B98;
extern int D_00241B9C;
extern int func_00106490(int);
extern void func_00109B70(int);

void func_00106830(int arg)
{
    int v;
    int count;

    if ((*(volatile int *)0x10002010 & 0x80004000) == 0x80000000) {
        count = 0;
        do {
            if (count >= 0x1389) {
                func_00109B70(D_002412F4);
                count = 0;
            }
            count++;
        } while ((*(volatile int *)0x10002010 & 0x80004000) == 0x80000000);
    }

    v = arg | 0x40000000;
    *(volatile int *)0x10002000 = v;
    D_00241324 = D_00241170[(unsigned int)v >> 28];
    func_00106490(D_00241324);
    D_00241B98 = (int)((long)v << 32 >> 32);
    D_00241B9C = 0x20;
}
