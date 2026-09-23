// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// NOT YET 100% (m3-matching lane): 52.6% -> 90.48%. Still force-listed in
// fill_unmatched SIZE_DRIFT_FORCE_ASM, so the linker uses the original .s.
// D_002821D0 is over-declared as an array (idiom #20) because the target
// stores it with lui/sw while the other three bytes/words are gp-relative;
// the volatiles keep the target's store order 50, 54, 21D0, 55. Residual: our
// `lui at,%hi(D_002821D0)` is scheduled one store earlier than the target's.
// D_002821D0 lies inside the 0x9C bytes cleared at D_002821B0 (+0x20), but
// spelling it as that field measured worse (89.95%).
//
// Semantics (target 0x001FC9B0): func_00121A28(D_002821B0, 0, 0x9C);
// D_00275C50 = D_0026EC10[0]; D_00275C54 = 0x80; D_002821D0 = D_00264D10;
// D_00275C55 = 0; return 1.
extern char D_002821B0[0x100];
extern int D_0026EC10[2];
extern char D_00264D10[8];
extern char *volatile D_002821D0[2];
extern volatile int D_00275C50;
extern volatile unsigned char D_00275C54;
extern volatile unsigned char D_00275C55;
extern void func_00121A28(void *, int, int);

int func_001FC9B0(void) {
    func_00121A28(D_002821B0, 0, 0x9C);
    D_00275C50 = D_0026EC10[0];
    D_00275C54 = 0x80;
    D_002821D0[0] = D_00264D10;
    D_00275C55 = 0;
    return 1;
}
