// NEARMISS func_00102278  (vram 0x00102278, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 54.73% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc multi-wall (o32-vs-eabi regalloc + dead-loop idiom + frame stride). Decompiled semantics correct; the &D_FFFFFF reloc-symbol loop counter and the forward branch-likely (expected bnel / ours beql, same semantics) BOTH reproduce, so NOT the branch-likely wall. Blockers: (1) original has a dead one-iteration inne...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0026B220;
extern int D_FFFFFF;
extern int func_00122B58(void *);

int func_00102278(int *a0, int a1, int a2)
{
    int *s0 = a0;
    int s3 = a1;
    int s4 = a2;
    int s1;

    if (s0[0] & 0x100) {
        s1 = (int)&D_FFFFFF;
        do {
            if (s1 < 0) {
                int v0;
                func_00122B58(&D_0026B220);
                v0 = s0[0];
                if (((unsigned int)v0 >> 8) & 1) {
                    int v1 = 0;
                    do {
                        v0 = v0 & ~0x100;
                    } while (v1);
                    s0[0] = v0;
                }
            }
            s1--;
        } while (s0[0] & 0x100);
    }

    if (s0[4] == -1)
        s0[4] = s3;
    s0[8] = s4;
    s0[0] = ((s0[0] & -0xD) & -0x2) | 0x100;
    return s0[0];
}
