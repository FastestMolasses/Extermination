// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Iterates an array of fixed-stride (0x860-byte) records anchored at the
// global table *D_0028A5A4: element count is the first word, records begin
// at +0x10. For each record it calls func_001D5A70(rec, rec+0x40) (a state
// poll). On result 0 it runs func_001D4FC0(rec); on a non-0xFF result it
// runs func_001D4FC0(rec) then func_001D5170(rec); a 0xFF result is skipped.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 build
// (which scores 85.24%). The loop guard written as `if (0 < count)` (not
// `count > 0`) yields the target `slt at,zero,s0; beqz at` entry test;
// declaration order (count, then ptr) gives the target saved-reg mapping
// (count->s0, ptr->s1). D_0028A5A4 is a far global (hi/lo, sdatathreshold 0).
// Verified TRUE objdiff 100.0 byte-identical vs build/expected/func_001D5BD0.o.
extern int func_001D4FC0(char *);
extern int func_001D5170(char *);
extern int func_001D5A70(char *, char *);
extern char *D_0028A5A4;

void func_001D5BD0(void) {
    char *base;
    int count;
    char *p;
    char *q;
    int v0;
    int i;

    i = 0;
    base = D_0028A5A4;
    count = *(int *)base;
    p = base + 0x10;
    if (0 < count) {
        do {
            q = p + 0x40;
            v0 = func_001D5A70(p, q);
            if (v0 == 0) {
                func_001D4FC0(p);
            } else if (v0 != 0xFF) {
                func_001D4FC0(p);
                func_001D5170(p);
            }
            i += 1;
            p = q + 0x820;
        } while (i < count);
    }
}
