// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Two-stage gate/status query. Calls func_00112D18(1); if it returns nonzero,
// returns 0. Otherwise calls func_00113680() and returns 1 when it is zero or
// 2 when it is nonzero (`movn`-idiom select between the literals 1 and 2).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the 991202 build lowers the
// early-return guard with merged-tail / inverted branch sense (CW emits a
// distinct bnez-to-epilogue with its own delay slot), leaving a branch-lowering
// residual; 2.3.3 matches byte-identical. Verified objdiff 100% vs
// build/expected/func_00200730.o.
extern int func_00112D18(int);
extern int func_00113680(void);

int func_00200730(void) {
    if (func_00112D18(1) == 0) {
        return func_00113680() == 0 ? 1 : 2;
    }
    return 0;
}
