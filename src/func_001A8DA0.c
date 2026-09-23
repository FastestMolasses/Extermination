// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Double-loop collision/interaction pass over two
// global object lists. Outer list: D_00275BB0 (base) / D_00275BB8 (count); inner
// list: D_00275BA0 (base) / D_00275BA8 (count). Runs only if both counts are
// nonzero. For each outer entry e whose flag byte e[0] has bit0 set, it walks the
// inner list using a global short countdown at 0x70003B86 (seeded from
// D_00275BA8): for each inner entry f with bit0 of f[0] set, type byte f[3]==3 and
// f[0xD]==0, it calls func_001A8CE0(e, f). The 0x70003B86 counter is decremented
// per inner iteration and re-tested, so a nested handler can shorten the inner
// walk.
//
// MATCH NOTE (m2-matching lane, 87.8% -> 100.0%): three source facts.
// (1) The 0x70003B86 countdown is a relocated, VOLATILE scratchpad short
// (idiom-22/32), over-declared as an array (idiom #20) so -sdatathreshold 8
// keeps it absolute; volatile gives the target's per-access reload.
// (2) The counter store precedes the D_00275BA0 load, in source order.
// (3) The outer-list base D_00275BB0 is read BEFORE the `n == 0` guard, so the
// load is in the guard's own block and fills its delay slot. mwcc never
// speculates a load from a successor into a conditional-branch slot, but it
// does move one from the same block.
extern void func_001A8CE0(unsigned char *, unsigned char *);
extern unsigned char **D_00275BA0;
extern short D_00275BA8;
extern unsigned char **D_00275BB0;
extern short D_00275BB8;
extern volatile short D_70003B86[8];

void func_001A8DA0(void) {
    int n;
    unsigned char *e;
    unsigned char **p;
    unsigned char **q;
    unsigned char *f;

    if (D_00275BA8 == 0) return;
    n = D_00275BB8;
    q = D_00275BB0;
    if (n == 0) return;
    do {
        e = *q++;
        n--;
        if ((*e & 1) != 0) {
            D_70003B86[0] = D_00275BA8;
            p = D_00275BA0;
            while (D_70003B86[0] != 0) {
                f = *p++;
                D_70003B86[0] = D_70003B86[0] - 1;
                if ((*f & 1) != 0 && f[3] == 3 && f[0xD] == 0) func_001A8CE0(e, f);
            }
        }
    } while (n != 0);
}
