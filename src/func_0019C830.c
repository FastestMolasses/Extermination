// NEARMISS func_0019C830  (vram 0x0019C830, 0x32C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling permutation. Instruction count is EXACTLY equal (219==219) between target and my compile -- structure and logic are fully recovered (binary-search-style scan over 6 candidate ranges to pick the best-fit index window, scratchpad round-trip through 0x70003B86/0x70003B88...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0019F1A0(int p, unsigned int flags);
extern int func_0019ED80(void *p, void *hit);
extern char D_70003190[];
extern char D_700031A0[];
extern int D_70003210[6];
extern short D_70003240[6];
extern char D_70003228[];

int func_0019C830(void) {
    int i;
    int best;
    int lo, hi;
    int cand;
    short *e;
    float sp50[3];
    int hitfound;

    if (*(float *)0x70003194 > *(float *)0x700031A4) {
        func_0019F1A0((int)D_70003190, 0x33);
    } else {
        func_0019F1A0((int)D_700031A0, 0x33);
    }

    best = *(int *)0x7000320C;
    hitfound = 0;
    for (i = 0; i < 6; i++) {
        if ((unsigned int)(i - 2) >= 2U) {
            if (i & 1) {
                *(short *)0x70003B86 = D_70003240[i];
                *(short *)0x70003B88 = *(short *)((char *)D_70003228 + i * 4) + (*(short *)0x70003B86 << 1);
            } else {
                *(short *)0x70003B88 = D_70003240[i];
                *(short *)0x70003B86 = *(short *)((char *)D_70003228 + i * 4 + (*(short *)0x70003B88 << 1));
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
                short kind = *(unsigned char *)(node + 0x1A);
                if (kind < 0x5A) {
                    int ok = 0;
                    if (kind == 0x51) {
                        if (*(short *)0x7000324E == 0) ok = 1;
                    } else if (kind == 0x52) {
                        if (*(short *)0x7000324E == 2) ok = 1;
                    } else {
                        if (kind != 0x53 || *(short *)0x7000324E != -1) ok = 1;
                    }
                    if (ok && func_0019ED80(D_70003190, node) != 0) {
                        int k;
                        *(float *)0x700031A4 = *(float *)0x700031B4;
                        for (k = 0; k < 3; k++) {
                            sp50[k] = *(float *)(D_70003190 + k * 4 + 0x20);
                        }
                        hitfound = *(int *)0x700031D0;
                    }
                }
            }
            lo += 1;
        } while (lo < hi);
    }

    if (hitfound != 0) {
        int k;
        *(int *)0x700031D0 = hitfound;
        for (k = 0; k < 3; k++) {
            *(float *)(D_70003190 + k * 4 + 0x20) = sp50[k];
        }
        return 0;
    }
    return 1;
}
