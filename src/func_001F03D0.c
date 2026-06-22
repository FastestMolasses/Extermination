// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Initializes a 0x20-entry array of 0x60-byte records at the per-index base
// &D_0028F700 + arg0*0xC00 + 0x4DBEC0 (0xC00 = 0x20 * 0x60, so arg0 selects a
// bank of 0x20 records). For each record: calls per-record init func_001029C0,
// then zeroes the 8 bytes at +0x50 and the words at +0x58 and +0x5C. Finally
// zeroes the arg0-th word of the int table D_0081F950.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Verified
// objdiff 100% vs build/expected/func_001F03D0.o. (991202 = 64.7%.) The local
// declaration order (int i before char *p) plus the statement order p=...; i=0;
// is load-bearing for the s0/s1/s2 saved-register assignment.
extern void func_001029C0(char *);
extern char D_0028F700;
extern int D_0081F950;

void func_001F03D0(int arg0) {
    int i;
    char *p;

    p = &D_0028F700 + arg0 * 0xC00 + 0x4DBEC0;
    i = 0;
    do {
        func_001029C0(p);
        *(long long *)(p + 0x50) = 0;
        i += 1;
        *(int *)(p + 0x58) = 0;
        *(int *)(p + 0x5C) = 0;
        p += 0x60;
    } while (i < 0x20);
    (&D_0081F950)[arg0] = 0;
}
