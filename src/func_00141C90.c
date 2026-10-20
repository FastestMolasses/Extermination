// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Guarded state reset for an entity e (arg0) and its state block s (arg1).
// Does nothing when e[0xD] has bit 0x80, the s+0x76 short is nonzero, the
// s+0x70 short is zero, the e+0x54 short is zero, s[0x80] lacks bit 2, or
// e[4] != 1. Otherwise, when the state byte e[5] is below 7 and not 5, it
// switches the entity to state 10 (e[5] = 10, sub-state e[6] = 0).
//
// Byte-matched (objdiff 100.0; also 100 with 991202 and 2.4). Matching keys:
// the state byte is cached in an int so the range test is signed, and the
// `v >= 7 || v == 5` early return gives the target's compare-then-skip shape
// (an if-nest gives a single inverted branch instead).

void func_00141C90(unsigned char *e, char *s) {
    int v;

    if (e[0xD] & 0x80) return;
    if (*(short *)(s + 0x76) != 0) return;
    if (*(short *)(s + 0x70) == 0) return;
    if (*(short *)(e + 0x54) == 0) return;
    if (!(s[0x80] & 2)) return;
    if (e[4] != 1) return;
    v = e[5];
    if (v >= 7 || v == 5) return;
    e[5] = 10;
    e[6] = 0;
}
