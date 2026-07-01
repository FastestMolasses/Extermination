// NEARMISS func_0019E930  (vram 0x0019E930, 0x444 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring (a2/a1/a0/v0/v1 consistently swapped vs target) plus a stack-slot/scheduling permutation in the two local[4] copy loops (target keeps local[] at sp+0x70; mwcc233 places it at sp+0x50 for this source shape). All branch shapes, load widths (verified s16 vs s32 on the 0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// func_0019D330 -- s16-grid heightfield/collision world query hub (see docs/FINDINGS.md
// "COLLISION s16-GRID SECTION FULLY DECODED"). Stages a query bbox in scratchpad
// 0x70003240..0x7000324A (queried by 6 sort directions), primes the two candidate windows
// D_70003190 / D_700031A0 via func_0019F1A0(base, dirmask) (order of the two float-boundary
// blocks + the 0x37/8/4 direction masks flips depending on whether 0x70003194 <= 0x700031A4),
// then scans the 6 rank-direction windows (D_70003210[dir] rank table, D_70003228[dir] range
// helper) for the narrowest [lo,hi) window, tracking it in lo/hi/best. If the live query id
// (0x70003244) is -1 there is nothing further to test (grid disabled) -> return 1 (no hit).
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

extern int D_70003210[6];
extern int D_70003228[6];
extern short D_70003240[6];

int func_0019E930(void) {
    float local[4];
    int sp50[6];
    short *pT6;
    int *pT3;
    int *pS0;
    int i;
    int lo, hi, best;
    int cnt;
    int status;
    short *s0;
    char *e;
    int a0v;
    int j;

    status = 0;
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        func_0019F1A0((char *)0x70003190, 8);
        func_0019F1A0((char *)0x700031A0, 4);
        {
            short t0 = *(short *)0x70003244;
            short t1 = *(short *)0x70003246;
            sp50[2] = t0;
            sp50[3] = t1;
        }
        func_0019F1A0((char *)0x70003190, 0x37);
        func_0019F1A0((char *)0x700031A0, 8);
    } else {
        func_0019F1A0((char *)0x70003190, 4);
        func_0019F1A0((char *)0x700031A0, 8);
        {
            short t0 = *(short *)0x70003244;
            short t1 = *(short *)0x70003246;
            sp50[2] = t0;
            sp50[3] = t1;
        }
        func_0019F1A0((char *)0x70003190, 8);
        func_0019F1A0((char *)0x700031A0, 0x37);
    }

    cnt = *(int *)0x7000320C;
    pT6 = D_70003240;
    pT3 = D_70003228;
    pS0 = sp50;

    for (i = 0; i < 6; i++) {
        int a3 = i & 1;
        if (i >= 2 && i < 4) {
            if (i & 1) {
                *(short *)0x70003B86 = *pT6;
                *(short *)0x70003B88 = (short) * (short *)(*pT3 + (*pS0 * 2));
            } else {
                *(short *)0x70003B86 = (short) * (short *)(*pT3 + (*pS0 * 2));
                *(short *)0x70003B88 = *pT6;
                *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
            }
        } else if (i >= 4) {
            a3 = i & 1;
            goto block10;
        } else {
block10:
            if (a3) {
                *(short *)0x70003B86 = *pT6;
                *(short *)0x70003B88 = (short) * (short *)(*pT3 + (*(short *)0x70003B86 * 2));
            } else {
                *(short *)0x70003B88 = *pT6;
                *(short *)0x70003B86 = (short) * (short *)(*pT3 + (*(short *)0x70003B88 * 2));
                *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
            }
        }
        {
            int a3v = *(short *)0x70003B88;
            int a2v = *(short *)0x70003B86;
            int t1 = a3v - a2v;
            if (t1 < cnt) {
                cnt = t1;
                lo = a2v;
                hi = a3v;
                best = i;
            }
        }
        pT6 += 2;
        pT3 += 4;
        pS0 += 1;
    }

    if (*(int *)0x70003244 == -1) {
        return 1;
    }

    s0 = (short *)(D_70003210[best] + lo * 2);
    if (lo < hi) {
        do {
            e = (char *)(*(int *)0x70003208 + (*s0 << 6));
            s0 += 2;
            if (*(short *)0x70003240 >= *(short *)(e + 0xC) &&
                *(short *)(e + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003244 >= *(short *)(e + 0x10) &&
                *(short *)(e + 0x12) >= *(short *)0x70003246 &&
                *(short *)0x70003248 >= *(short *)(e + 0x14) &&
                *(short *)(e + 0x16) >= *(short *)0x7000324A) {
                *(short *)0x70003B88 = *(unsigned char *)(e + 0x1A);
                a0v = *(short *)0x70003B88;
                if (a0v >= 0x1E && a0v < 0x5A && func_0019ED80((char *)0x70003190, e) != 0) {
                    *(float *)0x700031A4 = *(float *)0x700031B4;
                    for (j = 0; j < 3; j++) {
                        local[j] = *(float *)(0x70003190 + j * 4 + 0x20);
                    }
                    status = *(int *)0x700031D0;
                }
            }
            lo += 1;
        } while (lo < hi);
    }

    if (status != 0) {
        *(int *)0x700031D0 = status;
        for (j = 0; j < 3; j++) {
            *(float *)(0x70003190 + j * 4 + 0x20) = local[j];
        }
        return 0;
    }
    return 1;
}
