// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-tick timer/charge step. arg1 is a state struct (counter byte at +4),
// arg2 a float block (current at +0x10 i.e. float[4], target/seed at +0xC
// i.e. float[3]). On the first tick (counter byte == 0): seed the current
// value from +0xC and bump the counter byte. On later ticks: if the current
// value has run down to <= 0 return 1 (done), otherwise decrement it by 1.0
// and return 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// lone residual under 991202 was the clean-store delay-slot wall (#13) on
// the `sb` store - 991202 fills the beqz delay slot with a safe op where CW
// leaves a nop. The 2.3.3 build leaves that nop, so this readable C is
// byte-identical (objdiff 100%); 991202 is 96.15%. The else-if ordering
// (== 0 case first, both 0-returns merged into a single trailing return) is
// what reproduces CW's block layout and v0 register reuse.
int func_001B9BA0(int arg0, unsigned char *arg1, float *arg2) {
    if (arg1[4] == 0) {
        arg2[4] = arg2[3];
        arg1[4] += 1;
    } else if (arg2[4] <= 0.0f) {
        return 1;
    } else {
        arg2[4] = arg2[4] - 1.0f;
    }
    return 0;
}
