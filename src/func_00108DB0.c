// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.23% NEARMISS).
// Fixes read from the original instructions:
//  - func_00108818 takes two arguments (index, value), as its definition in src/
//    shows; the old one-argument extern dropped the pass-through of a2.
//  - The loop counter is not initialised at its declaration (only by the for), so
//    ee-gcc no longer shares it with the zero result and tests the count directly.
//  - The 16-byte table D_00241238 is indexed as an array of {lo, hi} pairs; the
//    hand-computed byte offset swapped the address-sum operands.
//
// Looks up the key derived from (a1, a2) in a table of 0x18-byte entries hanging
// off a0[0x40] (entries at +0x44, count at +0x48) and returns the matching entry's
// value field (0 when none). If the scan stopped below 64 entries, the count is
// bumped and the slot at the stop index is (re)written with the key, a3, t0 and
// the table's high word for a1.

extern long func_00108818(unsigned int idx, long arg);

struct tent { long lo; long hi; };
extern struct tent D_00241238[];

struct ent { long key; long f8; int f10; int f14; };

int func_00108DB0(int *a0, int a1, int a2, int a3, int t0) {
    int s1 = 0;
    int *s2 = *(int **)((char *)a0 + 0x40);
    struct ent *s0 = *(struct ent **)((char *)s2 + 0x44);
    long key = func_00108818(a1, a2);
    int cnt = *(int *)((char *)s2 + 0x48);
    int i;

    for (i = 0; i < cnt; i++) {
        if (key == s0[i].key) {
            s1 = s0[i].f10;
            break;
        }
    }

    if (i < 0x40) {
        struct ent *e = &s0[i];
        *(int *)((char *)s2 + 0x48) = cnt + 1;
        e->key = key;
        e->f14 = t0;
        e->f10 = a3;
        e->f8 = D_00241238[a1].hi;
    }
    return s1;
}
