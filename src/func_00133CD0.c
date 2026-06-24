// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Runs a 5-iteration setup loop over the table at D_00245140 (advancing the
// row pointer by 0x10 each pass). Per iteration: func_00102948 copies a matrix
// into the scratch block D_700038A0 from the current row; func_001B2B10 seeds
// it from the object p; a second func_00102948 stages p+0xB0 into D_700038B0;
// func_001028B8 combines the two scratch blocks; one scratch float (0x700038A4)
// is mirrored to 0x700038B4; then func_0019A440 tests the result and, on a
// nonzero return, bails out returning 0. If all five passes succeed, returns 1.
//
// Matched with mwcc 2.3.3 (also clean on mwcc24); the pinned 991202 build only
// reaches 82.4% (loop saved-reg/scheduling divergence). objdiff 100.0 vs expected.
extern void func_00102948(void *dst, void *src);
extern void func_001B2B10(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019A440(void *a, void *b, int n);
extern char D_00245140[];
extern char D_700038A0[];
extern char D_700038B0[];

int func_00133CD0(char *p) {
    int i;
    char *q;

    q = D_00245140;
    for (i = 0; i < 5; i++) {
        func_00102948(D_700038A0, q);
        func_001B2B10(p, D_700038A0, D_700038A0);
        func_00102948(D_700038B0, p + 0xB0);
        func_001028B8(D_700038A0, D_700038B0, D_700038A0);
        *(float *)0x700038B4 = *(float *)0x700038A4;
        if (func_0019A440(D_700038B0, D_700038A0, 6) != 0) {
            return 0;
        }
        q += 0x10;
    }
    return 1;
}
