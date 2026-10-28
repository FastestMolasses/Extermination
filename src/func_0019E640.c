// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000320C

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.64% NEARMISS).
// Fixes read from the original instructions (spelling follows the byte-matched
// twin func_0019E280):
// - saved-register order: the window bounds lo/hi are declared first, then the
//   hit flag, then the winning slot index (lo/hi/hit/slot = s0/s1/s2/s3).
// - the even-slot case keeps the reloaded start index in a local and stores
//   start + 1 from it instead of reading 0x70003B88 a third time.
// - the slot skip is `i == 2 || i == 3` (mwcc folds it to one unsigned range
//   test in $at), not a hand-written unsigned range compare.
// - the poly-node box test is one && chain with the record field on the left
//   (`node->lo <= query`); the material-id latch and the func_0019ED80 check
//   form a nested if; the index pointer advances right after its read.
// - func_0019F1A0 takes (void *, unsigned int) as in the twin.
// - the initial best width at 0x7000320C is a relocated scratchpad extern
//   (idiom-32); as a literal address mwcc filled the then-arm's branch slot.
//
// Level-world camera/room-window picker. Picks a spad candidate record
// (D_70003190 vs D_700031A0) by comparing their +0x4 floats and calls
// func_0019F1A0(candidate, 0x33) on the chosen one. Then scans 6 slots
// (skipping i==2,3) of a "window" table: odd slots read D_70003240[i] as a start
// index into D_70003228[i][] to get the end bound; even slots read it as an end
// index and derive start = table value, end = start+1. Keeps the (start,end)
// pair with the smallest width against a running best (seeded from 0x7000320C),
// remembering its slot index. If the sentinel short at 0x70003244 is -1, bails
// out (return 1). Otherwise walks the winning slot's record-index range
// (D_70003210[slot][start..end)) through the 64-byte poly-node table at
// *(int*)0x70003208, keeping nodes whose XZ box (+0xC/+0xE/+0x14/+0x16)
// contains the probe rect (0x70003240/42/48/4A) and whose material id (+0x1A)
// is < 0x1E, confirmed by func_0019ED80(D_70003190, node); on a hit, stashes the
// probe floor height into 0x700031A4 and copies D_70003190's vec3 at +0x20 into
// a stack scratch (last-hit-wins over the range). If any hit was found, writes
// the result pointer back to 0x700031D0 and restores the scratch vec3 into
// D_70003190+0x20, returning 0 (found); else returns 1.
extern void func_0019F1A0(void *p, unsigned int flags);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x4 float, +0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern short D_70003240[6];  // per-slot short table (index src)
extern short *D_70003228[6]; // per-slot short-array pointer table
extern short *D_70003210[6]; // per-slot record-index pointer table
extern int D_7000320C;       // PS2 scratchpad @ 0x7000320C: initial best window width

int func_0019E640(void) {
    int lo, hi;
    int hitfound;
    int cand;
    int i;
    int best;
    short *e;
    int k;
    float spA0[3];

    hitfound = 0;

    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        func_0019F1A0(D_70003190, 0x33);
    } else {
        func_0019F1A0(D_700031A0, 0x33);
    }

    best = D_7000320C;
    for (i = 0; i < 6; i++) {
        if (i == 2 || i == 3) {
            continue;
        }
        if (i & 1) {
            *(short *)0x70003B86 = D_70003240[i];
            *(short *)0x70003B88 = D_70003228[i][*(short *)0x70003B86];
        } else {
            *(short *)0x70003B88 = D_70003240[i];
            {
                short t = *(short *)0x70003B88;
                *(short *)0x70003B86 = D_70003228[i][t];
                *(short *)0x70003B88 = t + 1;
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

    if (*(short *)0x70003244 == -1) {
        return 1;
    }

    e = D_70003210[cand] + lo;
    for (; lo < hi; lo++) {
        char *node = *(char **)0x70003208 + (*e << 6);
        e += 1;
        if (*(short *)(node + 0xC) <= *(short *)0x70003240 &&
            *(short *)(node + 0xE) >= *(short *)0x70003242 &&
            *(short *)(node + 0x14) <= *(short *)0x70003248 &&
            *(short *)(node + 0x16) >= *(short *)0x7000324A) {
            *(short *)0x70003B88 = *(unsigned char *)(node + 0x1A);
            if (*(short *)0x70003B88 < 0x1E && func_0019ED80(D_70003190, node) != 0) {
                *(float *)0x700031A4 = *(float *)0x700031B4;
                for (k = 0; k < 3; k++) {
                    spA0[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                }
                hitfound = *(int *)0x700031D0;
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
