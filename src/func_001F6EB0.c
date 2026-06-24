// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Audio/state region dispatcher. Builds a 16-bit selector `code` from two
// adjacent globals (D_00810700<<8 | D_00810701) and routes on it. For the two
// known region codes (0x700 and 0x1200), it consults a per-region "current
// track" word (D_0025D524 for 0x700, D_0025D6E4 for 0x1200): if that word is
// -1 (none active) it starts via func_001F6E40 unless the request flag
// D_00810702 is already 1; otherwise (a track is active) it stops via
// func_001F6E80 only when D_00810702 == 1. The low byte of `code` (hi) is
// passed through to both callees as the second argument.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. The
// natural switch lowering places the 0x700 handler first in code and the
// 0x1200 handler last while comparing 0x1200 first in the dispatch — matching
// CW exactly. 2.3.3 byte-matches; 991202 only reaches 85.3% (delay-slot/branch
// lowering of the dispatch differs). Verified objdiff 100.0% vs
// build/expected/func_001F6EB0.o (zero diffing instructions).
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern int D_0025D524;
extern int D_0025D6E4;
extern void func_001F6E40(unsigned char, int);
extern void func_001F6E80(unsigned char, int);

void func_001F6EB0(void) {
    int hi = D_00810700 << 8;
    int code = hi + D_00810701;

    switch (code) {
    case 0x700:
        if (D_0025D524 == -1) {
            if (D_00810702 != 1) {
                func_001F6E40(D_00810702, hi);
            }
        } else {
            if (D_00810702 == 1) {
                func_001F6E80(D_00810702, hi);
            }
        }
        break;
    case 0x1200:
        if (D_0025D6E4 == -1) {
            if (D_00810702 != 1) {
                func_001F6E40(D_00810702, hi);
            }
        } else {
            if (D_00810702 == 1) {
                func_001F6E80(D_00810702, hi);
            }
        }
        break;
    }
}
