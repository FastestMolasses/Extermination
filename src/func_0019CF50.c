// NEARMISS func_0019CF50  (vram 0x0019CF50, 0x3E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Byte-for-byte the same function as func_0019D770 (near-identical twin, only 2 structural differences: an extra `st < 0x5A` upper-bound clamp on the surface-attr guard, and the hit/miss return polarity is inverted -- 0 = hit found, 1 = miss, with the D_700031D0 store moved BEFORE the normal copy-b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019ED80(void *node, void *edata);
extern void func_0019F1A0(void *seg, int mask);
extern float D_70003190;
extern float D_700031A0;
extern short D_70003240;
extern int D_70003228;
extern int D_70003210;

int func_0019CF50(void) {
    int s1, s2, best_i;
    int hit;
    int s5, s6;
    int sp80[6];
    float spA0[3];
    int i;
    short *p;
    int *q;
    int v0;
    short v1;
    short a2, a3;
    int t0;

    hit = 0;
    if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
        s5 = 1;
        s6 = 2;
    } else {
        s5 = 2;
        s6 = 1;
    }
    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
        s5 |= 4;
        s6 |= 8;
    } else {
        s5 |= 8;
        s6 |= 4;
    }
    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
        s5 |= 0x10;
        s6 |= 0x20;
    } else {
        s5 |= 0x20;
        s6 |= 0x10;
    }

    func_0019F1A0(&D_70003190, s6);
    func_0019F1A0(&D_700031A0, s5);

    for (i = 0; i < 6; i++) {
        sp80[i] = (&D_70003240)[i * 2];
    }

    func_0019F1A0(&D_70003190, s5);
    func_0019F1A0(&D_700031A0, s6);

    v0 = *(int *)0x7000320C;
    p = &D_70003240;
    q = &D_70003228;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = *p;
            a2 = *(short *)(q[i] + sp80[i] * 2);
        } else {
            *(short *)0x70003B86 = *(short *)(q[i] + sp80[i] * 2);
            *(short *)0x70003B88 = *p;
            a2 = *(short *)0x70003B88 + 1;
        }
        *(short *)0x70003B88 = a2;
        a3 = *(short *)0x70003B88;
        a2 = *(short *)0x70003B86;
        t0 = a3 - a2;
        if (t0 < v0) {
            v0 = t0;
            s1 = a2;
            s2 = a3;
            best_i = i;
        }
        p += 2;
        q += 4;
    }

    if (s1 < s2) {
        short *node = (short *)(*(&D_70003210 + best_i)) + s1;
        do {
            char *e = (char *)(*(int *)0x70003208) + (*node << 6);
            node += 2;
            if (*(short *)0x70003240 >= *(short *)(e + 0xC) &&
                *(short *)(e + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(e + 0x14) &&
                *(short *)(e + 0x16) >= *(short *)0x7000324A &&
                *(short *)0x70003244 >= *(short *)(e + 0x10) &&
                *(short *)(e + 0x12) >= *(short *)0x70003246) {
                short st = *(unsigned char *)(e + 0x1A);
                *(short *)0x70003B88 = st;
                st = *(short *)0x70003B88;
                if (st < 0x5A && (st < 0x51 || st >= 0x54)) {
                    if (func_0019ED80(&D_70003190, e)) {
                        int k;
                        for (k = 0; k < 3; k++) {
                            *(float *)(0x70003190 + 0x10 + k * 4) = *(float *)(0x70003190 + 0x20 + k * 4);
                            spA0[k] = *(float *)(0x70003190 + 0x20 + k * 4);
                        }
                        hit = *(int *)0x700031D0;
                    }
                }
            }
            s1 += 1;
        } while (s1 < s2);
    }

    v0 = 1;
    if (hit != 0) {
        int k;
        *(int *)0x700031D0 = hit;
        for (k = 0; k < 3; k++) {
            *(float *)(0x70003190 + 0x20 + k * 4) = spA0[k];
        }
        v0 = 0;
    }
    return v0;
}
