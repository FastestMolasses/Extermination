// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame position integrator. Writes the three float deltas (fparg0..2)
// plus a 1.0 w-component into the scratchpad block at 0x700038A0, calls
// func_001026A0(scratch, &actor[0xD0], scratch) (transform/rotate the delta by
// the actor's orientation at +0xD0), then adds the transformed result back
// into the actor's position at +0xB0/+0xB4/+0xB8.
//
// Byte-identical on BOTH builds (991202 and 2.3.3); 991202 chosen.
// The scratchpad is accessed at absolute address 0x700038A0; D_700038A0 is the
// named alias used for the relocated %hi/%lo references in the call. Verified
// objdiff 100% vs build/expected/func_00128830.o.
extern int func_001026A0(void *, void *, void *);
extern float D_700038A0;

void func_00128830(char *arg0, float fparg0, float fparg1, float fparg2) {
    *(float *)0x700038A0 = fparg0;
    *(float *)0x700038A4 = fparg1;
    *(float *)0x700038A8 = fparg2;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038A0, arg0 + 0xD0, &D_700038A0);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700038A0;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700038A4;
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700038A8;
}
