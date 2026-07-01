// NEARMISS func_0019DF10  (vram 0x0019DF10, 0x370 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling permutation, same class as siblings func_0019C830 (85.2%) and func_0019E640 (93.6%). Body/control-flow and every constant fully recovered (dual-candidate window select over D_70003190/D_700031A0, 6-slot min-window scan via func_0019F1A0, 64-byte poly-node walk with fu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0019F1A0(void *p, int a1);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];    // camera/spad candidate A (32-byte record; +0x0/+0x8 float pair, +0x10/+0x20 vec3)
extern char D_700031A0[];    // camera/spad candidate B (same layout)
extern int D_70003210[6];
extern short D_70003240[6];
extern char D_70003228[];

int func_0019DF10(void) {
    int maskA, maskB;
    int sp80[6];
    int i;
    int best;
    int lo, hi;
    int cand;
    short *e;
    float spA0[3];
    int hitfound;
    int k;

    if (*(float *)0x70003190 > *(float *)0x700031A0) {
        hitfound = 0;
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

    func_0019F1A0(D_70003190, maskB);
    func_0019F1A0(D_700031A0, maskA);

    for (i = 0; i < 6; i++) {
        sp80[i] = D_70003240[i];
    }

    func_0019F1A0(D_70003190, maskA);
    func_0019F1A0(D_700031A0, maskB);

    best = *(int *)0x7000320C;
    for (i = 0; i < 6; i++) {
        if ((unsigned int)(i - 2) >= 2U) {
            if (i & 1) {
                *(short *)0x70003B86 = D_70003240[i];
                *(short *)0x70003B88 = *(short *)((char *)D_70003228 + i * 4 + (sp80[i] << 1));
            } else {
                *(short *)0x70003B88 = D_70003240[i];
                *(short *)0x70003B86 = *(short *)((char *)D_70003228 + i * 4 + (sp80[i] << 1));
                *(short *)0x70003B88 = *(short *)0x70003B86 + 1;
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

    e = (short *)((char *)D_70003210[cand] + (lo * 2));
    if (lo < hi) {
        do {
            char *node = *(char **)0x70003208 + (*e << 6);
            e += 2;
            if (*(short *)0x70003240 >= *(short *)(node + 0xC) &&
                *(short *)(node + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(node + 0x14) &&
                *(short *)(node + 0x16) >= *(short *)0x7000324A) {
                unsigned char kind = *(unsigned char *)(node + 0x1A);
                if (kind >= 0x5A && kind < 0x78 && func_0019ED80(D_70003190, node) != 0) {
                    for (k = 0; k < 3; k++) {
                        *(float *)(D_70003190 + k * 4 + 0x10) = *(float *)(D_70003190 + k * 4 + 0x20);
                        spA0[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                    }
                    hitfound = *(int *)0x700031D0;
                }
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
