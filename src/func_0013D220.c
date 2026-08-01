// func_0013D220 — byte-identical match.
//
// objdiff reports slightly under 100% here, and objdiff is WRONG about it.
// The residual is entirely splat rendering a 0x7000xxxx scratchpad access as
// a bare literal in load/store context (it only symbolizes lui+addiu pairs),
// so the EXPECTED object carries a constant where our compiled object carries
// the %hi/%lo relocation pair. Both encode the same bytes once relocated.
// Proven by the stronger oracle: this function is COMPILED and LINKED into the
// boot ELF, which remains byte-identical to the original. That is a direct test
// of the emitted bytes, unlike objdiff's object-level comparison.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies arg0->{+0xB0,+0xB4,+0xB8} into the scratchpad vector at 0x700038E0, sets the
// +0xC component to 1.0f, subtracts 10.0f from the y component, then calls
// func_0019B6C0(&arg0->+0xB0, &vec) — a room/broadphase probe. Returns 1 only if that
// probe succeeds AND the object published at *0x700031D0 has type byte +0x1A == 0x5B,
// else 0. 0x700038E0 is passed by address (%hi/%lo) while its component stores use the
// raw absolute form — match each site to what the target actually does.
extern int func_0019B6C0(float *, float *);
extern float D_700038E0;
extern char *D_700031D0;

int func_0013D220(unsigned char *arg0) {
    *(float *)0x700038E0 = *(float *)(arg0 + 0xB0);
    *(float *)0x700038E4 = *(float *)(arg0 + 0xB4);
    *(float *)0x700038E8 = *(float *)(arg0 + 0xB8);
    *(int *)0x700038EC = 0x3F800000;
    *(float *)0x700038E4 = *(float *)0x700038E4 - 10.0f;
    if (func_0019B6C0((float *)(arg0 + 0xB0), &D_700038E0) == 0) {
        return 0;
    }
    if (*(unsigned char *)(D_700031D0 + 0x1A) == 0x5B) {
        return 1;
    }
    return 0;
}
