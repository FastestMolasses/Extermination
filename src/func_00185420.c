// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Linear search over the active-object table: D_00275B64 entries pointed to by
// D_00275B5C (array of object pointers). For each object it checks flag byte +0x0 bit0
// (active), flag byte +0x2 bit7, and state byte +0xB == 0; the first object passing all
// three AND accepted by func_00184D20(&D_008102B0 table, obj, arg0) is returned. Returns
// NULL if none match or the table is empty.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (86.11%). Levers:
// -sdatathreshold 8 keeps the two gp-relative counters/pointers gp-rel while the [256]
// over-declaration of D_008102B0 stays %hi/%lo absolute; the loop counter is `int`
// (a `short` would emit dsll32/dsra32 sign-extension each decrement); the table base is
// hoisted into a local before the loop so it claims the 5th saved reg (s2) instead of
// being recomputed each iteration; `p` is declared before `pp` to color them s0/s1 to
// match. Verified objdiff 100.0 vs build/expected/func_00185420.o.
extern int func_00184D20(char *, char *, int);
extern char **D_00275B5C;
extern short D_00275B64;
extern char D_008102B0[256];

char *func_00185420(int arg0) {
    int n;
    char *p;
    char **pp;
    char *tbl;

    tbl = D_008102B0;
    n = D_00275B64;
    pp = D_00275B5C;
    if (n != 0) {
        do {
            p = *pp;
            n -= 1;
            pp += 1;
            if ((*(unsigned char *)(p + 0) & 1) &&
                (*(unsigned char *)(p + 2) & 0x80) &&
                (*(unsigned char *)(p + 0xB) == 0) &&
                func_00184D20(tbl, p, arg0)) {
                return p;
            }
        } while (n != 0);
    }
    return 0;
}
