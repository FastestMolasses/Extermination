// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 128
//
// Actor state-transition guard: if BOTH velocity floats (+0x224, +0x22C) are
// zero AND flag bit 0x2 of byte +0xF is clear, bail (return 0). Otherwise pick
// a timer constant into gp-rel global D_00275B14 by the actor's anim-state byte
// +0x5 (==0x10 -> 0x34) or step counter byte +0x6 (>=0x28 -> 0x36, else 0x1E),
// arm state bytes (+0x4=2, +0x5=7, +0x6=0), return 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (which
// diverges on the byte +0x6 compare lowering, 87.5%). Verified objdiff 100.0
// vs build/expected/func_00181D70.o. Notes on the C shape required to match:
//  * `float z = 0.0f` + `x == z` (NOT `x == 0.0f`) flips c.eq.s to the f0,f1
//    operand order of the target.
//  * the step test is written `>= 0x28` (then 0x36) so mwcc emits the signed
//    `slti v0; bnez v0` value-kept form (the `< 0x28` form gives slti $at +
//    beqz, a near-miss).
//  * -sdatathreshold 128 is required for D_00275B14 to address gp-relative
//    (R_MIPS_GPREL16) as in the target; lower thresholds emit absolute HI16/LO16.
extern int D_00275B14;

int func_00181D70(char *arg0) {
    float z = 0.0f;
    if (!(*(float *)(arg0 + 0x224) == z && *(float *)(arg0 + 0x22C) == z &&
          (*(unsigned char *)(arg0 + 0xF) & 2) == 0)) {
        if (*(unsigned char *)(arg0 + 5) == 0x10) {
            D_00275B14 = 0x34;
        } else if (*(unsigned char *)(arg0 + 6) >= 0x28) {
            D_00275B14 = 0x36;
        } else {
            D_00275B14 = 0x1E;
        }
        *(char *)(arg0 + 4) = 2;
        *(char *)(arg0 + 5) = 7;
        *(char *)(arg0 + 6) = 0;
        return 1;
    }
    return 0;
}
