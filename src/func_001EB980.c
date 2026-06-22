// COMPILER: mwcc233
// CFLAGS: -O4,p
//
// Animation/effect setup: pulls two floats (+0x54, +0x5C) from the global
// state record *D_00275C34 and calls func_001CFB50(&D_0081F8F0, 0, arg0,
// f54, f5C, 1.0f, ~1e-6f, 3.0f), then fires two func_001CFBE0 events on the
// same D_0081F8F0 object with descriptor tables D_00255FB0 and D_00256040
// (mode 1, trailing arg 0). The 0x358637BD constant is exactly the float
// 9.999999974752427e-07. D_0081F8F0/D_00255FB0/D_00256040 declared as arrays
// to force absolute (%hi/%lo) addressing while the small gp-rel D_00275C34
// keeps its %gp_rel load (default sdata threshold).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 build
// the lone residual is the two func_001CFBE0 calls' delay slots (991202 fills
// them with the safe `paddub a0,s0` hoist; CW leaves the arg-setup before the
// jal and fills with `paddub t0,zero,zero`). 2.3.3 reproduces CW exactly.
// Verified objdiff 100% vs build/expected/func_001EB980.o.
extern void func_001CFB50(void *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern char *D_00275C34;
extern char D_00255FB0[256];
extern char D_00256040[256];
extern char D_0081F8F0[256];

void func_001EB980(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 1, D_00255FB0, D_0081F8F0, 0);
    func_001CFBE0(arg1, 1, D_00256040, D_0081F8F0, 0);
}
