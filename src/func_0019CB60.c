// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.49% NEARMISS).
// Fixes read from the original instructions (spelled like the byte-matched
// twins func_0019DF10 / func_0019DB50):
// - The two masks are spelled as in the twins (> compares, maskA/maskB).
// - D_70003210 / D_70003228 are tables of short pointers (each entry is
//   loaded, then indexed); the sp80 widen loop and node walk step one element
//   (one short per node), not two.
// - The window bounds go through scratchpad shorts 0x70003B86/0x70003B88, the
//   kind byte is stored to 0x70003B88 and re-read as a short, and the three
//   kind filters (0x51/0x52/0x53 against 0x7000324E) are sequential skips.
// - The hit path copies only the +0x20 vec3 to the stack (no ok flag),
//   and local declaration order follows the twins; box tests node-side first.
//
// Dual-candidate window select over D_70003190/D_700031A0: computes axis masks
// from the two candidates' float pairs, runs the 6-slot window setup via
// func_0019F1A0, picks the narrowest window among slots 0, 1, 4, 5, then walks
// that slot's 64-byte poly nodes; a node whose box overlaps, whose kind is
// below 0x5A and passes the 0x7000324E mode filter for kinds 0x51..0x53 is
// tested with func_0019ED80. On a hit candidate B's float pair is reloaded
// from +0x10/+0x18 of A, A's +0x20 vec3 is saved, and the hit word at
// 0x700031D0 is latched; at the end the hit word and vec3 are committed.
// Returns 0 on a hit, 1 otherwise.
extern void func_0019F1A0(void *p, int a1);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];
extern char D_700031A0[];
extern short *D_70003210[6];
extern short *D_70003228[6];
extern short D_70003240[6];

int func_0019CB60(void) {
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
    int kind;

    hitfound = 0;
    if (*(float *)0x70003190 > *(float *)0x700031A0) {
        maskB = 1;
        maskA = 2;
    } else {
        maskB = 2;
        maskA = 1;
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
        if ((unsigned int)(i - 2) > 1U) {
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
    }

    e = D_70003210[cand] + lo;
    for (; lo < hi; lo++) {
        char *node = *(char **)0x70003208 + (*e << 6);
        e++;
        if (*(short *)(node + 0xC) <= *(short *)0x70003240 &&
            *(short *)(node + 0xE) >= *(short *)0x70003242 &&
            *(short *)(node + 0x14) <= *(short *)0x70003248 &&
            *(short *)(node + 0x16) >= *(short *)0x7000324A) {
            *(short *)0x70003B88 = *(unsigned char *)(node + 0x1A);
            kind = *(short *)0x70003B88;
            if (kind < 0x5A) {
                if (kind == 0x51 && *(short *)0x7000324E != 0) continue;
                if (kind == 0x52 && *(short *)0x7000324E != 2) continue;
                if (kind == 0x53 && *(short *)0x7000324E == -1) continue;
                if (func_0019ED80(D_70003190, node) != 0) {
                    *(float *)0x700031A0 = *(float *)0x700031B0;
                    *(float *)0x700031A8 = *(float *)0x700031B8;
                    for (k = 0; k < 3; k++) {
                        spA0[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                    }
                    hitfound = *(int *)0x700031D0;
                }
            }
        }
    }

    if (hitfound != 0) {
        *(int *)0x700031D0 = hitfound;
        for (k = 0; k < 3; k++) {
            *(float *)(D_70003190 + k * 4 + 0x20) = spA0[k];
        }
        return 0;
    }
    return 1;
}
