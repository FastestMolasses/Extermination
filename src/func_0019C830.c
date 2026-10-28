// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000320C

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.20% NEARMISS).
// Fixes read from the original instructions (spelled like the byte-matched
// twins func_0019E640 / func_0019DF10):
// - D_70003210 / D_70003228 are tables of short pointers (each entry is
//   loaded, then indexed by the scratchpad short), not flat arrays offset by i*4.
// - The even-slot case keeps the reloaded 0x70003B88 value in a local and
//   stores value + 1 from it; the node-index walk steps one short per node.
// - The box tests are one && chain written node-side first; the kind byte is
//   latched through scratchpad short 0x70003B88 and re-read as an int; the
//   0x51/0x52/0x53 mode gates are one && chain ending in the func_0019ED80 test.
// - func_0019F1A0 takes (void *, int); saved-register order comes from local
//   declaration order (lo, hit flag, candidate, hi, cursor).
// - The initial best width at 0x7000320C is a relocated scratchpad extern
//   (idiom-32); as a literal address mwcc filled the then-arm's branch slot.
//
// Single-candidate window select: calls func_0019F1A0(candidate, 0x33) on
// D_70003190 or D_700031A0 (A when its +0x4 float is
// greater than B's), picks the narrowest index window among slots 0, 1,
// 4, 5, then walks that slot's 64-byte poly nodes; a node whose box overlaps,
// whose kind is below 0x5A and passes the mode gate at 0x7000324E (kind 0x51
// needs mode 0, 0x52 needs mode 2, 0x53 rejects mode -1) is tested with
// func_0019ED80. On a hit 0x700031A4 takes 0x700031B4 and the candidate's
// +0x20 vec3 and the hit word at 0x700031D0 are snapshotted; at the end the
// snapshot is written back. Returns 0 on a hit, 1 otherwise.

extern void func_0019F1A0(void *p, int a1);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];
extern char D_700031A0[];
extern short *D_70003210[6];
extern short *D_70003228[6];
extern short D_70003240[6];
extern int D_7000320C;

int func_0019C830(void) {
    int lo;
    int hitfound;
    int cand;
    int hi;
    short *e;
    int i;
    int best;
    float sp50[3];
    int k;
    int kind;

    hitfound = 0;
    if (*(float *)0x70003194 > *(float *)0x700031A4) {
        func_0019F1A0(D_70003190, 0x33);
    } else {
        func_0019F1A0(D_700031A0, 0x33);
    }

    best = D_7000320C;
    for (i = 0; i < 6; i++) {
        if ((unsigned int)(i - 2) > 1U) {
            if (i & 1) {
                *(short *)0x70003B86 = D_70003240[i];
                *(short *)0x70003B88 = D_70003228[i][*(short *)0x70003B86];
            } else {
                *(short *)0x70003B88 = D_70003240[i];
                {
                    short w = *(short *)0x70003B88;
                    *(short *)0x70003B86 = D_70003228[i][w];
                    *(short *)0x70003B88 = w + 1;
                }
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
                if ((kind != 0x51 || *(short *)0x7000324E == 0) &&
                    (kind != 0x52 || *(short *)0x7000324E == 2) &&
                    (kind != 0x53 || *(short *)0x7000324E != -1) &&
                    func_0019ED80(D_70003190, node) != 0) {
                    *(float *)0x700031A4 = *(float *)0x700031B4;
                    for (k = 0; k < 3; k++) {
                        sp50[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                    }
                    hitfound = *(int *)0x700031D0;
                }
            }
        }
    }

    if (hitfound != 0) {
        *(int *)0x700031D0 = hitfound;
        for (k = 0; k < 3; k++) {
            *(float *)(D_70003190 + k * 4 + 0x20) = sp50[k];
        }
        return 0;
    }
    return 1;
}
