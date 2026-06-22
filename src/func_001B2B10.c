// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Effect/transform setup: initializes the global effect block at D_70003400
// (func_001029C0), seeds it from self+0xC4 as a float (func_00102BB0), then
// dispatches the shared apply pair func_001026A0(arg1, &D_70003400, arg2).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. The
// pinned build's scheduler perturbs delay-slot/register placement around the
// three back-to-back jals and caps at 69.29%; 2.3.3 reproduces the exact
// machine code (objdiff 100.0).
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f12);
extern void func_001026A0(int a0, void *a1, int a2);
extern int D_70003400;

void func_001B2B10(char *arg0, int arg1, int arg2) {
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, *(float *)(arg0 + 0xC4));
    func_001026A0(arg1, &D_70003400, arg2);
}
