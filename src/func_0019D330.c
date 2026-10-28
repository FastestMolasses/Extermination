// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.75% NEARMISS).
// Fixes read from the original instructions:
//  - the span loop stores the two scratchpad halves directly in each arm (the
//    even arm increments the stored value in place, no short-typed temporary);
//  - the 0x50..0x59 type filter is a nested test entered only for types >= 0x50;
//  - the cell walk is a for loop over [lo, hi) with its list pointer computed
//    before the first test, the bounds tests as early `continue`s, and the
//    entry address formed as one integer sum;
//  - the hit-point copy loops index the scratchpad block, the restore loop
//    walks two pointers, and the hit flag is written back after the restore;
//  - local declaration order (hit before best_i, the running minimum before
//    the list pointers) restores the original register assignment.
//
// Builds two axis masks by comparing the two scratchpad points at 0x70003190
// and 0x700031A0 on each axis, runs func_0019F1A0 on both points with the masks
// (saving the six shorts at 0x70003240 in between), picks the narrowest of six
// cell-list spans, then tests each entry of that list against the bounds at
// 0x70003240 and a type/ID filter, calling func_0019ED80 on candidates. The last
// hit's point is copied back and its flag stored; returns 1 on a hit, else 0.

extern int func_0019ED80(void *node, void *edata);
extern void func_0019F1A0(void *seg, int mask);

extern float D_70003190;
extern float D_700031A0;
extern short D_70003240;
extern int D_70003228;
extern int D_70003210;

int func_0019D330(void) {
    int lo, hi;         /* best span [lo, hi) of the chosen cell list */
    int hit;
    int best_i;
    int s5, s6;         /* plane-side masks for the two corner points */
    int sp80[6];
    float spA0[3];
    int i;
    int best;           /* smallest span seen so far */
    short *p;
    short *node;
    int *q;
    short a2, a3;
    int span;

    hit = 0;
    if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
        s5 = 1;
        s6 = 2;
    } else {
        s5 = 2;
        s6 = 1;
    }
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        s5 |= 4;
        s6 |= 8;
    } else {
        s5 |= 8;
        s6 |= 4;
    }
    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
        s5 |= 0x10;
        s6 |= 0x20;
    } else {
        s5 |= 0x20;
        s6 |= 0x10;
    }

    func_0019F1A0(&D_70003190, s6);
    func_0019F1A0(&D_700031A0, s5);

    for (i = 0; i < 6; i++) {
        sp80[i] = (&D_70003240)[i];
    }

    func_0019F1A0(&D_70003190, s5);
    func_0019F1A0(&D_700031A0, s6);

    best = *(int *)0x7000320C;
    p = &D_70003240;
    q = &D_70003228;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = *p;
            *(short *)0x70003B88 = *(short *)(*q + sp80[i] * 2);
        } else {
            *(short *)0x70003B86 = *(short *)(*q + sp80[i] * 2);
            *(short *)0x70003B88 = *p;
            *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
        }
        a3 = *(short *)0x70003B88;
        a2 = *(short *)0x70003B86;
        span = a3 - a2;
        if (span < best) {
            best = span;
            lo = a2;
            hi = a3;
            best_i = i;
        }
        p += 1;
        q += 1;
    }

    node = (short *)((&D_70003210)[best_i] + lo * 2);
    for (; lo < hi; lo++) {
        char *e = (char *)(*(int *)0x70003208 + (*node << 6));
        node += 1;
        if (*(short *)(e + 0xC) > *(short *)0x70003240) continue;
        if (*(short *)(e + 0xE) < *(short *)0x70003242) continue;
        if (*(short *)(e + 0x14) > *(short *)0x70003248) continue;
        if (*(short *)(e + 0x16) < *(short *)0x7000324A) continue;
        if (*(short *)(e + 0x10) > *(short *)0x70003244) continue;
        if (*(short *)(e + 0x12) < *(short *)0x70003246) continue;
            {
                short st = *(unsigned char *)(e + 0x1A);
                *(short *)0x70003B88 = st;
                st = *(short *)0x70003B88;
                /* Types 0x50..0x59 are filtered: 0x50 always skipped,
                 * 0x51 only when the ID is 0, 0x52 only when it is 2,
                 * 0x53 unless it is -1; 0x54..0x59 pass; >= 0x5A skipped. */
                if (st >= 0x50) {
                    if (st >= 0x5A) goto next;
                    if (st == 0x50) goto next;
                    if (st == 0x51 && *(short *)0x7000324E != 0) goto next;
                    if (st == 0x52 && *(short *)0x7000324E != 2) goto next;
                    if (st == 0x53 && *(short *)0x7000324E == -1) goto next;
                }
                if (func_0019ED80(&D_70003190, e)) {
                    int k;
                    for (k = 0; k < 3; k++) {
                        (&D_70003190)[4 + k] = (&D_70003190)[8 + k];
                        spA0[k] = (&D_70003190)[8 + k];
                    }
                    hit = *(int *)0x700031D0;
                }
            }
        next:;
    }

    if (hit != 0) {
        float *f = spA0;
        float *d = &D_70003190;
        int k;
        for (k = 0; k < 3; k++) {
            d[8] = *f;
            f++;
            d++;
        }
        *(int *)0x700031D0 = hit;
        return 1;
    }
    return 0;
}
