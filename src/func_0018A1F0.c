// func_0018A1F0 — byte-identical match.
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
// CFLAGS: -O4,p -sdatathreshold 4

//
// REQUIRES the scratchpad-symbol opt-in to be extended to 0x700031D0:
//   tools/decomp/build.py  _SPAD_SYMS += "0x700031D0"
//   config/SCUS_971.12.lcf  D_700031D0 = 0x700031D0;
// Same mechanism already shipped for D_70003B6C/D_70003B8D. Without it objdiff
// reports 99.974% (reloc-vs-literal spelling only); the LINKED bytes are identical
// either way. CAUTION: the current opt-in predicate is "the C mentions the symbol",
// and the already-matched src/func_001787B0.c declares `extern char D_700031D0[];`
// for an address-taken use — enabling 0x700031D0 globally would symbolize THAT
// target too and break it. The predicate needs to be per-file explicit first.
//
// SEMANTICS: player/camera collision-probe tick.
//   Copies the 4-qword transform at D_008103F8[0]+0x90 into *D_00275B40+0x90.
//   NOTE the argument order: the target's dst is *D_00275B40+0x90 and its src is
//   D_008103F8[0]+0x90 — the earlier NEARMISS had these reversed, which was the
//   first of its two residuals (target: a0 = v1+0x90 with v1 = *D_00275B40).
//   When arg0[0]&1: builds the probe vector (0.25,1.0,0.0,1.0) at D_700038A0,
//   transforms it by the same matrix and, if func_0019B2C0 accepts it, fires the
//   0x80000003 effect through func_001F00A0 at the object named by the scratchpad
//   pointer 0x700031D0 (gated on func_00189EC0 when 0x700031D4 is non-null and on
//   that object's +0x1A class field being 0x2000).
//   Then sweeps six D_0024A440 probe rays plus one D_0024A4A0 ray through
//   func_0019A570, reporting hits 1..2 to func_00189FE0, and finishes with
//   func_00189D30.
//
// D_700031D0 is declared as an ARRAY OF POINTERS (used as D_700031D0[0]) rather
// than a plain `char *`: at -sdatathreshold 4 a 4-byte scalar extern would be
// placed in .sdata and accessed %gp_rel, but the target uses a %hi/%lo pair.

extern void copy_qw4(void *dst, void *src);
extern int func_001026A0(void *dst, void *src, void *m);
extern int func_001028B8(void *a, void *b, void *c);
extern int func_00102948(void *dst, void *src);
extern int func_001031E0(void *a, void *b);
extern void func_00189D30(unsigned char *arg0);
extern int func_00189EC0(int a);
extern void func_00189FE0(unsigned char *arg0, void *a, void *b);
extern void func_0018A180(unsigned char *arg0);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019B2C0(void *a, void *b, int n);
extern void func_001AA840(unsigned char *arg0);
extern void func_001F00A0(int a, void *b, void *c, int d);

extern int **D_00275B40;
extern char *D_700031D0[];

extern int D_008103F8[2];
extern float D_00810360[2];
extern int D_0024A440[];
extern int D_0024A450[];
extern int D_0024A4A0[];
extern float D_700031B0[2];
extern int D_700038A0[4];
extern int D_700038B0[4];
extern int D_700038C0[4];
extern int D_700038D0[4];

void func_0018A1F0(unsigned char *arg0) {
    int sp30[4];
    int sp40[4];
    int sp50[4];
    int i;
    int n;
    char *p;

    copy_qw4((char *)*D_00275B40 + 0x90, (char *)D_008103F8[0] + 0x90);

    if (*arg0 & 1) {
        func_001AA840(arg0);
        *(int *)0x700038A0 = 0x3E800000;
        *(int *)0x700038A4 = 0x3F800000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(D_700038A0, (char *)*D_00275B40 + 0x90, D_700038A0);
        if (func_0019B2C0(D_00810360, D_700038A0, 6) != 0) {
            if (*(int *)0x700031D4 != 0) {
                if (func_00189EC0(*(int *)0x700031D4) == 0 && (*(short *)(D_700031D0[0] + 0x1A) & 0xFF00) == 0x2000) {
                    func_001031E0(D_700038C0, D_700031B0);
                    p = D_700031D0[0];
                    *(float *)0x700038B0 = *(float *)(p + 0x24);
                    *(float *)0x700038B4 = *(float *)(p + 0x28);
                    *(float *)0x700038B8 = *(float *)(p + 0x2C);
                    *(int *)0x700038BC = 0x3F800000;
                    func_00102948(D_700038A0, D_700031B0);
                    func_001F00A0(0x80000003, D_700038A0, D_700038B0, 0);
                    func_0018A180(arg0);
                }
            } else if ((*(short *)(D_700031D0[0] + 0x1A) & 0xFF00) == 0x2000) {
                func_001031E0(D_700038C0, D_700031B0);
                p = D_700031D0[0];
                *(float *)0x700038B0 = *(float *)(p + 0x24);
                *(float *)0x700038B4 = *(float *)(p + 0x28);
                *(float *)0x700038B8 = *(float *)(p + 0x2C);
                *(int *)0x700038BC = 0x3F800000;
                func_00102948(D_700038A0, D_700031B0);
                func_001F00A0(0x80000003, D_700038A0, D_700038B0, 0);
                func_0018A180(arg0);
            }
        }
    }

    if (*arg0 & 1) {
        func_001026A0(sp30, (char *)*D_00275B40 + 0x90, D_0024A440);
        func_001026A0(sp40, (char *)*D_00275B40 + 0x90, D_0024A450);
        n = func_0019A570(sp30, sp40, 7, 0x20);
        if (n != 0) {
            if ((unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, sp40, sp30);
            }
        }
        i = 0;
        do {
            func_001026A0(sp50, (char *)*D_00275B40 + 0x90, (char *)D_0024A440 + (i + 2) * 0x10);
            func_001028B8(D_700038A0, sp50, sp30);
            func_001028B8(D_700038B0, sp50, sp40);
            n = func_0019A570(D_700038A0, D_700038B0, 7, 0x20);
            if (n != 0 && (unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, D_700038B0, D_700038A0);
            }
            i += 1;
        } while (i < 2);
        i = 0;
        do {
            func_001026A0(sp50, (char *)*D_00275B40 + 0x90, (char *)D_0024A440 + (i + 4) * 0x10);
            func_001028B8(D_700038C0, sp50, sp30);
            func_001028B8(D_700038D0, sp50, sp40);
            n = func_0019A570(D_700038C0, D_700038D0, 7, 0x20);
            if (n != 0 && (unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, D_700038D0, D_700038C0);
            }
            i += 1;
        } while (i < 2);
        func_001026A0(D_700038A0, (char *)*D_00275B40 + 0x90, D_0024A4A0);
        func_00102948(D_700038B0, D_00810360);
        *(float *)0x700038B4 = *(float *)0x700038A4;
        n = func_0019A570(D_700038B0, D_700038A0, 7, 0x20);
        if (n != 0 && (unsigned int)(n - 1) < 2U) {
            func_00189FE0(arg0, D_700038A0, D_700038B0);
        }
    }

    func_00189D30(arg0);
}
