// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Audio/system init sequence: calls func_001D25F0 with float 480.0f (0x43F00000),
// then func_001D2830 seven times with (id, 0) pairs for ids 0, 2, 1, 0x24, 0x20,
// 0x21, 0x22, then func_001D2610 with float 0.0f. No return value.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under 991202
// the func_001D25F0 float arg lui scheduling leaves a residual; 2.3.3 is byte-identical.
extern void func_001D25F0(float);
extern void func_001D2610(float);
extern void func_001D2830(int, int);

void func_001D2880(void) {
    func_001D25F0(480.0f);
    func_001D2830(0, 0);
    func_001D2830(2, 0);
    func_001D2830(1, 0);
    func_001D2830(0x24, 0);
    func_001D2830(0x20, 0);
    func_001D2830(0x21, 0);
    func_001D2830(0x22, 0);
    func_001D2610(0.0f);
}
