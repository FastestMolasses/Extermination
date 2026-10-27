// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.56% NEARMISS).
// Fixes read from the original instructions:
// - The loop counter lives in s0, the "found" flag in s1 and the three
//   induction pointers in s2/s3/s4: mwcc 2.3.3 gives the first-declared local
//   the lowest saved register, so the locals are declared in that order
//   (i, set, flags, angles, values). This was the whole "register-allocation
//   permutation" residual, not a compiler wall.
//
// Walks the scratchpad table of *(int *)0x700031E0 entries (flag halfwords at
// 0x70003170, values at 0x700030F0, angles at 0x00282250). For each flagged
// entry whose func_0011DF78(angle) result is <= 1.0471976 (pi/3), the
// first value is stored to *arg1; a later larger value replaces it, or returns
// 1 at once if it also exceeds the threshold at arg0+4. An unflagged entry
// returns 1 if a value is already set and the threshold is <= its value.
// Returns whether any value was set.

extern float func_0011DF78(float);
extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;

int func_001B30E0(char *arg0, float *arg1) {
    int i;
    int set;
    unsigned short *pf;
    float *pa;
    float *pt;

    if (*(int *)0x700031E0 == 0) {
        return 0;
    }
    pf = &D_70003170;
    pa = &D_00282250;
    pt = &D_700030F0;
    set = 0;
    for (i = 0; i < *(int *)0x700031E0; i++) {
        if (*pf & 1) {
            if (func_0011DF78(*pa) <= 1.0471976f) {
                if (set == 0) {
                    set = 1;
                    *arg1 = *pt;
                } else if (*arg1 < *pt) {
                    if (*(float *)(arg0 + 4) < *pt) {
                        return 1;
                    }
                    *arg1 = *pt;
                }
            }
        } else {
            if (set != 0) {
                if (*(float *)(arg0 + 4) <= *pt) {
                    return 1;
                }
            }
        }
        pf++;
        pa++;
        pt++;
    }
    return set;
}
