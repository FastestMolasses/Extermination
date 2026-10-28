// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.91% NEARMISS).
// Fixes read from the original instructions:
// - D_002487D0 and the stack tint are a 16-byte struct copied by struct
//   assignment (not a u128 cast store); this gives the original's explicit
//   source-address computation and register choice in the opening block.
// - The struct copy comes before the arg0+0xC0 / arg0+0x1F0 pointer setup.
// - The four colour words are D_700038B0[0..3] of a volatile extern array,
//   not literal-address volatile stores/loads; this leaves the unconditional
//   branch at the end of the bright arm with an unfilled delay slot as in
//   the original.

// Weapon-fire / muzzle-flash trigger. Advances the RNG twice (func_00122BB8),
// snapshots the identity-ish quadword D_002487D0 to the stack, scales arg0's
// direction (+0xC0) by 260.0f into D_700038A0 via func_00103230, adds arg0's
// position (+0xA0) via func_001028B8, and probes for a target lock via
// func_0019A570(D_700038B0, D_700038A0, 7, 0x20). On success it marks the
// locked target (D_700031D4) hostile-flagged via func_00183AC0, copies the
// aim direction to D_700031B0, sets arg0->0xBC=1.0 and self->0x1F0+0x20=1 (a
// fire-cooldown/state flag), and re-snapshots the direction at +0x1F0+0x10.
// It then advances a global cycling color index D_00275B00 (mod 32), derives
// a randomized RGB triple from it (spread pattern differs by D_008106E0 --
// wide/bright when set, narrow/dim otherwise), packs the four color bytes
// into a single word, and fires a muzzle-flash particle via func_001CD520
// (position D_700038A0, packed 64-bit color-ish constant, size f12 twice +
// 2.0f, plus the packed RGBA word as an extra int arg). Finally it builds a
// second transform at self+0x1F0 and calls func_001E2BA0 with a
// D_008106E0-dependent tint vector (full white vs. dim amber) and the same
// 260.0f scale.
//
// Earlier keys that still hold:
// (1) D_008106E0 is a scalar global 5.8MB away from $gp yet the target uses
// absolute lui/%hi %lo addressing for it (not gp_rel) -- over-declaring it
// int[4] (array-over-declaration idiom) fixes this at -sdatathreshold 8.
// (2) The packed RGBA word ($t0 in the target) is genuinely computed but
// never visibly consumed by any subsequent call in the m2c/asm view; making
// it a real local and passing it as an extra 8th int argument to
// func_001CD520 (beyond the 3 int + 1 int64 + 3 float args) is what keeps
// mwcc from spilling it to the stack (which grew the frame from -0x50 to
// -0x60 when the value was merely computed-and-discarded). (3) The pack
// order must be evaluated highest-byte-first (BC<<24 | B8<<16 | B4<<8 | B0)
// to match the target's load order, not the naively "natural" B0-first
// order. (4) `s2[0xA] = 0x80` must be written through an explicit
// `unsigned char *` cast so mwcc materializes the positive 0x80 immediate
// (addiu +0x80) instead of sign-extending a `char` literal to -0x80.

extern int func_00122BB8(void);
extern void func_00103230(void *, void *, float);
extern void func_001028B8(void *, void *, void *);
extern void func_00102948(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern int func_00183AC0(char *);
extern void func_001031E0(void *, void *);
extern void func_001CD520(int, int, void *, long long, float, float, float, int);
extern void func_001E2BA0(void *, void *, void *, float);


typedef struct { float w[4]; } Q;
extern Q D_002487D0;
extern int D_00275B00;
extern int D_008106E0[4];
extern float D_700031B0[4];
extern int D_700038A0[4];
extern volatile int D_700038B0[4];
extern int D_700038C0[4];

void func_00185760(char *arg0) {
    Q sp40;
    char *s0;
    char *s2;
    int v1;
    float f12;
    int packed;
    char *c0;

    func_00122BB8();
    func_00122BB8();

    sp40 = D_002487D0;
    c0 = arg0 + 0xC0;
    s0 = arg0 + 0x1F0;
    func_00103230(D_700038A0, c0, 260.0f);
    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xA0);
    *(volatile int *)0x700038AC = 0x3F800000;
    func_00102948(D_700038B0, arg0 + 0xA0);

    if (func_0019A570(D_700038B0, D_700038A0, 7, 0x20) != 0) {
        s2 = *(char **)0x700031D4;
        if (s2 != 0 && func_00183AC0(s2) != 0) {
            *(unsigned char *)(s2 + 0xA) = 0x80;
        }
        func_001031E0(D_700038A0, D_700031B0);
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        *(int *)(s0 + 0x20) = 1;
        func_00102948(s0 + 0x10, D_700038A0);
    }

    D_00275B00 = (D_00275B00 + 3) & 0x1F;
    v1 = (func_00122BB8() >> 0xF) & 0x1F;

    if (D_008106E0[0] != 0) {
        D_700038B0[0] = v1 + 0x70;
        D_700038B0[1] = v1 + 0x40;
        D_700038B0[2] = v1 + 0x20;
        D_700038B0[3] = 0x80;
        f12 = 5.0f;
    } else {
        D_700038B0[0] = v1 + 0x50;
        D_700038B0[1] = 0;
        D_700038B0[2] = 0;
        D_700038B0[3] = 0x80;
        f12 = 3.0f;
    }

    packed = (D_700038B0[3] << 24)
        | (D_700038B0[2] << 16)
        | (D_700038B0[1] << 8)
        | D_700038B0[0];

    func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCLL, f12, f12, 2.0f, packed);
    func_00102948(D_700038C0, arg0 + 0x1F0);

    if (D_008106E0[0] != 0) {
        sp40.w[0] = 1.0f;
        sp40.w[1] = 0.6f;
        sp40.w[2] = 0.2f;
        sp40.w[3] = 1.0f;
    } else {
        sp40.w[0] = 0.7f;
        sp40.w[1] = 0.0f;
        sp40.w[2] = 0.0f;
        sp40.w[3] = 1.0f;
    }

    func_001E2BA0(D_700038C0, D_700038A0, &sp40, 260.0f);
}
