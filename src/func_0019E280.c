// NEARMISS func_0019E280  (vram 0x0019E280, 0x3B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.43% on mwcc233. Full logic recovered including two bugs found and fixed versus the initial draft: (1) the odd/even index handling in the best-candidate search loop had the +1 correction on the wrong parity branch (target applies +1 only on the EVEN i case, not odd); fixing this alone jumped th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0019F1A0(void *p, unsigned int flags);
extern int func_0019ED80(void *p, void *hit);

extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x0/+0x8 float pair, +0x10/+0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern short *D_70003210[6]; // per-slot record-index pointer table
extern short *D_70003228[6]; // per-slot short-array pointer table
extern short D_70003240[6];  // per-slot short table (index src)

int func_0019E280(void) {
    int maskA, maskB;
    int i;
    int sp80[6];
    int best;
    int cand, lo, hi;
    short *e;
    float spA0[3];
    int hitfound;
    int k;

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
                *(short *)0x70003240 >= *(short *)(node + 0xC) &&
                *(short *)(node + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(node + 0x14) &&
                *(short *)(node + 0x16) >= *(short *)0x7000324A &&
                *(short *)0x70003244 >= *(short *)(node + 0x10) &&
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
