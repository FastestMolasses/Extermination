// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Weapon/sight aim-angle helper. Calls func_001D2590(224.0f, ang) where ang =
// 0.0174532925f * (5 + 45*(1 - arg0)) (degrees->radians of a 5..50deg sweep
// driven by arg0 in [0,1]). Reads f21 = world struct (D_00275670) +0xF8, then
// computes a target value f13 from +0xFC: if func_001B0070()&0x80 OR the global
// mode byte D_00810700==0x11, f13 = (+0xFC) unchanged; else when (+0xFC) < 450
// f13 = v + arg0*(450 - v), otherwise f13 = v + 200*arg0. Tail-calls
// func_0021B970(f21, f13).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 build the
// lone residuals are wall #13 (it fills two clean-store/branch delay slots that
// CW leaves as nop) -> 95.94%. 2.3.3 leaves the nops, giving byte-identical code.
// D_00810700 is declared as an array (size > threshold) with -sdatathreshold 4 so
// it is addressed absolutely (lui/%hi) while the 4-byte pointer D_00275670 stays
// gp-relative. Verified objdiff 100% vs build/expected/func_001D2610.o.
extern char *D_00275670;
extern unsigned char D_00810700[8];
extern void func_001D2590(float, float);
extern int func_001B0070(void);
extern void func_0021B970(float, float);

void func_001D2610(float arg0) {
    float f21;
    float f13;
    float fc;

    func_001D2590(224.0f, 0.017453292f * (5.0f + (45.0f * (1.0f - arg0))));
    f21 = *(float *)(D_00275670 + 0xF8);
    if (func_001B0070() & 0x80) {
        f13 = *(float *)(D_00275670 + 0xFC);
    } else if (D_00810700[0] == 0x11) {
        f13 = *(float *)(D_00275670 + 0xFC);
    } else {
        fc = *(float *)(D_00275670 + 0xFC);
        if (fc < 450.0f) {
            f13 = fc + arg0 * (450.0f - fc);
        } else {
            f13 = fc + 200.0f * arg0;
        }
    }
    func_0021B970(f21, f13);
}
