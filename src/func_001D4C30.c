// NEARMISS func_001D4C30  (vram 0x001D4C30, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation (identical to func_001D4B80, differs only in bank base D_00816740): target uses saved-temps t0/t1/t2 + reuses a2 for the 0x8 literal and saves args s0-then-s1; mwcc colors into a4/a5, keeps ctx in v1, saves s1-then-s0. Driven by trailing func_001D4A90 call. Permuter.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 77.1% (mwcc233). Identical to func_001D4B80 except the bank base is
// D_00816740 (vs D_00816640). VIF command-record writer (sibling of matched
// func_001D4EB0 / func_001D49D0): vif_build_unpack_const(0),
// vif_append_ref_tag(s1, &D_00239C90), then index slot s1 of context D_00275670
// (t0 = ctx + s1*4) and write a 0x10-byte VIF record at t0[0x10]: tag 0x30 at +3,
// D_00816740 + (ctx[0x9c]<<7) at +4, halfword 8 at +0; advance t0[0x10] by 0x10;
// finish with func_001D4A90(s1, s0). LOGIC/STRUCTURE byte-correct.
//
// Sole residual is the same register-allocation permutation as func_001D4B80:
// target colors into saved-temps t0/t1/t2 + reuses a2 for the 0x8 literal and
// saves args s0-then-s1; mwcc 2.3.3 uses caller-saved a4/a5, keeps ctx in v1,
// saves s1-then-s0. Driven by the trailing func_001D4A90 call. No source rewrite
// fixes the coloring -- permuter territory (cf. parked sibling func_001D4DA0).
extern char *D_00275670;
extern char D_00816740[0x10000];
extern char D_00239C90[8];
extern void vif_build_unpack_const(int);
extern void vif_append_ref_tag(int, char *);
extern void func_001D4A90(int, int);

void func_001D4C30(int a0, int a1) {
    char *ctx;
    char *t0;
    char *v1;
    char *a3;
    int t2;
    int s0;
    int s1;

    s0 = a1;
    s1 = a0;
    vif_build_unpack_const(0);
    vif_append_ref_tag(s1, D_00239C90);

    ctx = (char *)D_00275670;
    a3 = D_00816740;
    t2 = *(int *)(ctx + 0x9c);
    t0 = ctx + s1 * 4;
    v1 = *(char **)(t0 + 0x10);
    t2 = t2 << 7;
    v1[3] = 0x30;
    v1 = *(char **)(t0 + 0x10);
    a3 = a3 + t2;
    *(int *)(v1 + 4) = (int)a3;
    v1 = *(char **)(t0 + 0x10);
    *(short *)v1 = 8;
    v1 = *(char **)(t0 + 0x10);
    v1 = v1 + 0x10;
    *(char **)(t0 + 0x10) = v1;
    func_001D4A90(s1, s0);
}
