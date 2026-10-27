// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x700036A0 0x700036B4 0x700036C8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.25% NEARMISS).
// Fixes read from the original instructions:
//  - The clamp step is a compound assignment `+= 0.02f`: the original adds
//    the field and the constant in that operand order, which the
//    temp-then-store spelling did not reproduce.
//  - The three diagonal stores of the scale matrix at 0x700036A0 (+0x00,
//    +0x14, +0x28) are stores through relocated scratchpad externs
//    (idiom-32, opted in with // SPAD:), not integer-literal addresses. As
//    named objects mwcc knows they cannot alias D_00275B40, so it hoists
//    that global's reload above them as the original does; the literal
//    spelling kept the reload after the stores. No separate copy of
//    D_00275B40 is kept for the first clear.
//
// Saves the two bodies' positions (D_00275B40+0xC / +0x14, fields 0xC0..0xC8)
// into scratchpad 0x700038A0/0x700038B0, builds an identity matrix at
// D_700036A0 (func_001029C0), grows the scale at arg0+0xD4 by 0.02 clamped to
// 2.0 and writes it on the matrix diagonal, zeroes the positions, transforms
// each body's matrix at +0x90 by the scale matrix (func_001026D0), then
// restores the saved positions.

extern void func_001029C0(void *p);
extern void func_001026D0(void *dst, void *m, void *src);
extern int D_00275B40;
extern float D_700036A0[];
extern float D_700036B4[4];
extern float D_700036C8[4];

void func_0012DE90(char *arg0) {
    int p;

    p = D_00275B40;
    *(float *)0x700038A0 = *(float *)(*(int *)(p + 0xC) + 0xC0);
    *(float *)0x700038A4 = *(float *)(*(int *)(p + 0xC) + 0xC4);
    *(float *)0x700038A8 = *(float *)(*(int *)(p + 0xC) + 0xC8);
    *(float *)0x700038B0 = *(float *)(*(int *)(p + 0x14) + 0xC0);
    *(float *)0x700038B4 = *(float *)(*(int *)(p + 0x14) + 0xC4);
    *(float *)0x700038B8 = *(float *)(*(int *)(p + 0x14) + 0xC8);
    func_001029C0(D_700036A0);
    *(float *)(arg0 + 0xD4) += 0.02f;
    if (!(*(float *)(arg0 + 0xD4) <= 2.0f)) {
        *(float *)(arg0 + 0xD4) = 2.0f;
    }
    D_700036A0[0] = *(float *)(arg0 + 0xD4);
    D_700036B4[0] = *(float *)(arg0 + 0xD4);
    D_700036C8[0] = *(float *)(arg0 + 0xD4);
    *(int *)(*(int *)(D_00275B40 + 0xC) + 0xC0) = 0;
    *(int *)(*(int *)(D_00275B40 + 0xC) + 0xC4) = 0;
    *(int *)(*(int *)(D_00275B40 + 0xC) + 0xC8) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC0) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC4) = 0;
    *(int *)(*(int *)(D_00275B40 + 0x14) + 0xC8) = 0;
    func_001026D0((void *)(*(int *)(D_00275B40 + 0xC) + 0x90), D_700036A0, (void *)(*(int *)(D_00275B40 + 0xC) + 0x90));
    func_001026D0((void *)(*(int *)(D_00275B40 + 0x14) + 0x90), D_700036A0, (void *)(*(int *)(D_00275B40 + 0x14) + 0x90));
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC0) = *(float *)0x700038A0;
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC4) = *(float *)0x700038A4;
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0xC8) = *(float *)0x700038A8;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC0) = *(float *)0x700038B0;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC4) = *(float *)0x700038B4;
    *(float *)(*(int *)(D_00275B40 + 0x14) + 0xC8) = *(float *)0x700038B8;
}
