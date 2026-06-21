// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// World-hit surface-impact resolver on the bullet path (docs/FINDINGS.md
// "WEAPON SYSTEM" §7). func_001860A0(a, b) forwards its args to the ray query
// func_0019B6C0(a, b); on miss returns 0. On hit: copies the spad hit point
// into the FX position via func_001031E0(&D_70003620, &D_700031B0), builds the
// FX normal vec4 at 0x70003630..0x7000363C from the hit record (*0x700031D0)
// +0x24/+0x28/+0x2C with w = 1.0f, then keys the impact on the record's
// surface-type byte +0x1A: 0x5A -> FX func_001EFD90(0x8000002C, pos, normal);
// 0x5B -> FX 0x80000026 + flash func_001E8B90(&pos, 1.0f); 0x5C -> FX
// 0x80000067; returns 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. The
// stub's documented ~96% cap (three dead `b; <dup-instr>` CW 2.3.1 artifacts:
// the dead addiu 0x5B/0x5C compare-chain dups and the dead paddub return-0
// dup, plus the early-return beqz with paddub-in-delay-slot) is exactly the
// branch-lowering / delay-slot wall that 2.3.3 reproduces but 991202 does not
// (991202 tops out at 91.6%). Verified objdiff 100.0% vs
// build/expected/func_001860A0.o.
extern int func_0019B6C0(int a, int b);
extern void func_001031E0(void *dst, void *src);
extern void func_001EFD90(int id, void *pos, void *normal);
extern void func_001E8B90(void *pos, float f);
extern char D_70003620;
extern char D_700031B0;
extern char D_70003630;

int func_001860A0(int a, int b) {
    char *rec;
    unsigned char surf;

    if (func_0019B6C0(a, b) != 0) {
        func_001031E0(&D_70003620, &D_700031B0);
        rec = *(char **)0x700031D0;
        *(float *)0x70003630 = *(float *)(rec + 0x24);
        *(float *)0x70003634 = *(float *)(rec + 0x28);
        *(float *)0x70003638 = *(float *)(rec + 0x2C);
        *(float *)0x7000363C = 1.0f;
        surf = *(unsigned char *)(rec + 0x1A);
        if (surf == 0x5A) {
            func_001EFD90(0x8000002C, &D_70003620, &D_70003630);
        } else if (surf == 0x5B) {
            func_001EFD90(0x80000026, &D_70003620, &D_70003630);
            func_001E8B90(&D_70003620, 1.0f);
        } else if (surf == 0x5C) {
            func_001EFD90(0x80000067, &D_70003620, &D_70003630);
        }
        return 1;
    }
    return 0;
}
