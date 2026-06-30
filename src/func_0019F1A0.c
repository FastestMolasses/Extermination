// NEARMISS func_0019F1A0  (vram 0x0019F1A0, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring at 92.268% (mwcc233). Every instruction is in the EXACT target order with zero scheduling/structural deltas — all residual diffs are DIFF_ARG_MISMATCH (register names only). CodeWarrior colors the R5900's extra EE temp/arg registers (t4-t7 / a4-a7) and the var_t4=ra m...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int D_70003210[2];
extern short D_70003240[2];

void func_0019F1A0(int arg0, unsigned int arg1) {
    int *pT3;
    short *pT6;
    int ra, t9, cnt, t4, t5, i;

    ra = *(int *)0x70003208;
    pT3 = &D_70003210[0];
    pT6 = &D_70003240[0];
    cnt = *(int *)0x7000320C;
    t4 = ra;
    i = 0;
    t5 = 0;
    t9 = *(int *)0x700031FC;
    do {
        if (arg1 & 1) {
            int base = *pT3;
            int idx = (i >> 1) << 2;
            float key = *(float *)(arg0 + idx);
            int lo = base;
            int hi = base + ((cnt >> 1) << 1);
            int lim = base + (cnt << 1);
            if (base < hi) {
                int t2 = t9 + idx;
                do {
                    int j = *(short *)(t4 + (*(short *)hi << 6)) * 0xC;
                    if (*(float *)(t2 + j) < key) {
                        lo = hi;
                    } else {
                        lim = hi;
                    }
                    hi = lo + (((lim - lo) >> 1) & ~1);
                } while (lo < hi);
            }
            {
                int k = *(short *)(ra + (*(short *)lo << 6) + t5) * 0xC;
                if (!(*(float *)(t9 + k + idx) <= key)) {
                    *pT6 = 0;
                } else {
                    *pT6 = (short)((lo - base) >> 1);
                }
            }
        }
        i += 1;
        pT3 += 1;
        t4 += 2;
        t5 += 2;
        pT6 += 1;
        arg1 >>= 1;
    } while (i < 6);
}
