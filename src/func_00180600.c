// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds a transform/parameter pair in the scratchpad and applies it to an
// actor at arg0. Writes a vec4 to scratch block 0x700038A0 = {0, f1, f2, 1.0},
// and a second vec4 to 0x700038B0 = {(arg1?f0:-f0), f1, f2, 1.0} (the X is
// negated when arg1 == 0, i.e. a mirror/flip about X). Then runs two
// func_001026A0 transforms (0x700038C0 = M(arg0+0xD0) applied to the first vec,
// 0x700038D0 = M(arg0+0xD0) applied to the second) and dispatches func_0019AFE0
// with mode 7. The 0x700038xx scratchpad stores use raw absolute casts (lui $at,
// 0x7000 form); the call args use the named &D_70003xxx symbols (%hi/%lo form).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 (92.7%) is the trailing-call delay-slot fill on the
// final func_0019AFE0 (mwcc991202 hoists `paddub a0,s0,zero` ahead and leaves
// the slot differently). 2.3.3 leaves the matching schedule, so this readable C
// is byte-identical. Verified objdiff 100.0% vs build/expected/func_00180600.o.
extern void func_001026A0(float *, int, float *);
extern void func_0019AFE0(int, float *, float *, int);
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;

void func_00180600(int arg0, int arg1, float f0, float f1, float f2) {
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = f1;
    *(float *)0x700038A8 = f2;
    *(int *)0x700038AC = 0x3F800000;
    if (arg1 == 0) {
        *(float *)0x700038B0 = -f0;
        *(float *)0x700038B4 = f1;
        *(float *)0x700038B8 = f2;
        *(int *)0x700038BC = 0x3F800000;
    } else {
        *(float *)0x700038B0 = f0;
        *(float *)0x700038B4 = f1;
        *(float *)0x700038B8 = f2;
        *(int *)0x700038BC = 0x3F800000;
    }
    func_001026A0(&D_700038C0, arg0 + 0xD0, &D_700038A0);
    func_001026A0(&D_700038D0, arg0 + 0xD0, &D_700038B0);
    func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
}
