// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000320C

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.37% NEARMISS).
// Fixes read from the original instructions:
//  - the window-scan loop indexes its three tables by i (D_70003240[i] short,
//    D_70003228[i] a short* table, sp50[i]); the old pointer walk stepped the
//    short and int tables by 2 and 4 elements instead of 1, and the boundary list
//    pointer likewise steps one short per vertex (D_70003210 is a short* table).
//  - the "grid disabled" test reads 0x70003244 as a short, not an int.
//  - candidate windows D_70003190 / D_700031A0 are relocated arrays, the initial
//    best width 0x7000320C a relocated extern (SPAD), and the two staged sp50
//    entries are D_70003240[2..3].
//  - i in [2,4) spelled `i > 1 && i < 4`, no goto; the even outer-arm case keeps the
//    reloaded 0x70003B88 in a local for the index and the +1; the node bound tests
//    on +0xC/+0x10/+0x14 are written node <= query; locals ordered lo, status,
//    best, hi; the snapshot is float[3] above sp50.
//
// func_0019E930 -- s16-grid heightfield/collision world query hub (see docs/FINDINGS.md
// "COLLISION s16-GRID SECTION FULLY DECODED"). Stages a query bbox in scratchpad
// 0x70003240..0x7000324A (queried by 6 sort directions), primes the two candidate windows
// D_70003190 / D_700031A0 via func_0019F1A0(base, dirmask) (order of the two float-boundary
// blocks + the 0x37/8/4 direction masks flips depending on whether 0x70003194 <= 0x700031A4),
// then scans the 6 rank-direction windows (D_70003210[dir] rank table, D_70003228[dir] range
// helper) for the narrowest [lo,hi) window, tracking it in lo/hi/best. If the live query id
// (0x70003244, s16) is -1 there is nothing further to test (grid disabled) -> return 1 (no hit).
// Otherwise walks the boundary-vertex list for [lo,hi) in the winning direction, resolving
// each s16 index into a 64-byte poly node (base D_70003208, node = idx<<6), rejecting nodes
// whose per-direction s16 rank bounds (+0xC/+0xE/+0x10/+0x12/+0x14/+0x16) don't overlap the
// query bbox, then filtering on the node's surface-attr byte (+0x1A) needing to fall in
// [0x1E,0x5A) before calling func_0019ED80 (the front-facing plane/edge convex test) against
// the query segment (D_70003190). On acceptance the float boundary snapshot (D_70003190+0x20,
// 3 floats) is copied into local[] and the hit-record ptr status = 0x700031D0 is kept; after
// the scan, if a hit was recorded the local[] snapshot is written back (re-priming the query
// window for the next set in the caller's chain) and 0 is returned (hit), else 1 (no hit).
extern void func_0019F1A0(char *arg0, int arg1);
extern int func_0019ED80(char *a0, char *a1);

extern char D_70003190[];
extern char D_700031A0[];
extern short *D_70003210[6];
extern short *D_70003228[6];
extern short D_70003240[6];
extern int D_7000320C;

int func_0019E930(void) {
    int sp50[6];
    float local[3];
    int i;
    int lo, status, best, hi;
    int cnt;
    short *s0;
    char *e;
    int a0v;
    int j;

    status = 0;
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        func_0019F1A0(D_70003190, 8);
        func_0019F1A0(D_700031A0, 4);
        sp50[2] = D_70003240[2];
        sp50[3] = D_70003240[3];
        func_0019F1A0(D_70003190, 0x37);
        func_0019F1A0(D_700031A0, 8);
    } else {
        func_0019F1A0(D_70003190, 4);
        func_0019F1A0(D_700031A0, 8);
        sp50[2] = D_70003240[2];
        sp50[3] = D_70003240[3];
        func_0019F1A0(D_70003190, 8);
        func_0019F1A0(D_700031A0, 0x37);
    }

    cnt = D_7000320C;

    for (i = 0; i < 6; i++) {
        if (i > 1 && i < 4) {
            if (i & 1) {
                *(short *)0x70003B86 = D_70003240[i];
                *(short *)0x70003B88 = D_70003228[i][sp50[i]];
            } else {
                *(short *)0x70003B86 = D_70003228[i][sp50[i]];
                *(short *)0x70003B88 = D_70003240[i];
                *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
            }
        } else {
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
        }
        {
            short a88 = *(short *)0x70003B88;
            short a86 = *(short *)0x70003B86;
            int diff = a88 - a86;
            if (diff < cnt) {
                cnt = diff;
                lo = a86;
                hi = a88;
                best = i;
            }
        }
    }

    if (*(short *)0x70003244 == -1) {
        return 1;
    }

    s0 = D_70003210[best] + lo;
    for (; lo < hi; lo++) {
        e = (char *)(*(int *)0x70003208 + (*s0 << 6));
        s0++;
        if (*(short *)(e + 0xC) <= *(short *)0x70003240 &&
            *(short *)(e + 0xE) >= *(short *)0x70003242 &&
            *(short *)(e + 0x10) <= *(short *)0x70003244 &&
            *(short *)(e + 0x12) >= *(short *)0x70003246 &&
            *(short *)(e + 0x14) <= *(short *)0x70003248 &&
            *(short *)(e + 0x16) >= *(short *)0x7000324A) {
            *(short *)0x70003B88 = *(unsigned char *)(e + 0x1A);
            a0v = *(short *)0x70003B88;
            if (a0v >= 0x1E && a0v < 0x5A && func_0019ED80(D_70003190, e) != 0) {
                *(float *)0x700031A4 = *(float *)0x700031B4;
                for (j = 0; j < 3; j++) {
                    local[j] = *(float *)(D_70003190 + j * 4 + 0x20);
                }
                status = *(int *)0x700031D0;
            }
        }
    }

    if (status != 0) {
        *(int *)0x700031D0 = status;
        for (j = 0; j < 3; j++) {
            *(float *)(D_70003190 + j * 4 + 0x20) = local[j];
        }
        return 0;
    }
    return 1;
}
