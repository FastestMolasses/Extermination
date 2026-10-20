// COMPILER: eegcc
// CFLAGS: -O2
//
// Calls func_001152D8 when the volatile word D_0027F740[0x11] is nonzero.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern volatile int D_0027F740[];
extern void func_001152D8(void);
void func_001152B0(void) {
    if (D_0027F740[0x11] != 0) func_001152D8();
}
