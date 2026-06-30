// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state driver dispatched on the state byte at arg0+1 (states 0/1).
// State 0 bumps the state byte (+1) and zeroes arg0+2, then falls through.
// State 1 (common): if the mode word at arg1+0x230 == 0x11, sets the 0xA byte
// at arg0+6 and clears the state byte; then kicks two clip/anim updates via
// func_0018C4B0(self+0x20, arg1->0xB4 + self->0x8C, 0.2f) and
// func_0018C4B0(&D_008105E0, self->0x24, 0.1f).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 95.1%. Writing the dispatch as a `switch` (case 0 falling through to case 1)
// reproduces CW's descending compare order with the case-0 branch-likely
// (beqzl, delay slot = the `e[1]=st+1` increment via the switch local). objdiff
// 100.0 vs expected (only render delta is objdiff naming the $f13 odd-half
// register fa0f vs fa1; identical encoded bytes).
extern void func_0018C4B0(void *p, float a, float b);
extern char D_008105E0;

void func_00198CE0(unsigned char *e, unsigned char *arg1) {
    unsigned char st;

    st = e[1];
    switch (st) {
    case 0:
        e[1] = st + 1;
        e[2] = 0;
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x230) == 0x11) {
            e[6] = 0xA;
            e[1] = 0;
        }
        func_0018C4B0(e + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(e + 0x8C), 0.2f);
        func_0018C4B0(&D_008105E0, *(float *)(e + 0x24), 0.1f);
        break;
    }
}
