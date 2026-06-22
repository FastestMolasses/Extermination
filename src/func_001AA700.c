// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Actor-table scan + conditional dispatch. Walks the actor pointer table
// (base D_00275B7C, count D_00275B84) and for each non-null entry whose
// type/class byte (+0x02, low 5 bits) == 4, subtype byte (+0x03) == 6, and
// status byte (+0x00) == 1, invokes func_001AA640(arg0, entry). The count is
// loaded once into an int loop counter (lh sign-extends the short, then it is
// treated as int -- no per-iteration re-narrowing). The redundant outer
// `if (count != 0)` before the do/while reproduces CW's double beqz guard.
//
// Matched on mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 stalls at 95.79%
// (delay-slot/schedule wall). -sdatathreshold 8 makes the gp-window globals
// D_00275B84/D_00275B7C gp-relative. Declaration order (pointer `p` before
// counter `count`) steers regalloc to CW's choice (counter->s1, pointer->s0).
// Verified objdiff 100% vs build/expected/func_001AA700.o.
extern int func_001AA640(int, char *);
extern char **D_00275B7C;
extern short D_00275B84;

void func_001AA700(int arg0) {
    char **p;
    int count;
    char *e;

    count = D_00275B84;
    if (count != 0) {
        p = D_00275B7C;
        if (count != 0) {
            do {
                e = *p;
                count -= 1;
                p += 1;
                if ((e != 0) &&
                    ((*(unsigned char *)(e + 2) & 0x1F) == 4) &&
                    (*(unsigned char *)(e + 3) == 6) &&
                    (*(unsigned char *)(e + 0) == 1)) {
                    func_001AA640(arg0, e);
                }
            } while (count != 0);
        }
    }
}
