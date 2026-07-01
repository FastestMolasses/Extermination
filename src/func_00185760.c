// NEARMISS func_00185760  (vram 0x00185760, 0x2B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation confined to the opening prologue block. Structurally identical to the target for the entire body; the sole residual is register-coloring in the D_002487D0 quadword-copy + func_00103230 call-setup sequence (v0/v1/a0/a1 permuted, one branch-offset shift caused by tha...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

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
// NEARMISS 96.91% with mwcc 2.3.3 (mwcps2-2.3.3-000906). Keys applied:
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
// Residual: pure register-coloring permutation in the D_002487D0 copy /
// func_00103230 call-setup prologue -- parked for the permuter pass.

extern int func_00122BB8(void);
extern void func_00103230(void *, void *, float);
extern void func_001028B8(void *, void *, void *);
extern void func_00102948(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern int func_00183AC0(char *);
extern void func_001031E0(void *, void *);
extern void func_001CD520(int, int, void *, long long, float, float, float, int);
extern void func_001E2BA0(void *, void *, void *, float);

typedef int u128 __attribute__((mode(TI)));

extern u128 D_002487D0;
extern int D_00275B00;
extern int D_008106E0[4];
extern float D_700031B0[4];
extern int D_700038A0[4];
extern int D_700038B0[4];
extern int D_700038C0[4];

void func_00185760(char *arg0) {
    int sp40[4];
    char *s0;
    char *s2;
    int v1;
    float f12;
    int packed;
    char *c0;

    func_00122BB8();
    func_00122BB8();

    c0 = arg0 + 0xC0;
    s0 = arg0 + 0x1F0;
    *(u128 *)sp40 = D_002487D0;
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
        *(volatile int *)0x700038B0 = v1 + 0x70;
        *(volatile int *)0x700038B4 = v1 + 0x40;
        *(volatile int *)0x700038B8 = v1 + 0x20;
        *(volatile int *)0x700038BC = 0x80;
        f12 = 5.0f;
    } else {
        *(volatile int *)0x700038B0 = v1 + 0x50;
        *(volatile int *)0x700038B4 = 0;
        *(volatile int *)0x700038B8 = 0;
        *(volatile int *)0x700038BC = 0x80;
        f12 = 3.0f;
    }

    packed = (*(volatile int *)0x700038BC << 24)
        | (*(volatile int *)0x700038B8 << 16)
        | (*(volatile int *)0x700038B4 << 8)
        | *(volatile int *)0x700038B0;

    func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCLL, f12, f12, 2.0f, packed);
    func_00102948(D_700038C0, arg0 + 0x1F0);

    if (D_008106E0[0] != 0) {
        sp40[0] = 0x3F800000;
        sp40[1] = 0x3F19999A;
        sp40[2] = 0x3E4CCCCD;
        sp40[3] = 0x3F800000;
    } else {
        sp40[0] = 0x3F333333;
        sp40[1] = 0;
        sp40[2] = 0;
        sp40[3] = 0x3F800000;
    }

    func_001E2BA0(D_700038C0, D_700038A0, sp40, 260.0f);
}
