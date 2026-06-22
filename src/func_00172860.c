// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame spin/rotation update for an actor (arg0). If the spin angle
// accumulator +0xC0 has decayed to <= 0 and the actor's owner state word
// (*(u16*)(*(int*)(arg0+0x20)+0x2E)) is not 1, the spin is killed (+0xC0 and
// +0x38 zeroed) and we bail. Otherwise advance the phase +0x38 by 0.15707964
// (pi/20) rad, wrap it with func_001B1470, decay +0xC0 by fparg0*sin(phase)
// (func_0011DE90) and wrap it too. Then builds a transform in scratch object
// D_700036A0 from those two angles (+0xC0,+0xC4) and the actor's offset +0x290,
// seeds a fixed translation {0, -20.5(0xC1A40000), 0, 1.0} in D_700038A0, and
// applies it to the actor's matrix at +0xB0 via func_001026A0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 only reaches ~89% (it
// schedules the float setup and fills delay slots differently). The +0xC0
// phase add uses `+=` (not `a = a + k`) to force the field into the higher fp
// reg so the add operand order matches CW. The D_700038A0 stores are literal
// absolute addresses (no reloc) but the pointer arg uses the symbol, matching
// the original mixed addressing. Verified objdiff 100% vs
// build/expected/func_00172860.o.
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, void *);
extern void func_001026A0(void *, void *, void *);
extern float func_0011DE90(float);
extern float func_001B1470(float);
extern int D_700036A0;
extern int D_700038A0;

void func_00172860(char *arg0, float fparg0) {
    if (*(float *)(arg0 + 0xC0) <= 0.0f) {
        if (*(unsigned short *)(*(int *)(arg0 + 0x20) + 0x2E) != 1) {
            *(float *)(arg0 + 0xC0) = 0.0f;
            *(int *)(arg0 + 0x38) = 0;
            return;
        }
    }
    *(float *)(arg0 + 0x38) += 0.15707964f;
    *(float *)(arg0 + 0x38) = func_001B1470(*(float *)(arg0 + 0x38));
    *(float *)(arg0 + 0xC0) = fparg0 * func_0011DE90(*(float *)(arg0 + 0x38)) + *(float *)(arg0 + 0xC0);
    *(float *)(arg0 + 0xC0) = func_001B1470(*(float *)(arg0 + 0xC0));
    func_001029C0(&D_700036A0);
    func_00102B08(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC0));
    func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC4));
    func_00102918(&D_700036A0, &D_700036A0, arg0 + 0x290);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0xC1A40000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(arg0 + 0xB0, &D_700036A0, &D_700038A0);
}
