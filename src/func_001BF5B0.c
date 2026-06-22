// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Flag-gated state-change notifier (structural twin of func_001BEC40, different
// strings/codes). If the 0x1000 bit of status halfword (*p) is set and mode
// short p[1] differs from (short)arg2, post a message via
// func_001D0D40(arg0, <string>, <code>, 1) and latch p[1]: arg2==0 -> string
// D_0024FD50 (code 0x5B), set p[1]=0; else string D_00250750 (code 0x41), set
// p[1]=1. The (short) cast on the int arg gives the dsll32/dsra32 idiom.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under 991202 the sole residual is
// wall #13 (fills the `bnez` delay slot with the safe lui where CW leaves a
// nop). 2.3.3 leaves the nop -> objdiff 100.0 vs expected.
extern void func_001D0D40(int a0, char *a1, int a2, int a3);
extern char D_0024FD50[];
extern char D_00250750[];

void func_001BF5B0(int arg0, short *p, int arg2) {
    if (p[0] & 0x1000) {
        short v = arg2;
        if (p[1] != v) {
            if (v == 0) {
                func_001D0D40(arg0, D_0024FD50, 0x5B, 1);
                p[1] = 0;
            } else {
                func_001D0D40(arg0, D_00250750, 0x41, 1);
                p[1] = 1;
            }
        }
    }
}
