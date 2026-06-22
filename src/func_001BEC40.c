// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Flag-gated state-change notifier. If the 0x1000 bit of the status halfword
// (*p) is set, and the current mode short p[1] differs from the requested
// (short)arg2, post a message via func_001D0D40(arg0, <string>, <code>, 1) and
// latch p[1] to the new state: arg2==0 -> string D_0024E960 (code 0x65), set
// p[1]=0; else string D_0024F470 (code 0x29), set p[1]=1. The (short) cast on
// the int arg produces the dsll32/dsra32 sign-extend idiom.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 the sole
// residual is wall #13 (it fills the `bnez` delay slot with the safe lui where
// CW leaves a nop). 2.3.3 leaves the nop -> objdiff 100.0 vs expected.
extern void func_001D0D40(int a0, char *a1, int a2, int a3);
extern char D_0024E960[];
extern char D_0024F470[];

void func_001BEC40(int arg0, short *p, int arg2) {
    if (p[0] & 0x1000) {
        short v = arg2;
        if (p[1] != v) {
            if (v == 0) {
                func_001D0D40(arg0, D_0024E960, 0x65, 1);
                p[1] = 0;
            } else {
                func_001D0D40(arg0, D_0024F470, 0x29, 1);
                p[1] = 1;
            }
        }
    }
}
