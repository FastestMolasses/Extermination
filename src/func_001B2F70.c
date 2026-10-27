// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.59% NEARMISS).
// Fixes read from the original instructions:
//  - The callee-saved register "permutation" was declaration order: the original
//    gives the loop counter the lowest saved register, then the found flag, then
//    the three table pointers, so the locals are declared i, found, p, q, r.
//  - (Earlier, lane DFIX2:) func_0019BC40 receives arg0, the position.
//
// Scans the active-flag table at D_70003170 (one u16 per slot, count at
// 0x700031E0). For each set slot, if func_0011DF78(angle D_00282250[i]) <= PI/3
// (1.0471976), tracks the largest D_700030F0[i] into *arg1 and returns 1 early
// when *(arg0+4) crosses it; returns the "found" flag otherwise.
extern float func_0011DF78(float);
extern void func_0019BC40(char *pos);
extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;

int func_001B2F70(char *arg0, float *arg1) {
    int i;
    int found;
    unsigned short *p;
    float *q;
    float *r;

    func_0019BC40(arg0);
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
