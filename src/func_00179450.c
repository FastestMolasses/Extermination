// NEARMISS func_00179450  (vram 0x00179450, 0x104 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register coloring: target keeps `thr` (the +0xB4 field) in $f1 (fv0f) but mwcc 2.3.3 colors it into $f2 (fv1). This single FP-coloring difference cascades into the inner c.lt.s operand register, the inner branch sense (bc1t vs bc1f), and the v0=1/v0=2 emit order. Body and all control flow are ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.0% (mwcc 2.3.3; 75.1% on pinned 991202). Body/logic byte-correct;
// sole residual is FP register coloring ($f1 vs $f2 for `thr`) which cascades to
// the inner branch sense and the 1/2 return-value order. Permuter territory.
//
// First runs func_0019BC40(arg1). If the table count at *0x700031E0 is nonzero,
// scans the parallel global tables from index (count-1) down to 0: the u16 flag
// table D_70003170[] and the float table D_700030F0[]. For each entry whose flag
// bit 0 is set, if D_700030F0[i] < arg0[0xB4] it commits: writes the delta
// (D_700030F0[i] - arg0[0xB4]) to *(float*)(arg0+0x258), then returns 1 if the
// parallel angle/value table D_00282250[i] < 0.62831855 (2*pi/10) else 2. If no
// entry qualifies, returns 0.
extern void func_0019BC40(int);
extern float D_700030F0;
extern unsigned short D_70003170;
extern float D_00282250;

int func_00179450(char *arg0, int arg1) {
    int i;
    unsigned short *flags;
    float *vals;
    float thr;

    func_0019BC40(arg1);
    if (*(int *)0x700031E0 != 0) {
        i = *(int *)0x700031E0 - 1;
        if (i >= 0) {
            flags = &D_70003170 + i;
            vals = &D_700030F0 + i;
            do {
                if (*flags & 1) {
                    thr = *(float *)(arg0 + 0xB4);
                    if (*vals < thr) {
                        *(float *)(arg0 + 0x258) = (&D_700030F0)[i] - thr;
                        if (!((&D_00282250)[i] < 0.62831855f)) {
                            return 2;
                        }
                        return 1;
                    }
                }
                i--;
                flags--;
                vals--;
            } while (i >= 0);
        }
    }
    return 0;
}
