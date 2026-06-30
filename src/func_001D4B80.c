// NEARMISS func_001D4B80  (vram 0x001D4B80, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation: body/structure byte-identical to confirmed sibling func_001D4EB0, but mwcc colors bank-base/shift into caller-saved a4/a5 and keeps ctx in v1, while target uses saved-temps t0/t1/t2 and reuses a2 for the 0x8 literal; also arg-save order (s0-then-s1 vs s1-then-s0)....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 77.1% (mwcc233). VIF command-record writer (sibling of the matched
// func_001D4EB0 / func_001D49D0). After vif_build_unpack_const(0) and
// vif_append_ref_tag(s1, &D_00239C90), it loads the render-context pointer
// D_00275670, indexes slot s1 (t0 = ctx + s1*4) and writes a 0x10-byte VIF
// record into the per-slot list at t0[0x10]: tag byte 0x30 at +3, then the bank
// base D_00816640 + (ctx[0x9c] << 7) at +4, then a halfword count of 8 at +0,
// then advances the slot write pointer (t0[0x10]) by 0x10. Finishes with
// func_001D4A90(s1, s0). LOGIC and STRUCTURE are byte-for-byte correct vs the
// target (identical instruction sequence to the confirmed-matched func_001D4EB0
// for the record-write body).
//
// Sole residual is a register-allocation permutation: the target colors the
// working pointer / bank-base / shift into the saved-temp range t0/t1/t2 and
// reuses a2 (the dead ctx pointer) for the 0x8 literal, and saves the two
// incoming args in s0-then-s1 order; mwcc 2.3.3 instead colors base/shift into
// caller-saved a4/a5, keeps ctx live in v1, and saves s1-then-s0. The trailing
// func_001D4A90 call adds the cross-call pressure that tips mwcc's greedy
// coloring away from the target's choice. No source rewrite (var layout,
// inlining the base+shift, threshold 0/4/8, statement reordering) changes the
// coloring -- same class as the parked sibling func_001D4DA0. Permuter territory.
extern char *D_00275670;
extern char D_00816640[0x10000];
extern char D_00239C90[8];
extern void vif_build_unpack_const(int);
extern void vif_append_ref_tag(int, char *);
extern void func_001D4A90(int, int);

void func_001D4B80(int a0, int a1) {
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
    a3 = D_00816640;
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
