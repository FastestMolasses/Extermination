// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame subsystem tick / mode dispatcher. Re-inits two related objects via
// func_001DEEE0 (passed gp-rel global D_00275670 plus offsets 0x2470 and 0x2490),
// then runs three gated stages keyed on func_001D2910(mode):
//   - if func_001D2910(1) is nonzero, run func_001DDAA0().
//   - if func_0015D2F0() == 2 (state 2) AND byte D_008106C6 == 2 AND
//     func_001D2910(6) is nonzero, run func_001DDB70().
//   - if func_001D2910(7) is nonzero, run func_001DFF70().
// Returns void; sq/lq $ra frame (0x10).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build lowers the
// nested state-2 guard with a merged-tail / inverted branch sense (it reorders the
// jal func_001D2910(6) and its addiu a0,0x6 across the bne), leaving a
// branch-lowering residual (89.7%). 2.3.3 matches the target's two-exit shape
// byte-identical. D_00275670 is gp-rel (-sdatathreshold 4); D_008106C6 is forced
// absolute by the [8]-array over-declaration (idiom #20). Verified objdiff 100.0%.
extern int D_00275670;
extern unsigned char D_008106C6[8];
extern void func_001DEEE0(int);
extern int func_001D2910(int);
extern void func_001DDAA0(void);
extern int func_0015D2F0(void);
extern void func_001DDB70(void);
extern void func_001DFF70(void);

void func_001DDA00(void) {
    func_001DEEE0(D_00275670 + 0x2470);
    func_001DEEE0(D_00275670 + 0x2490);
    if (func_001D2910(1) != 0) {
        func_001DDAA0();
    }
    if (func_0015D2F0() == 2) {
        if (D_008106C6[0] == 2 && func_001D2910(6) != 0) {
            func_001DDB70();
        }
    }
    if (func_001D2910(7) != 0) {
        func_001DFF70();
    }
}
