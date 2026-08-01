// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera / line-of-sight AABB sweep. Builds two complementary 6-bit axis masks from a
// componentwise compare of the two vec3s at D_70003190 / D_700031A0, runs the prepare/swap
// helper func_0019F1A0 with the masks in both orders, snapshotting the six per-slot start
// indices from D_70003240 into a stack array in between. Then picks the slot `cand` whose
// [lo,hi) index span is narrowest, where the span endpoints come from D_70003240[i] and
// D_70003228[i][sp80[i]] with the two sources swapped on odd i and a +1 correction applied
// only on the EVEN-i case. Walks that slot's index list D_70003210[cand][lo..hi); each entry
// selects a 64-byte record from *(char**)0x70003208. A record qualifies when its type byte
// (+0x1A) is 0x78 and its three AABB ranges (+0xC/+0xE, +0x14/+0x16, +0x10/+0x12) overlap the
// query box at 0x70003240..0x7000324A, and func_0019ED80 confirms the hit; on a hit the vec3
// at D_70003190+0x20 is copied down to +0x10 and stashed, and the hit id is latched from
// 0x700031D0. If anything hit, the stashed vec3 is written back to +0x20, the hit id restored
// and 1 returned, else 0.
//
// NOTE (matching): the six AABB tests must be written with the RECORD field as the left
// operand (`node->lo <= query`), and every saved-reg scalar must be declared BEFORE the
// `sp80[6]` stack array — see the decl-order pivot model in the wall notes.

extern void func_0019F1A0(void *p, unsigned int flags);
extern int func_0019ED80(void *p, void *hit);

extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x0/+0x8 float pair, +0x10/+0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern short *D_70003210[6]; // per-slot record-index pointer table
extern short *D_70003228[6]; // per-slot short-array pointer table
extern short D_70003240[6];  // per-slot short table (index src)

int func_0019E280(void) {
    int lo, hi;
    int hitfound;
    int cand;
    int maskA, maskB;
    int i;
    int best;
    short *e;
    int k;
    float spA0[3];
    int sp80[6];

    hitfound = 0;

    if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
        maskA = 1;
        maskB = 2;
    } else {
        maskA = 2;
        maskB = 1;
    }
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        maskA |= 4;
        maskB |= 8;
    } else {
        maskA |= 8;
        maskB |= 4;
    }
    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
        maskA |= 0x10;
        maskB |= 0x20;
    } else {
        maskA |= 0x20;
        maskB |= 0x10;
    }

    func_0019F1A0(D_70003190, maskB);
    func_0019F1A0(D_700031A0, maskA);

    for (i = 0; i < 6; i++) {
        sp80[i] = D_70003240[i];
    }

    func_0019F1A0(D_70003190, maskA);
    func_0019F1A0(D_700031A0, maskB);

    best = *(int *)0x7000320C;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = D_70003240[i];
            *(short *)0x70003B88 = D_70003228[i][sp80[i]];
        } else {
            *(short *)0x70003B86 = D_70003228[i][sp80[i]];
            *(short *)0x70003B88 = D_70003240[i];
            *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
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
    if (lo < hi) {
        do {
            char *node = *(char **)0x70003208 + (*e << 6);
            e += 1;
            if (*(unsigned char *)(node + 0x1A) == 0x78 &&
                *(short *)(node + 0xC) <= *(short *)0x70003240 &&
                *(short *)(node + 0xE) >= *(short *)0x70003242 &&
                *(short *)(node + 0x14) <= *(short *)0x70003248 &&
                *(short *)(node + 0x16) >= *(short *)0x7000324A &&
                *(short *)(node + 0x10) <= *(short *)0x70003244 &&
                *(short *)(node + 0x12) >= *(short *)0x70003246 &&
                func_0019ED80(D_70003190, node) != 0) {
                for (k = 0; k < 3; k++) {
                    *(float *)(D_70003190 + k * 4 + 0x10) = *(float *)(D_70003190 + k * 4 + 0x20);
                    spA0[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                }
                hitfound = *(int *)0x700031D0;
            }
            lo += 1;
        } while (lo < hi);
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
