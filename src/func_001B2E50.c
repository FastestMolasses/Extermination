// NEARMISS func_001B2E50  (vram 0x001B2E50, 0x120 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Compiler-version guard-elision + register-allocation permutation. Target (mwcc 2.3.1.01) omits the `slt at,zero,count; beqz` loop-entry guard that both mwcc233 and 991202 emit; this cascades into a full regalloc permutation (count/index/pf land in different registers) plus one tail branch-sense f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 86.39% (mwcc233). Lookup over a sorted threshold table at 0x700030F0
// with count at *0x700031E0 and per-entry flag halfwords at D_70003170/D_7000316E.
// Walks until the actor float (arg0+4) is < the table entry; on the first entry
// that is NOT less, if its 0x70003170 flag bit0 is set it outputs the matching
// 0x700030F0 float to *arg1 and returns 1, else returns 0. If the whole table is
// passed, falls back to entry (count-1): bit0 of D_70003170[last] -> return 0,
// last==0 -> return 0, else bit0 of D_7000316E[last] -> output D_700030EC[last],
// return 1, else 0. Body + the branch-likely loop test (idiom-20, recovered via the
// inverted `if(!(*pf<f))` shape so mwcc emits bc1tl with pf++ in the slot) are
// correct. Residual: mwcc 2.3.1.01 (target) elided the `slt at,zero,count; beqz`
// loop-entry guard that BOTH mwcc233 and 991202 emit, which cascades into a whole
// register-allocation permutation (count->v1 vs a3, idx->a3 vs a2, pf->a2 vs v1)
// and one tail branch-sense flip. Compiler-version guard-elision + regalloc wall.
extern float D_700030EC;
extern float D_700030F0;
extern unsigned short D_7000316E;
extern unsigned short D_70003170;

int func_001B2E50(char *arg0, float *arg1) {
    int count;
    float *pf;
    int i;
    int last;

    count = *(int *)0x700031E0;
    if (count == 0) {
        return 0;
    }
    pf = &D_700030F0;
    for (i = 0; i < count; i++) {
        if (!(*pf < *(float *)(arg0 + 4))) {
            if ((*(&D_70003170 + i) & 1) != 0) {
                *arg1 = (&D_700030F0)[i];
                return 1;
            }
            return 0;
        }
        pf++;
    }
    last = i - 1;
    if ((*(&D_70003170 + last) & 1) != 0) {
        return 0;
    }
    if (last == 0) {
        return 0;
    }
    if ((*(&D_7000316E + last) & 1) != 0) {
        *arg1 = (&D_700030EC)[last];
        return 1;
    }
    return 0;
}
