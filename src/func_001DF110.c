// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Allocates/initializes a 0x10-byte record at the bump-pointer D_00275670->[0x1C].
// Calls func_001DF020(3, arg0) (returns a value forwarded as arg2 of
// func_001CB760), captures the current record pointer p = base->[0x1C], writes
// header fields (byte +3 = 0x60, word +4 = 0, half +0 = 0), advances the bump
// pointer base->[0x1C] += 0x10, then calls
// func_001CB760(D_007635C0, 0xFFF000, func_001DF020_ret, p).
//
// Needs -sdatathreshold 4 so the pointer global D_00275670 is gp-relative
// (matches `lw t1, %gp_rel(D_00275670)(gp)`); D_007635C0 is declared as an open
// array so it stays absolute (%hi/%lo) rather than going gp-rel. The base
// pointer is cached once in a local to reproduce the single gp_rel load reused
// across all field accesses.
//
// Built with mwcc 2.3.3, not 991202 (92% -- scheduling/MAC differences).
// Verified objdiff 100% vs build/expected/func_001DF110.o.
extern int func_001DF020(int, int);
extern void func_001CB760(void *, int, int, char *);
extern char D_007635C0[];
extern char *D_00275670;

void func_001DF110(int arg0) {
    char *base;
    char *p;
    int r;
    r = func_001DF020(3, arg0);
    base = D_00275670;
    p = *(char **)(base + 0x1C);
    *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 0;
    *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
    func_001CB760(D_007635C0, 0xFFF000, r, p);
}
