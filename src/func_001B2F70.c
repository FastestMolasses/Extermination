// NEARMISS func_001B2F70  (vram 0x001B2F70, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation permutation (callee-saved coloring + a0/a1 param-save order); body byte-identical, not the clean-store nop -> permuter, not 2.3.3
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 98.4% (mwcc 2.3.3; pinned 991202 = 89.8%). Body is byte-identical to
// the target; the sole residual is a register-allocation permutation
// (callee-saved coloring + the a0/a1 param-save order at the two leading paddub
// moves). The target colors {a1->s5, a0->s6} and {ptrs->s2/s3/s4, found->s1,
// counter->s0}; mwcc colors {a0->s6, a1->s5} and {ptrs->s0/s1/s2, found->s3,
// counter->s4}. Same instruction stream, different register numbers -> permuter.
//
// Scans the active-flag table at D_70003170 (one u16 per slot, count at
// 0x700031E0). For each set slot, if func_0011DF78(angle D_00282250[i]) <= PI/3
// (1.0471976), tracks the smallest D_700030F0[i] into *arg1 and returns 1 early
// when *(arg0+4) crosses it; returns the "found" flag otherwise.
extern float func_0011DF78(float);
extern void func_0019BC40();
extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;

int func_001B2F70(char *arg0, float *arg1) {
    unsigned short *p;
    float *q;
    float *r;
    int found;
    int i;

    func_0019BC40();
    if (*(int *)0x700031E0 == 0) {
        return 0;
    }
    p = &D_70003170;
    q = &D_00282250;
    r = &D_700030F0;
    found = 0;
    for (i = 0; i < *(int *)0x700031E0; i++) {
        if (*p & 1) {
            if (func_0011DF78(*q) <= 1.0471976f) {
                if (found == 0) {
                    found = 1;
                    *arg1 = *r;
                } else if (*arg1 < *r) {
                    if (*(float *)(arg0 + 4) < *r) {
                        return 1;
                    }
                    *arg1 = *r;
                }
            }
        } else if (found != 0) {
            if (*(float *)(arg0 + 4) <= *r) {
                return 1;
            }
        }
        p += 1;
        q += 1;
        r += 1;
    }
    return found;
}
