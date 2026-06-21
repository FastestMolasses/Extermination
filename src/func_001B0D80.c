// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// If float field +0xB4 < -200.0f, set state byte +0x04 = 3 and return 1;
// otherwise return 0. The early-return shape is required to reproduce the
// CW branch lowering: a `b` to the shared epilogue plus a dead trailing
// `paddub v0,zero,zero` block.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the residual is the CW branch-lowering / clean-store delay-slot
// wall. 2.3.3 is byte-identical. Verified objdiff 100.0 vs
// build/expected/func_001B0D80.o.
int func_001B0D80(char *arg0) {
    if (*(float *)(arg0 + 0xB4) < -200.0f) {
        *(char *)(arg0 + 4) = 3;
        return 1;
    }
    return 0;
}
