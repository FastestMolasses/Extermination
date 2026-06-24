// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Runs an actor-update validation pass over a 3-entry config table
// (D_00245C30, stride 0x10). Early-out returns 0 if actor flag bit at
// +0xD bit0 is set. For each of the 3 table rows: build a transform from
// the row (func_00102948 -> scratch D_700038A0), apply it against the
// actor (func_001B2B10), build a second transform from the actor's +0xB0
// vector (func_00102948 -> D_700038B0), compose them (func_001028B8),
// copy a float field (0x700038A4 -> 0x700038B4), then run the check
// func_0019A440(..., 6); a nonzero result aborts with return 0. Returns 1
// if all 3 rows pass.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202.
// Verified objdiff 100% vs build/expected/func_0014C130.o. Under 991202
// this lands at 89.5% (clean-store delay-slot family wall #13). Local
// var_s0 (loop counter) must be declared/initialized before var_s1 (table
// pointer) so the counter colors to $s0 and the pointer to $s1.
extern int func_001028B8(void *, void *, void *);
extern int func_00102948(void *, void *);
extern int func_0019A440(void *, void *, int);
extern int func_001B2B10(char *, void *, void *);
extern char D_00245C30;
extern char D_700038A0;
extern char D_700038B0;

int func_0014C130(char *arg0) {
    int var_s0;
    char *var_s1;

    if (*(unsigned char *)(arg0 + 0xD) & 1) {
        return 0;
    }
    var_s0 = 0;
    var_s1 = &D_00245C30;
    do {
        func_00102948(&D_700038A0, var_s1);
        func_001B2B10(arg0, &D_700038A0, &D_700038A0);
        func_00102948(&D_700038B0, arg0 + 0xB0);
        func_001028B8(&D_700038A0, &D_700038B0, &D_700038A0);
        *(float *)0x700038B4 = *(float *)0x700038A4;
        if (func_0019A440(&D_700038B0, &D_700038A0, 6) != 0) {
            return 0;
        }
        var_s0 += 1;
        var_s1 += 0x10;
    } while (var_s0 < 3);
    return 1;
}
