// NEARMISS func_0019E640  (vram 0x0019E640, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/regalloc-order residual only: instruction count matches (191==191 mwcc233), but mwcc's -O4 allocator picks a different s2/s3 assignment for the found-hit flag vs winning-slot-index, and the two candidate-record branches land a0/v0 the opposite way. Every reordering tried (branch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (93.6% mwcc233 / 81.7% mwcc): level-world camera/room-window
// picker. Picks a spad candidate record (D_70003190 vs D_700031A0) by
// comparing their +0x4 floats and calls func_0019F1A0(candidate, 0x33) on
// the chosen one. Then scans 6 slots (skipping i==2,3) of a "window" table:
// odd slots read D_70003240[i] as a start index into D_70003228[i][] to get
// the end bound; even slots read it as an end index and derive start = table
// value, end = start+1. Keeps the (start,end) pair with the smallest width
// against a running best (seeded from *(int*)0x7000320C), remembering its
// slot index. If the sentinel short at 0x70003244 is -1, bails out (return
// 1). Otherwise walks the winning slot's record-index range
// (D_70003210[slot][start..end)) through the 64-byte poly-node table at
// *(int*)0x70003208, keeping nodes whose XZ box (+0xC/+0xE/+0x14/+0x16)
// contains the probe rect (0x70003240/42/48/4A) and whose material id
// (+0x1A) is < 0x1E, confirmed by func_0019ED80(D_70003190, node); on a hit,
// stashes the probe floor height into 0x700031A4 and copies D_70003190's
// vec3 at +0x20 into a stack scratch (last-hit-wins over the range). If any
// hit was found, writes the result pointer back to 0x700031D0 and restores
// the scratch vec3 into D_70003190+0x20, returning 0 (found); else returns 1.
//
// Body/control-flow and every constant fully recovered; instruction-count-
// near-exact (191==191 vs mwcc233 target). Residual is register coloring in
// the min-window scan and the poly-node filter chain (s2/s3 naming swaps
// between "found-hit flag" and "winning slot index", and the two candidate-
// record branches keep landing on a0/v0 in the opposite arms) -- every
// reordering tried (branch polarity, local declaration order) shifted the
// score by <1% without clearing it. Genuine mwcc regalloc-order residual;
// the pinned 991202 build additionally re-orders two of the box-edge
// compares (v0/v1 slot vs a1-offset load), which is why it trails mwcc233.
extern void func_0019F1A0(void *p, int a1);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x4 float, +0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern short D_70003240[6];   // per-slot short table (index src)
extern short *D_70003228[6];  // per-slot short-array pointer table
extern short *D_70003210[6];  // per-slot record-index pointer table

int func_0019E640(void) {
    int best;
    short lo;
    short hi;
    int foundHit;
    int bestLo;
    int bestHi;
    int bestIdx;
    int i;
    short w;
    short *rec;
    unsigned char *hit;
    float scratch[3];
    float *src;
    float *dst;
    int k;

    foundHit = 0;

    if (*(float *)0x70003194 > *(float *)0x700031A4) {
        func_0019F1A0(D_70003190, 0x33);
    } else {
        func_0019F1A0(D_700031A0, 0x33);
    }

    best = *(int *)0x7000320C;
    for (i = 0; i < 6; i++) {
        if ((unsigned int)(i - 2) < 2) {
            continue;
        }
        if (i & 1) {
            *(short *)0x70003B86 = D_70003240[i];
            w = D_70003228[i][*(short *)0x70003B86];
            *(short *)0x70003B88 = w;
        } else {
            *(short *)0x70003B88 = D_70003240[i];
            w = D_70003228[i][*(short *)0x70003B88];
            *(short *)0x70003B86 = w;
            *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
        }
        lo = *(short *)0x70003B86;
        hi = *(short *)0x70003B88;
        if ((hi - lo) < best) {
            best = hi - lo;
            bestLo = lo;
            bestHi = hi;
            bestIdx = i;
        }
    }

    if (*(short *)0x70003244 == -1) {
        return 1;
    }

    rec = D_70003210[bestIdx] + bestLo;
    for (; bestLo < bestHi; bestLo++, rec += 1) {
        hit = (unsigned char *)(*(int *)0x70003208) + (*rec << 6);
        if (*(short *)0x70003240 < *(short *)(hit + 0xC)) {
            continue;
        }
        if (*(short *)(hit + 0xE) < *(short *)0x70003242) {
            continue;
        }
        if (*(short *)0x70003248 < *(short *)(hit + 0x14)) {
            continue;
        }
        if (*(short *)(hit + 0x16) < *(short *)0x7000324A) {
            continue;
        }
        *(short *)0x70003B88 = *(unsigned char *)(hit + 0x1A);
        if (*(short *)0x70003B88 >= 0x1E) {
            continue;
        }
        if (func_0019ED80(D_70003190, hit) == 0) {
            continue;
        }
        *(float *)0x700031A4 = *(float *)0x700031B4;
        src = (float *)D_70003190;
        for (k = 0; k < 3; k++) {
            scratch[k] = *(src + 8);
            src += 1;
        }
        foundHit = *(int *)0x700031D0;
    }

    if (foundHit != 0) {
        *(int *)0x700031D0 = foundHit;
        dst = (float *)D_70003190;
        for (k = 0; k < 3; k++) {
            *(dst + 8) = scratch[k];
            dst += 1;
        }
        return 0;
    }
    return 1;
}
