// NEARMISS func_0019DB50  (vram 0x0019DB50, 0x3C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling permutation, same class as sibling family func_0019DF10 (91.86%), func_0019C830 (85.2%), func_0019E640 (93.64%) which all share this exact code shape (D_70003190/D_700031A0 dual-candidate window select + 6-slot min-window scan via func_0019F1A0 + 64-byte poly-node wal...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019ED80(void *p, void *hit);
extern void func_0019F1A0(int p, unsigned int flags);
extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x10/+0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern int D_70003210[6];
extern short D_70003240[6];
extern char D_70003228[];

int func_0019DB50(void) {
    unsigned int flagsA, flagsB;
    int i;
    int best;
    int lo, hi;
    int cand;
    short *e;
    float sp[3];
    int hitfound;
    int idx[6];

    hitfound = 0;

    if (*(float *)0x70003190 > *(float *)0x700031A0) {
        flagsA = 1;
        flagsB = 2;
    } else {
        flagsA = 2;
        flagsB = 1;
    }
    if (*(float *)0x70003194 > *(float *)0x700031A4) {
        flagsA = flagsA | 4;
        flagsB = flagsB | 8;
    } else {
        flagsA = flagsA | 8;
        flagsB = flagsB | 4;
    }
    if (*(float *)0x70003198 > *(float *)0x700031A8) {
        flagsA = flagsA | 0x10;
        flagsB = flagsB | 0x20;
    } else {
        flagsA = flagsA | 0x20;
        flagsB = flagsB | 0x10;
    }

    func_0019F1A0((int)D_70003190, flagsB);
    func_0019F1A0((int)D_700031A0, flagsA);

    for (i = 0; i < 6; i++) {
        idx[i] = D_70003240[i];
    }

    func_0019F1A0((int)D_70003190, flagsA);
    func_0019F1A0((int)D_700031A0, flagsB);

    best = *(int *)0x7000320C;
    for (i = 0; i < 6; i++) {
        short a86, a88;
        if (i & 1) {
            a86 = D_70003240[i];
            a88 = *(short *)((char *)D_70003228 + i * 4 + (idx[i] << 1));
        } else {
            a86 = *(short *)((char *)D_70003228 + i * 4 + (idx[i] << 1));
            a88 = (short)(D_70003240[i] + 1);
        }
        {
            int diff = a88 - a86;
            if (diff < best) {
                best = diff;
                lo = a86;
                hi = a88;
                cand = i;
            }
        }
    }

    e = (short *)((char *)D_70003210[cand] + (lo * 2));
    if (lo < hi) {
        do {
            char *node = *(char **)0x70003208 + (*e << 6);
            e += 2;
            if (*(short *)0x70003240 >= *(short *)(node + 0xC) &&
                *(short *)(node + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(node + 0x14) &&
                *(short *)(node + 0x16) >= *(short *)0x7000324A &&
                *(short *)0x70003244 >= *(short *)(node + 0x10) &&
                *(short *)(node + 0x12) >= *(short *)0x70003246 &&
                (short)*(unsigned char *)(node + 0x1A) < 0x5A &&
                func_0019ED80(D_70003190, node) != 0) {
                int k;
                for (k = 0; k < 3; k++) {
                    *(float *)(D_70003190 + k * 4 + 0x10) = *(float *)(D_70003190 + k * 4 + 0x20);
                    sp[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                }
                hitfound = *(int *)0x700031D0;
            }
            lo += 1;
        } while (lo < hi);
    }

    if (hitfound != 0) {
        int k;
        for (k = 0; k < 3; k++) {
            *(float *)(D_70003190 + k * 4 + 0x20) = sp[k];
        }
        *(int *)0x700031D0 = hitfound;
        return 1;
    }
    return 0;
}
