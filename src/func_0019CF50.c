// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.05% NEARMISS).
// Fixes read from the original instructions (same fixes as its twin func_0019D770):
// - each span-loop arm stores both scratchpad halfwords itself; the even arm
//   increments 0x70003B88 in place (no short temporary, so no sign extension);
// - the cell-list pointer is computed before the walk test, so the walk is a
//   `for` loop (not if + do/while); the cell lists are `short *D_70003210[6]`;
// - face bound tests are spelled `face field <= bound` / `>= bound`, the kind
//   skip is `st < 0x51 || st > 0x53`, and the face address is one sum;
// - the best-span-length local is declared first (its own variable, not the
//   return value), hit/best_i order fixed; func_0019F1A0 takes (float *, unsigned int);
// - tail is `if (hit) { write back 0x700031D0; restore point; return 0; } return 1;`.
//
// Twin of func_0019D770; this one adds the `st < 0x5A` bound on the face kind and
// returns 0 on a hit, 1 on a miss. The six bounds at 0x70003240 are halfwords, the
// column table at 0x70003228 is words, the cell lists are halfwords. As in the twin,
// if no span is shorter than the 0x7000320C word, s1/s2/best_i are uninitialised and
// the original walks with its caller's registers (see func_0019D770.c).

extern int func_0019ED80(void *node, void *edata);
extern void func_0019F1A0(float *pt, unsigned int mask);
extern float D_70003190;
extern float D_700031A0;
extern short D_70003240;
extern int D_70003228;
extern short *D_70003210[6];

int func_0019CF50(void) {
    int best_len;
    int s1, s2, hit, best_i;
    int s5, s6;
    int sp80[6];
    float spA0[3];
    int i;
    short *p;
    int *q;
    short a2, a3;
    int t0;
    short *node;

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
        sp80[i] = (&D_70003240)[i];
    }

    func_0019F1A0(&D_70003190, s5);
    func_0019F1A0(&D_700031A0, s6);

    best_len = *(int *)0x7000320C;
    p = &D_70003240;
    q = &D_70003228;
    for (i = 0; i < 6; i++) {
        if (i & 1) {
            *(short *)0x70003B86 = *p;
            *(short *)0x70003B88 = *(short *)(*q + sp80[i] * 2);
        } else {
            *(short *)0x70003B86 = *(short *)(*q + sp80[i] * 2);
            *(short *)0x70003B88 = *p;
            *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
        }
        a3 = *(short *)0x70003B88;
        a2 = *(short *)0x70003B86;
        t0 = a3 - a2;
        if (t0 < best_len) {
            best_len = t0;
            s1 = a2;
            s2 = a3;
            best_i = i;
        }
        p += 1;
        q += 1;
    }

    node = &D_70003210[best_i][s1];
    for (; s1 < s2; s1++) {
        char *e = (char *)(*(int *)0x70003208 + (*node << 6));
        node += 1;
        if (*(short *)(e + 0xC) <= *(short *)0x70003240 &&
            *(short *)(e + 0xE) >= *(short *)0x70003242 &&
            *(short *)(e + 0x14) <= *(short *)0x70003248 &&
            *(short *)(e + 0x16) >= *(short *)0x7000324A &&
            *(short *)(e + 0x10) <= *(short *)0x70003244 &&
            *(short *)(e + 0x12) >= *(short *)0x70003246) {
            short st = *(unsigned char *)(e + 0x1A);
            *(short *)0x70003B88 = st;
            st = *(short *)0x70003B88;
            if (st < 0x5A && (st < 0x51 || st > 0x53)) {
                if (func_0019ED80(&D_70003190, e)) {
                    float *f = &D_70003190;
                    int k;
                    for (k = 0; k < 3; k++) {
                        f[4 + k] = f[8 + k];
                        spA0[k] = f[8 + k];
                    }
                    hit = *(int *)0x700031D0;
                }
            }
        }
    }

    if (hit != 0) {
        *(int *)0x700031D0 = hit;
        for (i = 0; i < 3; i++) {
            (&D_70003190)[8 + i] = spA0[i];
        }
        return 0;
    }
    return 1;
}
