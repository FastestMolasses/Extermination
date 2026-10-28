// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.39% NEARMISS).
// Fixes read from the original instructions:
//  - No compiler-version guard elision: the loop test re-reads the count word at
//    0x700031E0 (as the byte-matched twin func_001B2F70 does) instead of a cached
//    local, so mwcc emits no count > 0 entry guard and the registers line up.
//  - The table entry is read into a local before the compare, so it is loaded
//    ahead of the actor float.
//  - The tail tests `last != 0` and the 0x7000316E flag bit in one condition.
//
// Lookup over a sorted threshold table at 0x700030F0 with count at *0x700031E0
// and per-entry flag halfwords at D_70003170/D_7000316E. Walks until the actor
// float (arg0+4) is < the table entry; on the first entry that is NOT less, if
// its 0x70003170 flag bit0 is set it outputs the matching 0x700030F0 float to
// *arg1 and returns 1, else returns 0. If the whole table is passed, falls back
// to entry (count-1): bit0 of D_70003170[last] -> return 0, last==0 -> return 0,
// else bit0 of D_7000316E[last] -> output D_700030EC[last], return 1, else 0.
extern float D_700030EC;
extern float D_700030F0;
extern unsigned short D_7000316E;
extern unsigned short D_70003170;

int func_001B2E50(char *arg0, float *arg1) {
    float *pf;
    int i;
    int last;
    float v;

    if (*(int *)0x700031E0 == 0) {
        return 0;
    }
    pf = &D_700030F0;
    for (i = 0; i < *(int *)0x700031E0; i++) {
        v = *pf;
        if (!(v < *(float *)(arg0 + 4))) {
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
    if (last != 0 && (*(&D_7000316E + last) & 1) != 0) {
        *arg1 = (&D_700030EC)[last];
        return 1;
    }
    return 0;
}
