// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// fx_render: call func_001C22A0(p); on a nonzero result return 3, otherwise
// call func_001C6380(p) and return 1. The sq/lq frame saves $ra and $s0
// (p kept across the calls).
//
// Built with mwcc 2.3.3, not 991202: the residual under 991202 was the
// mwcc-vs-CW branch lowering / delay-slot fill on the beqz two-exit. 2.3.3
// reproduces CW's exact branch layout. objdiff 100.0.
extern int func_001C22A0(void *p);
extern void func_001C6380(void *p);

int func_001F5490(void *p) {
    if (func_001C22A0(p) != 0) {
        return 3;
    }
    func_001C6380(p);
    return 1;
}
