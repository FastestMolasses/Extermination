// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.77% NEARMISS).
// Fixes read from the original instructions (spelled like the byte-matched
// twin func_0019DF10):
// - D_70003210 / D_70003228 are tables of short pointers (each entry is
//   loaded, then indexed), not flat arrays offset by i*4.
// - The window bounds go through the scratchpad shorts 0x70003B86/0x70003B88
//   (the even-slot branch increments 0x70003B88 in place), not locals.
// - The node-index walk steps one short per node, not two.
// - func_0019F1A0 takes (void *, int) as in its twins; saved-register order
//   comes from local declaration order (cursor, bounds, hit flag, candidate,
//   masks), and the box tests are written node-side first for the
//   upper-bound compares.
//
// Dual-candidate window select over D_70003190/D_700031A0: computes axis masks
// from the two candidates' three float pairs, runs the 6-slot window setup via
// func_0019F1A0, picks the narrowest window among all 6 slots, then walks that
// slot's 64-byte poly nodes; a node whose box overlaps on all three axes and
// whose kind is below 0x5A is tested with func_0019ED80, and on a hit the
// candidate's vec3 is committed and the hit word at 0x700031D0 is stored.
// Returns 1 on a hit.
extern void func_0019F1A0(void *p, int a1);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x0/+0x4/+0x8 floats, +0x10/+0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern short *D_70003210[6];  // per-slot node-index lists
extern short *D_70003228[6];  // per-slot window-bound tables
extern short D_70003240[6];

int func_0019DB50(void) {
    short *e;
    int lo, hi;
    int hitfound;
    int cand;
    int maskB, maskA;
    int sp80[6];
    int i;
    int best;
    float spA0[3];
    int k;

    hitfound = 0;
    if (*(float *)0x70003190 > *(float *)0x700031A0) {
        maskB = 1;
        maskA = 2;
    } else {
        maskB = 2;
        maskA = 1;
    }

    if (*(float *)0x70003194 > *(float *)0x700031A4) {
        maskB |= 4;
        maskA |= 8;
    } else {
        maskB |= 8;
        maskA |= 4;
    }

    if (*(float *)0x70003198 > *(float *)0x700031A8) {
        maskB |= 0x10;
        maskA |= 0x20;
    } else {
        maskB |= 0x20;
        maskA |= 0x10;
    }

    func_0019F1A0(D_70003190, maskA);
    func_0019F1A0(D_700031A0, maskB);

    for (i = 0; i < 6; i++) {
        sp80[i] = D_70003240[i];
    }

    func_0019F1A0(D_70003190, maskB);
    func_0019F1A0(D_700031A0, maskA);

    best = *(int *)0x7000320C;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = D_70003240[i];
            *(short *)0x70003B88 = D_70003228[i][sp80[i]];
        } else {
            *(short *)0x70003B86 = D_70003228[i][sp80[i]];
            *(short *)0x70003B88 = D_70003240[i];
            *(short *)0x70003B88 += 1;
        }
        {
            short a88 = *(short *)0x70003B88;
            short a86 = *(short *)0x70003B86;
            int diff = a88 - a86;
            if (diff < best) {
                best = diff;
                lo = a86;
                hi = a88;
                cand = i;
            }
        }
    }

    e = D_70003210[cand] + lo;
    for (; lo < hi; lo++) {
        char *node = *(char **)0x70003208 + (*e << 6);
        e++;
        if (*(short *)(node + 0xC) <= *(short *)0x70003240 &&
            *(short *)(node + 0xE) >= *(short *)0x70003242 &&
            *(short *)(node + 0x14) <= *(short *)0x70003248 &&
            *(short *)(node + 0x16) >= *(short *)0x7000324A &&
            *(short *)(node + 0x10) <= *(short *)0x70003244 &&
            *(short *)(node + 0x12) >= *(short *)0x70003246 &&
            (short)*(unsigned char *)(node + 0x1A) < 0x5A &&
            func_0019ED80(D_70003190, node) != 0) {
            for (k = 0; k < 3; k++) {
                *(float *)(D_70003190 + k * 4 + 0x10) = *(float *)(D_70003190 + k * 4 + 0x20);
                spA0[k] = *(float *)(D_70003190 + k * 4 + 0x20);
            }
            hitfound = *(int *)0x700031D0;
        }
    }

    if (hitfound != 0) {
        for (k = 0; k < 3; k++) {
            *(float *)(D_70003190 + k * 4 + 0x20) = spA0[k];
        }
        *(int *)0x700031D0 = hitfound;
        return 1;
    }
    return 0;
}
