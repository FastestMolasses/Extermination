// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Ring-buffer dispatch over a 16-entry int table D_00281CF0[16] indexed by a
// gp-rel rolling cursor D_00275B34 (0..15, wraps & 0xF). Reads the slot at the
// current cursor; if it is -1 (empty) returns immediately. Otherwise, depending
// on two flag bytes: if D_00282155 is clear, calls func_001FA790(1, slot) and
// arms the result to -1; else if D_00282156 is clear, calls func_001FA790(2,
// slot) and arms -1. If the (possibly re-armed) value is -1, stores it back
// into the slot and advances the cursor (+1 & 0xF).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906). The addressing split is the key
// to byte-exactness under -sdatathreshold 4: the 1-byte cursor D_00275B34 is
// declared at its natural size so it lands in gp-relative sdata (matching the
// target's `lb/sb %gp_rel`), while the data table and the two flag bytes are
// declared as arrays larger than the 4-byte threshold (int[16], signed char[8])
// so they exceed sdata and use absolute lui %hi/%lo addressing — exactly the
// original split. The if/else-if chain reproduces the target's two flag tests
// and shared re-arm. Verified objdiff 100% vs build/expected/func_001FA5F0.o
// (991202 = 93.66%; the residual there is the wall-#13 delay-slot nop in the
// flag-test chain, which 2.3.3 leaves unfilled).
extern void func_001FA790(int, int);
extern signed char D_00275B34;
extern int D_00281CF0[16];
extern signed char D_00282155[8];
extern signed char D_00282156[8];

void func_001FA5F0(void) {
    int a1;

    a1 = D_00281CF0[D_00275B34];
    if (a1 == -1) {
        return;
    }
    if (D_00282155[0] == 0) {
        func_001FA790(1, a1);
        a1 = -1;
    } else if (D_00282156[0] == 0) {
        func_001FA790(2, a1);
        a1 = -1;
    }
    if (a1 == -1) {
        D_00281CF0[D_00275B34] = a1;
        D_00275B34 = (D_00275B34 + 1) & 0xF;
    }
}
