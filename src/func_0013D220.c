// func_0013D220 — byte-identical match.
//
// The C references the scratchpad pointer 0x700031D0 as a relocated extern,
// as the original did; splat renders that address as a bare literal. The
// `// SPAD: 0x700031D0` directive below symbolizes it in THIS file's expected
// object only (build.py _symbolize_scratchpad), so a plain objdiff of the
// compiled object reports 100.0% (m2-matching lane; it was 99.95/99.97 from
// the reloc-vs-literal spelling). The canonical report already showed 100%
// after inject_relocs.py. The boot ELF is byte-identical either way.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0

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
