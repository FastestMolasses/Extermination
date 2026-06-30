// NEARMISS func_001D4DA0  (vram 0x001D4DA0, 0x80 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation: instruction structure byte-identical to confirmed siblings func_001D39A0/3900/3CF0, but mwcc colors the bank-base pointer (D_00816440) and the 0x30 tag literal into the opposite two registers vs target. Matched siblings anchor that coloring via a trailing call pas...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 98.9% (mwcc233). VIF command-record writer: after the fixed setup
// calls (vif_build_unpack_const(0), vif_append_ref_tag(0,&D_00237180),
// func_001D1F80(0,1,0)) it stages a 0x10-byte record into the per-slot list at
// D_00275670[0x10]: tag byte 0x30, the bank base D_00816440 + (state[0x9c]<<7),
// a halfword count of 8, then advances the slot write pointer by 0x10. Sibling
// of the matched func_001D39A0/3900/3CF0 (differ only in the func_001D1F80 args
// and bank-base global). Sole residual: regalloc colors the bank-base pointer
// and the 0x30 literal into the opposite two registers because, unlike the
// matched siblings, this variant has no trailing call to anchor 0x30. Permuter.
extern char *D_00275670;
extern char D_00237180[0x10000];
extern char D_00816440[0x10000];
extern void vif_build_unpack_const(int a0);
extern void vif_append_ref_tag(int a0, char *a1);
extern void func_001D1F80(int a0, int a1, int a2);

void func_001D4DA0(void)
{
    char *t0;
    char *ptr;
    char *v1;
    int t1;
    char **a3;

    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_00237180);
    func_001D1F80(0, 1, 0);
    v1 = D_00275670;
    t0 = D_00816440;
    t1 = *((int *)(v1 + 0x9c));
    a3 = (char **)v1;
    ptr = a3[4];
    t1 = t1 << 7;
    t0 = t0 + t1;
    ptr[3] = 0x30;
    ptr = a3[4];
    *((char **)(ptr + 4)) = t0;
    ptr = a3[4];
    *((short *)ptr) = 8;
    ptr = a3[4];
    a3[4] = ptr + 0x10;
}
