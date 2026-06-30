// NEARMISS func_00102530  (vram 0x00102530, 0xD0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.15% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc regalloc + inner-loop idiom near-miss. Clean -O2 C reaches 73% (permuter plateaued ~79% over 360s, no zero). Body structure is correct: a2==1 fast path returns (a0[4]<a1); else the while(a0[4]<a1) outer loop with i=(a3?a3:0x1000000)-1 countdown, the if(i<0){ func_00122B58(&D_0026B220); if((flags>>8)&1){ inner-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00122B58(int *p);
extern int D_0026B220;

int func_00102530(unsigned int *a0, unsigned int a1, int a2, int a3)
{
    int i;
    unsigned int flags;

    if (a2 == 1)
        return a0[4] < a1;

    if (!(a0[4] < a1))
        return 0;

    if (a3 == 0)
        a3 = 0x1000000;
    i = a3 - 1;

    while (a0[4] < a1) {
        if (i < 0) {
            func_00122B58(&D_0026B220);
            flags = a0[0];
            if ((flags >> 8) & 1) {
                do {
                    flags = flags & ~0x100;
                } while (flags & 0x100);
                a0[0] = flags;
            }
        }
        i--;
    }
    return 0;
}
