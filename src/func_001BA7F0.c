// NEARMISS func_001BA7F0  (vram 0x001BA7F0, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jal arg-setup / delay-slot scheduling order. Logic + structure are byte-exact under mwcc 2.3.3 except the very first func_00102948 call: the target sets up a0 (&D_700038A0) BEFORE the jal and fills the delay slot with a1 (actor->0x114 + 0xC0); mwcc swaps the two (a1 before jal, a0 in the slot). T...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/effect shake-decay step. Builds two scratchpad vectors
// (D_700038A0, D_700038B0) from the actor at arg0->0x114 (+0xC0 sub-object)
// via func_00102948, decays the shake magnitude at 0x700038B4 by 20.0f, and
// if func_0019A570(...,4,0) reports the region is still active, refreshes the
// vectors from a config object (*0x700031D0)+0x24..0x2C and re-applies the
// transform via func_001F9100(actor+0xC0, vecs, 5.0f).
// NEARMISS 99.56% (mwcc 2.3.3): logic + structure byte-exact except the very
// first func_00102948 call -- target sets up a0 (&D_700038A0) before the jal
// and fills the delay slot with a1 (actor+0xC0); mwcc swaps the two
// (a1 before jal, a0 in slot). Pure jal arg-setup / delay-slot scheduling
// order; tried temp-ptr, assign-in-arg, and opt-level sweeps -- mwcc's slot
// filler for this call is fixed. The two instructions are identical, only
// reordered.

extern void func_00102948(void *dst, void *src);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001F9100(int handle, void *a, void *b, float t);

extern int D_700038A0;
extern int D_700038B0;
extern int D_700031B0;

void func_001BA7F0(unsigned char *arg0) {
    int *src;

    func_00102948(&D_700038A0, (void *)(*(int *)(arg0 + 0x114) + 0xC0));
    func_00102948(&D_700038B0, &D_700038A0);
    *(float *)0x700038B4 = *(float *)0x700038B4 - 20.0f;
    if (func_0019A570(&D_700038A0, &D_700038B0, 4, 0) != 0) {
        func_00102948(&D_700038A0, &D_700031B0);
        src = *(int **)0x700031D0;
        *(float *)0x700038B0 = *(float *)((unsigned char *)src + 0x24);
        *(float *)0x700038B4 = *(float *)((unsigned char *)src + 0x28);
        *(float *)0x700038B8 = *(float *)((unsigned char *)src + 0x2C);
        func_001F9100(*(int *)(arg0 + 0x114) + 0xC0, &D_700038A0, &D_700038B0, 5.0f);
    }
}
