// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Entry state byte at *arg0 gates the work: state 0 advances to state 1
// (*arg0=1) and falls through; state 1 runs the body; any other state returns.
// Body: when the 16-bit code (D_00810700<<8 | D_00810701) equals 0x1500, calls
// func_001D2910(0x24) and dispatches func_001E2260 with one of two 64-bit GIF/
// packet tags (0x20076C0121323740 if non-zero, else 0x20076A8121323700). Then
// unconditionally calls func_001E0CF0() and func_001D5370().
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906) at objdiff 100.0; the pinned
// 991202 build reaches 89.4%. Key: the entry dispatch must be a switch with a
// case-0->case-1 fallthrough so mwcc emits the target's descending beq==1 /
// beqz==0 dispatch and the default early-return epilogue.
extern int func_001D2910(int a);
extern void func_001E2260(long long a);
extern void func_001E0CF0(void);
extern void func_001D5370(void);
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001C1D00(unsigned char *arg0) {
    unsigned char st;

    st = *arg0;
    switch (st) {
    case 0:
        *arg0 = 1;
        /* fallthrough */
    case 1:
        if (((D_00810700 << 8) + D_00810701) == 0x1500) {
            if (func_001D2910(0x24) != 0) {
                func_001E2260(0x20076C0121323740LL);
            } else {
                func_001E2260(0x20076A8121323700LL);
            }
        }
        func_001E0CF0();
        func_001D5370();
        break;
    }
}
